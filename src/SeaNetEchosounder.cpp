#include "SeaNetEchosounder.hpp"
#include <iodrivers_base/Timeout.hpp>
#include <iostream>
#include <base/logging.h>

namespace sea_net{

Echosounder::Echosounder(): 
  iodrivers_base::Driver(50,true)
{
}

Echosounder::~Echosounder() 
{
}

void Echosounder::openSerial(std::string const& port, int baudrate)
{
    LOG_DEBUG_S <<"opening serial port: "<<  port << " with baudrate: " << baudrate ;
    ::iodrivers_base::Driver::openSerial(port,baudrate);
}

bool Echosounder::getDistance(base::samples::RigidBodyState &state, int timeout)
{
    iodrivers_base::Timeout time_out(timeout);
    while(!time_out.elapsed())
    {
        size_t size = iodrivers_base::Driver::readPacket(buffer,50,time_out.timeLeft());
        if(size>0){
        int depth = 0;
        if(sscanf((const char*)&buffer[0],"%dmm",&depth) == 1)
        {
            state.position[2] = 0.001*depth; //We want Si units not mm
            state.time = base::Time::now();
            state.cov_position(2,2) = 0.2;
            LOG_DEBUG_S <<"decoding EchoSounder: got depth:" << state.position[2];
            return true;
        }
        else
            throw std::runtime_error("Cannot decode EchoSounder. Depth was not find in the package");
        }
    }
    return false;
}


int Echosounder::extractPacket(uint8_t const* buff, size_t buffer_size) const
{
    LOG_DEBUG_S << "trying to extract package buffer size " << buffer_size ;
    for(unsigned int i=0;i<buffer_size;i++){
        if(buff[i] == '\n'){
            LOG_DEBUG_S << "Found packet of size " << i << std::endl;
            return i+1;
        }
    }
    return 0;
}

}
