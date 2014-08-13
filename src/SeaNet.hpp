#ifndef _SEANET_H_
#define _SEANET_H_ 

#include <iodrivers_base/Driver.hpp>
#include "SeaNetTypes.hpp"
#include "SeaNetTypesInternal.hpp"

namespace sea_net {
class SeaNetPacket;
class HeadConfigPacket;

class SeaNet : protected ::iodrivers_base::Driver
{
public:
        /** Base class for RS232 SeaNet devices
         *  The class has to be initialized with 
         *  the right device type which is used
         *  as communictaion check
         */ 
	SeaNet(const DeviceType type);
	~SeaNet();

        /** Opens a serial port and sets it up to a sane configuration.
         *
         * Throws UnixError on error */
        void openSerial(std::string const& port, int baudrate=115200);
        /** Clears the input buffer and triggers the device to send 
         *  mtHeadData 
         *
         * Throws UnixError on error */
        void start();
        void stop();

        /** Reboots the Device and waits for a mtAlive package 
         *  be careful this takes a while and even if you receive mtAlives
         *  the device my be in a state where it does not accept mtHeadCommands */
        void reboot(int timeout);

        /** Reads one packat from the input buffer and returns its type.
         *  Use getAuxData, getVersion ... depending on the returned type
         *  to get the content of the package */
        PacketType readPacket(int timeout);

        void waitForPacket(PacketType type, int timeout);

        /** extracts the software version of the device from a mtVersionData package 
         *
         *  call this function if readPacket returns mtVersionData */  
        void getVersion(VersionData &version, int timeout);

        /** Returns if the remote device is configured as full duplex or half
         * duplex
         */
        bool isFullDuplex(int timeout);

        /** @deprecated use isFullDuplex instead */
        bool isFullDublex(int timeout);
        
        void setWriteTimeout(uint32_t timeout);

protected:
        void writeSendData(int timeout);
        void writeHeadCommand(HeadCommand &hc, int timeout);
        virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const;
        SeaNetPacket* getSeaNetPacket();

protected:
        SeaNetPacket sea_net_packet;
        DeviceType device_type;
        bool bstart;
}; };
#endif

