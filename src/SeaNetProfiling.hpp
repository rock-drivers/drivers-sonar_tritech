#ifndef _SEANET_PROFILING_HPP_
#define _SEANET_PROFILING_HPP_ 

#include "SeaNet.hpp"
#include <base/samples/laser_scan.h>

namespace aggregator
{
    class TimestampEstimator;
};


namespace sea_net
{
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
    } __attribute__ ((packed)) __attribute__((__may_alias__));
    
    class Profiling : public SeaNet
    {
    public:
        Profiling();
        ~Profiling();
        void configure(const ProfilingConfig &config, uint32_t timeout);
        void decodeScan(base::samples::LaserScan &scan);
    private:
        aggregator::TimestampEstimator* timestamp_estimator;
        ProfilingHeadCommand profiling_head_config;
        double speed_of_sound;
    };
};

#endif
