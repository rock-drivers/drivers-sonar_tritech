#include "SeaNetMicron.hpp"
#include "SeaNetTypesInternal.hpp"
#include <iodrivers_base/Timeout.hpp>
#include <iostream>
#include <base/logging.h>

using namespace sea_net;

enum HeadConfigFlags
{
    ADC8ON = 1, CONT = 2, SCANRIGHT = 4, INVERT = 8,
    MOTOFF = 16, TXOFF = 32, SPARE = 64, CHAN2 = 128,
    RAW = 256, HASMOT = 512, APPLYOFFSET = 1024, PINGPONG = 2048,
    STARELLIM = 4096, REPLYASL = 8192, REPLYTHR = 16384,
    IGNORESENSOR = 32768
};

Micron::Micron(): SeaNet(IMAGINGSONAR)
{ 
}

Micron::~Micron() 
{
}

void Micron::decodeSonar(base::samples::Sonar &sonar)
{
    LOG_DEBUG_S <<"decoding SonarBeam" ;
    if(sea_net_packet.getSenderType() != IMAGINGSONAR)
        throw std::runtime_error("Micron::getSonarBeam: Wrong device type");

    ImagingHeadData data;
    sea_net_packet.decodeHeadData(data);

    //check if the configuration is ok
    //be careful some values cannot be configured for the micron dst like SCANRIGHT (always == 1)
    //some other values are dynamically by the device like MOTOFF
    if( head_config.left_limit != data.left_limit ||
        head_config.right_limit != data.right_limit ||
        head_config.motor_step_angle_size != data.motor_step_angle_size || 
        head_config.ad_low != data.ad_low || 
        head_config.ad_span != data.ad_span ||
        (head_config.head_control &(CONT|ADC8ON|INVERT)) !=(data.head_control &(CONT|ADC8ON|INVERT)))
    {
        LOG_ERROR_S << "Configuration of the beam differs from the desired one." ;
        LOG_ERROR_S << "left_limit " << head_config.left_limit << " / " << data.left_limit ; 
        LOG_ERROR_S << "rigth_limit " << head_config.right_limit << " / " << data.right_limit ; 
        LOG_ERROR_S << "angle size " << head_config.motor_step_angle_size << " / " << data.motor_step_angle_size ; 
        LOG_ERROR_S << "ad low " << head_config.ad_low << " / " << data.ad_low ; 
        LOG_ERROR_S << "ad span " << head_config.ad_span << " / " << data.ad_span ; 
        LOG_ERROR_S << "head_control " << head_config.head_control << " / " << data.head_control ; 
        throw std::runtime_error("Configuration of the beam differs from the desired one.");
    }

    //copy data into SonarBeam
    sonar.time = base::Time::now();
    sonar.speed_of_sound = speed_of_sound;
    sonar.beam_height = base::Angle::fromDeg(35.0);
    sonar.beam_width = base::Angle::fromDeg(3.0);

    //the micron dst is not using the lockout_time value
    //therefore we are removing the values here 
    //lockout_time is in microseconds and sampling_interval in sec
    double sampling_interval = ((640.0 * data.ad_interval) * 1e-9);
    size_t lockouts = head_config.lockout_time / (10e6 * sampling_interval);
    LOG_DEBUG_S << "Erasing " << lockouts<< " bins because of lockout_time." ;

    std::vector<uint8_t> raw_data;
    if((data.head_control & ADC8ON) == 1)
    {
        raw_data.resize(data.data_bytes,0);
        memcpy(&raw_data[0]+lockouts, data.scan_data+lockouts, data.data_bytes-lockouts);
    }
    else
    {
        //low resolution is used
        //this means each bin has 4 Bits instead of 8 Bits
        //scale it up to values between 0 and 255
        raw_data.resize(lockouts,0);
        for(int i=lockouts;i<data.data_bytes;++i)
        {
            raw_data.push_back((data.scan_data[i]&0x0F)*17);
            raw_data.push_back((data.scan_data[i] >> 4)*17);
        }
    }

    std::vector<float> sonar_data(raw_data.begin(), raw_data.end());
    std::transform(sonar_data.begin(), sonar_data.end(), sonar_data.begin(), std::bind2nd(std::divides<float>(), 255));

    sonar.bin_count = sonar_data.size();

    base::Angle bearing = base::Angle::fromRad(-(data.bearing / 6399.0 * 2.0 * M_PI) + M_PI);
    sonar.pushBeam(sonar_data, bearing);
}

