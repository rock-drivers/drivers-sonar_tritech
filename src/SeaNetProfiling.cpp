#include "SeaNetProfiling.hpp"
#include "SeaNetTypesInternal.hpp"
#include <base/logging.h>
#include <cmath>
#include <aggregator/TimestampEstimator.hpp>

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
    timestamp_estimator = new aggregator::TimestampEstimator(base::Time::fromSeconds(2),base::Time::fromSeconds(0),INT_MAX);
}

Profiling::~Profiling() 
{
    delete timestamp_estimator;
}

struct ProfilingHeadCommand
{
    uint8_t V3B_params; uint16_t head_control; uint8_t head_type;
    uint32_t txn_ch1; uint32_t txn_ch2; uint32_t rxn_ch1; uint32_t rxn_ch2;
    uint16_t pulse_length; uint16_t range_scale; uint16_t left_limit;
    uint16_t right_limit; uint8_t ad_threshold; uint8_t filt_gain; uint8_t adaptive_gain_control_max;
    uint8_t adaptive_gain_control_set_point; uint16_t slope_ch1; uint16_t slope_ch2; uint8_t motor_step_delay_time;
    uint8_t motor_step_angle_size; uint16_t scan_time; uint16_t prf_spl;
    uint16_t prf_ctl2; uint16_t lockout_time; uint16_t minor_axis_dir;
    uint8_t major_axis_pan; uint8_t crtl2; uint16_t scan_z; uint8_t ad_threshold_ch1;
    uint8_t ad_threshold_ch2; uint8_t filt_gain_ch1; uint8_t filt_gain_ch2; uint8_t agc_max_ch1;
    uint8_t agc_max_ch2; uint8_t agc_set_point_ch1; uint8_t agc_set_point_ch2;
    uint16_t advanced_slope_ch1; uint16_t advanced_slope_ch2; uint16_t advanced_slope_delay_ch1;
    uint16_t advanced_slope_delay_ch2;

    ProfilingHeadCommand()
    {
        //clear everything 
        memset(this,0,sizeof(*this));
    }
} __attribute__ ((packed));

void Profiling::configure(const ProfilingConfig& config, uint32_t timeout)
{
    //check configuration 
    if(config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0 ||
        config.frequency_chan1 < 600000 || config.frequency_chan1 > 1200000 || config.frequency_chan2 < 600000 || config.frequency_chan2 > 1200000 ||
        config.select_channel < 1 || config.select_channel > 2 || config.max_distance > 16383.0 || config.max_distance < 0.0)
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

    //some conversions
    uint16_t slope_ch1 = 114 + ((config.frequency_chan1 - 600000) / 600000) * 36;
    uint16_t slope_ch2 = 114 + ((config.frequency_chan2 - 600000) / 600000) * 36;

    uint16_t head_control = HASMOT | PRF_MASTER;
    if(!config.continous)
        head_control |= PRF_ALT;
    if(config.select_channel == 2)
        head_control |= CHAN2;
    if(config.mode == PROFILING_FIRST)
        head_control |= PRF_FIRST;

    speed_of_sound = config.speed_of_sound;

    ProfilingHeadCommand profiling_head_config;
    //generate head data
    profiling_head_config.V3B_params = 0x1D;

    profiling_head_config.head_control = head_control;
    profiling_head_config.head_type = 5;
    profiling_head_config.prf_ctl2 = (config.mode == PROFILING_MAX) ? 1 : 0;
    profiling_head_config.crtl2 = 0;
    profiling_head_config.prf_spl = 0;
    profiling_head_config.minor_axis_dir = 1600;
    profiling_head_config.major_axis_pan = 1;
    profiling_head_config.scan_z = 0;

    profiling_head_config.txn_ch1 = (config.frequency_chan1 * 4294967296) / 32e6;
    profiling_head_config.txn_ch2 = (config.frequency_chan2 * 4294967296) / 32e6;
    profiling_head_config.rxn_ch1 = ((config.frequency_chan1 + 455000) * 4294967296) / 32e6;
    profiling_head_config.rxn_ch2 = ((config.frequency_chan2 + 455000) * 4294967296) / 32e6;
    uint16_t left_limit = (((M_PI-config.left_limit.rad)/(M_PI*2.0))*6399.0);
    profiling_head_config.left_limit = left_limit; 
    uint16_t right_limit = (((M_PI-config.right_limit.rad)/(M_PI*2.0))*6399.0);
    profiling_head_config.right_limit = right_limit;
    // Value obtained by reverse engineering the communication between the
    // windows application and the seaking ...
    profiling_head_config.lockout_time = config.min_distance * 1356;
    profiling_head_config.pulse_length = 25 + config.max_distance * 25 / 10;
    profiling_head_config.range_scale = config.max_distance * 10;
    // Magic number I also got from reverse engineering ... I've picked 13.56
    // because it is really close to what the windows app does *and* is the same
    // than the 1356 value for lockout_time which is also a time-to-distance
    // conversion. Or how to invent correlations where there is probably none.
    profiling_head_config.scan_time = 13.56 * config.max_distance;

    profiling_head_config.motor_step_delay_time = 25; // 25 to 40
    uint8_t motor_step_angle_size = config.angular_resolution.rad/(M_PI*2.0)*6399.0;
    profiling_head_config.motor_step_angle_size = motor_step_angle_size;

    profiling_head_config.ad_threshold = 50;
    profiling_head_config.ad_threshold_ch1 = 50;
    profiling_head_config.ad_threshold_ch2 = 50;
    profiling_head_config.filt_gain = (config.select_channel == 1) ? 1 : 20;
    profiling_head_config.filt_gain_ch1 = 1;
    profiling_head_config.filt_gain_ch2 = 20;

    profiling_head_config.adaptive_gain_control_max = 107;
    profiling_head_config.adaptive_gain_control_set_point = 100;
    profiling_head_config.agc_max_ch1 = 0x6b;
    profiling_head_config.agc_max_ch2 = 0x6b;
    profiling_head_config.agc_set_point_ch1 = 0x64;
    profiling_head_config.agc_set_point_ch2 = 0x64;

    profiling_head_config.slope_ch1 = slope_ch1;
    profiling_head_config.slope_ch2 = slope_ch2;
    profiling_head_config.advanced_slope_ch1 = 110;
    profiling_head_config.advanced_slope_ch2 = 150;
    profiling_head_config.advanced_slope_delay_ch1 = 0;
    profiling_head_config.advanced_slope_delay_ch2 = 0;
    
    HeadCommand* head_config = reinterpret_cast<HeadCommand*>(&profiling_head_config);
    writeHeadCommand(*head_config, timeout);
}

