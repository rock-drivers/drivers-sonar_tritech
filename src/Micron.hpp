#ifndef _MICRON_H_
#define _MICRON_H_ 
#include "SeaNet.h" 

namespace sea_net
{ 
    struct HeadConfigMicron;

    class Micron : public SeaNet
    {
        public:
            Micorn();
            ~Micron();
            void configure(sensorConfig::SonarConfig &config,uint32_t timeout);
            void getSonarBeam(base::samples::SonarBeam &beam);

        private:
            HeadConfigMicron *head_config;
    };
};

#endif

