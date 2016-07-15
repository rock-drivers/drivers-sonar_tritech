#ifndef _SEANET_PROFILING_HPP_
#define _SEANET_PROFILING_HPP_ 

#include "SeaNet.hpp"
#include <base/samples/LaserScan.hpp>

namespace aggregator
{
    class TimestampEstimator;
};


namespace sea_net
{
    class Profiling : public SeaNet
    {
    public:
        Profiling();
        ~Profiling();
        void configure(const ProfilingConfig &config, uint32_t timeout);
        void configureAcquisition(const ProfilingAcquisitionConfig &config, uint32_t timeout);
        void decodeScan(base::samples::LaserScan &scan);

    private:
        aggregator::TimestampEstimator* timestamp_estimator;
        double speed_of_sound;
    };
};

#endif