// Change acquisition parameters without resetting the head (fast parameter
// change)
//
// This is purely coming from reverse engineering
//
// The 'magic' fields are the fields for which I had no idea what to do
struct ProfilingAcquisitionConfigMessage
{
    // Always 30 for this message
    uint8_t V3B_params;
    // A/D thresholds for channels 1 and 2
    uint8_t ad_threshold[2];
    // "filt_gain" (no idea what the hell it is). I leave it to the default
    // values, as even the windows application does not touch it
    uint8_t filt_gain[2];
    // The actual gain. Note that this is THE value that - it seems - cannot be
    // changed in the head profiling configuration structure
    uint8_t gain[2];
    // Wild guess. Left at default values of 189
    uint8_t agc_setpoint[2];
    // Wild guess. Left at default values of 110 and 150
    uint16_t slope[2];
    // Wild guess. Left at default values of 0
    uint16_t advanced_slope_delay[2];
} __attribute__ ((packed));

void Profiling::configureAcquisition(ProfilingAcquisitionConfig const& config, uint32_t timeout)
{
    if (has_pending_data)
        throw std::runtime_error("requestData() called and the corresponding receiveData() has not been called");

    ProfilingAcquisitionConfigMessage msg;
    msg.V3B_params = 30;
    msg.ad_threshold[0] = msg.ad_threshold[1] = config.ad_threshold * 255;
    msg.filt_gain[0] = 1;
    msg.filt_gain[1] = 20;
    msg.gain[0] = config.gain * 255;
    msg.gain[1] = config.gain * 255;
    msg.agc_setpoint[0] = msg.agc_setpoint[1] = 189;
    msg.slope[0] = 110;
    msg.slope[1] = 150;
    msg.advanced_slope_delay[0] = 0;
    msg.advanced_slope_delay[1] = 0;

    std::vector<uint8_t> packet =
        SeaNetPacket::createPaket(device_type,
                mtHeadCommand,
                (uint8_t*)&msg,
                sizeof(msg));

    LOG_DEBUG_S <<"Sent mtHeadCommand packet" ;
    writePacket(&packet[0],packet.size(),timeout);
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
    base::Time bak = scan.time;
    scan.time = timestamp_estimator->update(base::Time::now());//base::Time::now();
    scan.start_angle = base::Angle::fromRad(-((double)data.right_limit/6399.0*2.0*M_PI)+M_PI).getRad();
    scan.angular_resolution = ((double)data.motor_step_angle_size)/6399.0*2.0*M_PI;
    scan.minRange = 300; // minimum range of the profiling sonar 0.3m
    if((data.range & 0xC000) == 0)
        scan.maxRange = (data.range & 0x3FFF) * 100;
    else
        scan.maxRange = 100000; //Hardcoded 100meter
    scan.speed = (data.data_count * scan.angular_resolution) / (scan.time-bak).toSeconds();// data.scan_time.toSeconds();
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
