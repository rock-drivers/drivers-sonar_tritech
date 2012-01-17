#include "Micron.hpp"
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

void Micron::decodeSonarBeam(base::samples::SonarBeam &sonar_beam)
{
    LOG_DEBUG_S <<"decoding SonarBeam" << std::endl;
    if(sea_net_packet.getSenderType() != IMAGINGSONAR)
        throw std::runtime_error("Micron::getSonarBeam: Wrong device type");

    HeadData data;
    sea_net_packet.decodeHeadData(data);

    //check if the configuration is ok
    //be careful some values cannot be configured for the micron dst like SCANRIGHT (always == 1)
    //some other values are dynamically by the device like MOTOFF
    if( head_config.left_limit != data.left_limit ||
        head_config.right_limit != data.right_limit ||
        (head_config.head_control &(CONT|ADC8ON|INVERT)) !=(data.head_control &(CONT|ADC8ON|INVERT)))
    {
        LOG_ERROR_S << "Configuration of the beam differs from the desired one." << std::endl;
        LOG_ERROR_S << "left_limit " << head_config.left_limit << " / " << data.left_limit << std::endl; 
        LOG_ERROR_S << "rigth_limit " << head_config.right_limit << " / " << data.right_limit << std::endl; 
        LOG_ERROR_S << "head_control " << head_config.head_control << " / " << data.head_control << std::endl; 
        throw std::runtime_error("Configuration of the beam differs from the desired one.");
    }

    //copy data into SonarBeam
    sonar_beam.time = base::Time::now();
    sonar_beam.sampling_interval  = ((data.ad_interval*640.0)*1e-9);
    sonar_beam.bearing     = base::Angle::fromRad(M_PI-(data.bearing/6399.0*2.0*M_PI));
    sonar_beam.speed_of_sound = 1500;
    sonar_beam.beamwidth_vertical = 35.0/180.0*M_PI;
    sonar_beam.beamwidth_horizontal = 3.0/180.0*M_PI;

    //TODO check if we are using low resolution 
    sonar_beam.beam.resize(data.data_bytes);
    memcpy(&sonar_beam.beam[0], data.scan_data, data.data_bytes);
}

void Micron::configure(const MicronConfig &config,uint32_t timeout)
{
    //some conversions
    int ad_interval = (((config.resolution/config.speed_of_sound)*2.0)*1e9)/640.0;
    int number_of_bins = config.max_distance/config.resolution;
    uint16_t left_limit = (((config.left_limit.rad+M_PI)/(M_PI*2.0))*6399.0);
    uint16_t right_limit = (((config.right_limit.rad+M_PI)/(M_PI*2.0))*6399.0);
    uint8_t motor_step_angle_size = config.angular_resolution.rad/(M_PI*2.0)*6399.0;
    uint8_t initial_gain = config.gain*210;
    uint16_t lockout_time = (config.min_distance*2.0/config.speed_of_sound*1e6);  //in microseconds 

    //check configuration 
    if(config.gain < 0.0 || config.gain > 1.0 ||
       config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0 ||
       ad_interval < 0 || ad_interval > 1500 || number_of_bins < 0 || number_of_bins > 1500 || config.min_distance < 0 ||
       config.min_distance > 98.0 )
    {
        throw std::runtime_error("Micron::Micron: invalid configuration.");
    }

    //generate head data
    head_config.V3B_params = 0x1D;
    head_config.head_type = IMAGINGSONAR;
    head_config.range_scale  = 40;
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
    head_config.lockout_time = lockout_time;
    head_config.minor_axis_dir = (0x40) | (0x06<<8);
    head_config.major_axis_pan = (0x01);

    //SCANRIGHT is always 1 for microns dst even if the flag is not set
    head_config.head_control =
        (((!config.low_resolution)?ADC8ON:0) | (config.continous?CONT:0) |
        (config.invert?INVERT:0) | RAW | HASMOT | REPLYASL | CHAN2 );
        
    iodrivers_base::Timeout time_out(timeout);
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(IMAGINGSONAR,
                                                           mtHeadCommand,
                                                           (uint8_t*)&head_config,
                                                            sizeof(head_config));
    LOG_DEBUG_S <<"Sent mtHeadCommand packet" << std::endl;
    writePacket(&packet[0],packet.size(),time_out.timeLeft());

    clear();

    //wait for an alive packet to check if the sonar is configured
    AliveData alive_data;
    while(!alive_data.ready && alive_data.no_config && !alive_data.config_send)
    {
        std::cout << "not ready" << std::endl;
        waitForPackage(mtAlive,time_out.timeLeft());
        sea_net_packet.decodeAliveData(alive_data);
    }

    //wait for head data 
    writeSendData(time_out.timeLeft());
    waitForPackage(mtHeadData,time_out.timeLeft());
}


