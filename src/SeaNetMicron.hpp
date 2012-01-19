#ifndef _SEANETMICRON_H_
#define _SEANETMICRON_H_ 

#include "SeaNet.hpp"
#include "SeaNetTypesInternal.hpp"

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
            void decodeSonarBeam(base::samples::SonarBeam &beam);

        private:
            HeadCommand head_config;
            double speed_of_sound;
    };
};

#endif

