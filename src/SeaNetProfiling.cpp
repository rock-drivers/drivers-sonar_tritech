#include "SeaNetProfiling.hpp"
#include "SeaNetTypesInternal.hpp"
#include <base/logging.h>
#include <cmath>

namespace sea_net { 

enum HeadConfigFlags{
    ADAPTIVE_GAIN = 1,
    PRF_ALT = 2,
    SCANRIGHT = 4,
    INVERT = 8,
    MOTOFF = 16,
    TXOFF = 32,
    PRF_T10 = 64,
    CHAN2 = 128,
    PRF_FIRST = 256,
    HASMOT = 512,
    PRF_PINGSYNC = 1024,
    PRF_SCANSYNC = 2048,
    STARELLIM = 4096,
    PRF_MASTER = 8192,
    PRF_MIRROR = 16384,
    IGNORESENSOR = 32768
};

Profiling::Profiling() : SeaNet(PROFILINGSONAR)
{
}

Profiling::~Profiling() 
{
}

void Profiling::configure(const ProfilingConfig& config, uint32_t timeout)
{
    //some conversions
    uint16_t left_limit = (((M_PI-config.left_limit.rad)/(M_PI*2.0))*6399.0);
    uint16_t right_limit = (((M_PI-config.right_limit.rad)/(M_PI*2.0))*6399.0);
    uint8_t motor_step_angle_size = config.angular_resolution.rad/(M_PI*2.0)*6399.0;
    uint8_t ad_treshold = config.gain*210;
    uint8_t filt_gain = config.filt_gain*250;
    uint32_t freq_chan1_tx = (config.frequency_chan1 * 4294967296) / 32e6;
    uint32_t freq_chan1_rx = ((config.frequency_chan1 + 455000) * 4294967296) / 32e6;
    uint32_t freq_chan2_tx = (config.frequency_chan2 * 4294967296) / 32e6;
    uint32_t freq_chan2_rx = ((config.frequency_chan2 + 455000) * 4294967296) / 32e6;
    uint16_t slope_ch1 = 114 + ((config.frequency_chan1 - 600000) / 600000) * 36;
    uint16_t slope_ch2 = 114 + ((config.frequency_chan2 - 600000) / 600000) * 36;
    uint16_t pulse_length = (config.max_distance + 10) * 25 / 10;
    uint16_t range_scale = (uint16_t)(config.max_distance * 10.0) & 0x3FFF;
    uint16_t lockout_time = 542 - ((config.frequency_chan1 - 600000) / 600000) * 271;
    uint16_t head_control = PRF_ALT | PRF_FIRST | HASMOT | PRF_MASTER;
    if(config.select_channel == 2)
        head_control = head_control | CHAN2;
    
    LOG_INFO_S << "Configure Sonar";
    LOG_DEBUG_S << " right_limit:" << right_limit << " motor_step_angle_size:" << (int)motor_step_angle_size << " initial_gain:"
                << " lockout_time:" << lockout_time;

    //check configuration 
    if(config.gain < 0.0 || config.gain > 1.0 ||
       config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0 ||
        config.frequency_chan1 < 600000 || config.frequency_chan1 > 1200000 || config.frequency_chan2 < 600000 || config.frequency_chan2 > 1200000 ||
        config.filt_gain < 0.0 || config.filt_gain > 1.0 || config.select_channel < 1 || config.select_channel > 2 || config.max_distance > 16383.0 || config.max_distance < 0.0)
    {
        throw std::runtime_error("Profiling::Profiling: invalid configuration.");
    }
    double opening_angle = 0.0;
    if(config.left_limit.rad < 0.0 && config.right_limit.rad >= 0.0)
        opening_angle = config.left_limit.rad - (config.right_limit.rad - 2*M_PI);
    else
        opening_angle = config.left_limit.rad - config.right_limit.rad;
    if(std::ceil(opening_angle / config.angular_resolution.rad) > (SEA_NET_MAX_PACKET_SIZE - 51) / 2.0)
    {
        throw std::runtime_error("Profiling::Profiling: invalid configuration. To many samples per message. Decrease the opening angle or increase the angular resolution.");
    }

    speed_of_sound = config.speed_of_sound;

    //generate head data
    profiling_head_config.V3B_params = 0x1D;
    profiling_head_config.head_control = head_control;
    profiling_head_config.head_type = 5;
    profiling_head_config.txn_ch1 = freq_chan1_tx;
    profiling_head_config.txn_ch2 = freq_chan2_tx;
    profiling_head_config.rxn_ch1 = freq_chan1_rx;
    profiling_head_config.rxn_ch2 = freq_chan2_rx;
    profiling_head_config.pulse_length = pulse_length;
    profiling_head_config.range_scale = range_scale;
    profiling_head_config.left_limit = left_limit; 
    profiling_head_config.right_limit = right_limit;
    profiling_head_config.ad_threshold = ad_treshold;
    profiling_head_config.filt_gain = filt_gain;
    profiling_head_config.adaptive_gain_control_max = 107;
    profiling_head_config.adaptive_gain_control_set_point = 100;
    profiling_head_config.slope_ch1 = slope_ch1;
    profiling_head_config.slope_ch2 = slope_ch2;
    profiling_head_config.motor_step_delay_time = 25; // 25 to 40
    profiling_head_config.motor_step_angle_size = motor_step_angle_size;
    profiling_head_config.scan_time = 30;
    profiling_head_config.prf_spl = 0;
    profiling_head_config.prf_ctl2 = 0;
    profiling_head_config.lockout_time = lockout_time;
    profiling_head_config.minor_axis_dir = 1600;
    profiling_head_config.major_axis_pan = 1;
    profiling_head_config.crtl2 = 0;
    profiling_head_config.scan_z = 0;
    profiling_head_config.ad_threshold_ch1 = 0x32;
    profiling_head_config.ad_threshold_ch2 = 0x32;
    profiling_head_config.filt_gain_ch1 = 1;
    profiling_head_config.filt_gain_ch2 = 1;
    profiling_head_config.agc_max_ch1 = 0x6b;
    profiling_head_config.agc_max_ch2 = 0x6b;
    profiling_head_config.agc_set_point_ch1 = 0x64;
    profiling_head_config.agc_set_point_ch2 = 0x64;
    profiling_head_config.advanced_slope_ch1 = 110;
    profiling_head_config.advanced_slope_ch2 = 150;
    profiling_head_config.advanced_slope_delay_ch1 = 0;
    profiling_head_config.advanced_slope_delay_ch2 = 0;
    
    HeadCommand* head_config = reinterpret_cast<HeadCommand*>(&profiling_head_config);
    writeHeadCommand(*head_config, timeout);
}

void Profiling::decodeScan(base::samples::LaserScan& scan)
{
    LOG_DEBUG_S <<"decoding profiling scan" ;
    if(sea_net_packet.getSenderType() != PROFILINGSONAR)
        throw std::runtime_error("Micron::getSonarBeam: Wrong device type");

    ProfilingHeadData data;
    sea_net_packet.decodeHeadData(data);

    /*
    //check if the configuration is ok
    if( profiling_head_config.left_limit != data.left_limit ||
        profiling_head_config.right_limit != data.right_limit ||
        profiling_head_config.motor_step_angle_size != data.motor_step_angle_size || 
        (profiling_head_config.head_control &(PRF_ALT | CHAN2 |  PRF_FIRST | HASMOT | PRF_MASTER)) !=(data.head_control &(PRF_ALT | CHAN2 |  PRF_FIRST | HASMOT | PRF_MASTER)))
    {
        LOG_ERROR_S << "Configuration of the beam differs from the desired one." ;
        LOG_ERROR_S << "left_limit " << profiling_head_config.left_limit << " / " << data.left_limit ; 
        LOG_ERROR_S << "rigth_limit " << profiling_head_config.right_limit << " / " << data.right_limit ; 
        LOG_ERROR_S << "angle size " << profiling_head_config.motor_step_angle_size << " / " << data.motor_step_angle_size ; 
        LOG_ERROR_S << "head_control " << profiling_head_config.head_control << " / " << data.head_control ; 
        throw std::runtime_error("Configuration of the beam differs from the desired one.");
    }*/

    //copy data into LaserScan
    scan.time = base::Time::now();
    scan.start_angle = base::Angle::fromRad(-((double)data.right_limit/6399.0*2.0*M_PI)+M_PI).getRad();
    scan.angular_resolution = ((double)data.motor_step_angle_size)/6399.0*2.0*M_PI;
    scan.minRange = 300; // minimum range of the profiling sonar 0.3m
    if((data.range & 0xC000) == 0)
        scan.maxRange = (data.range & 0x3FFF) * 100;
    else
        scan.maxRange = 100000; //Hardcoded 100meter
    scan.speed = (data.data_count * scan.angular_resolution) / ((double)data.scan_time / 10000.0);
    
    if(data.head_control & 0x04){ //Do we scan left or right?
        for(unsigned int i = 0; i < data.data_count; i++){
                scan.speed *= -1.0;
                double distance = (data.scan_data[(data.data_count*2-1)-((i*2)+1)] | (data.scan_data[(data.data_count*2-1)-(i*2)]<<8)) * 1e-6 * speed_of_sound / 2.0;//Time in microsecounds to secounds (1e-6) * time of water in speed / twice the way (2.0)
                scan.ranges.push_back(distance*1000.0); //To millimeters 
        }
    }else{
        for(unsigned int i = 0; i < data.data_count; i++){
                double distance = (data.scan_data[(i*2)] | data.scan_data[(i*2)+1]<<8) * 1e-6 * speed_of_sound / 2.0;//Time in microsecounds to secounds (1e-6) * time of water in speed / twice the way (2.0)
                scan.ranges.push_back(distance*1000.0); //To millimeters 
        }
    }
}

}
