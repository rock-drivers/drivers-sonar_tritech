#ifndef _MICRON_H_
#define _MICRON_H_ 

#include "SeaNet.hpp" 
#include <base/samples/sonar_beam.h>

namespace sea_net
{ 
    struct HeadConfigPacket;

    class Micron : public SeaNet
    {
        public:
            Micron();
            ~Micron();
            void configure(const MicronConfig &config,uint32_t timeout);
            void getSonarBeam(base::samples::SonarBeam &beam);

        private:
            HeadConfigPacket * head_config;
    };
};

#endif