void Micron::configure(const MicronConfig &config,uint32_t timeout)
{
    //some conversions
    int ad_interval = (((config.resolution/config.speed_of_sound)*2.0)*1e9)/640.0;
    int number_of_bins = config.max_distance/config.resolution;
    uint16_t left_limit = (((M_PI-config.left_limit.rad)/(M_PI*2.0))*6399.0);
    uint16_t right_limit = (((M_PI-config.right_limit.rad)/(M_PI*2.0))*6399.0);
    uint8_t motor_step_angle_size = config.angular_resolution.rad/(M_PI*2.0)*6399.0;
    uint8_t initial_gain = config.gain*210;
    uint16_t lockout_time = 2.0*config.min_distance/config.speed_of_sound*10e6;

    std::cout << "Configure Sonar";
    std::cout << "ad_interval:" << ad_interval << " number_of_bins:" << number_of_bins << " left_limit:" << left_limit
                << " right_limit:" << right_limit << " motor_step_angle_size:" << (int)motor_step_angle_size << " initial_gain:"
                << " lockout_time:" << lockout_time;

    //check configuration 
    if(config.gain < 0.0 || config.gain > 1.0 ||
       config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0 ||
       ad_interval < 0 || ad_interval > 1500 || number_of_bins < 0 || number_of_bins > 1500 )
    {
        if(config.gain < 0.0 || config.gain > 1.0){
            throw std::runtime_error("Micron::Micron: invalid Gain configuration.");
        }
        if(config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0){
            throw std::runtime_error("Micron::Micron: Invalid angular rad configuration.");
        }
        if(ad_interval < 0 || ad_interval > 1500){
            throw std::runtime_error("Micron::Micron: Invalid ad_interval configuration.");
        }
        if(number_of_bins < 0 || number_of_bins > 1500){
            throw std::runtime_error("Micron::Micron: Invalid numbers of bins.");
        }
        throw std::runtime_error("Micron::Micron: invalid configuration.");
    }

    speed_of_sound = config.speed_of_sound;

    //generate head data
    head_config.V3B_params = 0x1D;
    head_config.head_type = IMAGINGSONAR;
    head_config.left_limit   =left_limit; 
    head_config.right_limit  =right_limit;
    head_config.ad_span = 81;
    head_config.ad_low = 8;
    head_config.initial_gain_ch1 = initial_gain;
    head_config.initial_gain_ch2 = initial_gain;
    head_config.motor_step_delay_time = 25;
    head_config.motor_step_angle_size = motor_step_angle_size;
    head_config.ad_interval = ad_interval;
    head_config.number_of_bins = number_of_bins;
    head_config.max_ad_buff = (0xE8) | (3<<8);
    head_config.lockout_time = 100;
    head_config.minor_axis_dir = (0x40) | (0x06<<8);
    head_config.major_axis_pan = (0x01);
    head_config.lockout_time = lockout_time; 
    // Range scale does not control the sonar, only provides a way to note the current settings in a human readable
    // format. The lower 14 bits are the range scale * 10 units and the higher 2 bits are coded units:
    // 0: meters
    // 1: feet
    // 2: fathoms
    // 3: yards
    // Only the metric system is implemented for now.
    head_config.range_scale  = floor(config.max_distance * 10);

    //SCANRIGHT is always 1 for microns dst even if the flag is not set
    head_config.head_control =
        (((!config.low_resolution)?ADC8ON:0)|(config.continous?CONT:0)|RAW|HASMOT|REPLYASL|CHAN2|(config.invert?INVERT:0));
    
    writeHeadCommand(head_config,timeout);
}

void Micron::decodeEchoSounder(base::samples::RigidBodyState &state)
{
    LOG_DEBUG_S <<"decoding EchoSounder" ;
    if(sea_net_packet.getSenderType() != IMAGINGSONAR)
        throw std::runtime_error("Micron::getSonarBeam: Wrong device type");
    
    state.invalidate();
    std::vector<uint8_t> data;
    sea_net_packet.decodeAuxData(data);
    data.back() = '\0'; //make sure string ends correctly
    int depth = 0;
    if(sscanf((const char*)&data[0],"%dmm",&depth) == 1)
    {
        state.position[2] = 0.001*depth; //We want Si units not mm
        state.time = base::Time::now();
        state.cov_position(2,2) = 0.2;
        LOG_DEBUG_S <<"decoding EchoSounder: got depth:" << state.position[2];
    }
    else
        throw std::runtime_error("Cannot decode EchoSounder. Depth was not find in the package");
}


