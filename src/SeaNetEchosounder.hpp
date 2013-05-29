#ifndef _SEANETECHOSOUNDER_H_
#define _SEANETECHOSOUNDER_H_

#include <iodrivers_base/Driver.hpp>
#include <base/samples/rigid_body_state.h>

namespace sea_net {

class Echosounder : protected ::iodrivers_base::Driver
{
public:
	Echosounder();
	~Echosounder();

        /** Opens a serial port and sets it up to a sane configuration.
         *
         * Throws UnixError on error */
        void openSerial(std::string const& port, int baudrate=115200);

        /** Reads one packat from the input buffer and returns its type.
         *  Use getAuxData, getVersion ... depending on the returned type
         *  to get the content of the package */
        bool getDistance(base::samples::RigidBodyState &state, int timeout);


protected:
        virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const;
        uint8_t buffer[50];

}; };
#endif

