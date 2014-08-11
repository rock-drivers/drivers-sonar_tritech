#ifndef _SEANET_H_
#define _SEANET_H_ 

#include <iodrivers_base/Driver.hpp>
#include "SeaNetTypes.hpp"
#include "SeaNetTypesInternal.hpp"

namespace sea_net {
class SeaNetPacket;
class HeadConfigPacket;

class SeaNet : public ::iodrivers_base::Driver
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

        /** Overloaded from iodrivers_base::Driver */
        void openURI(std::string const& uri);

        /** Reboots the Device and waits for a mtAlive package 
         *  be careful this takes a while and even if you receive mtAlives
         *  the device my be in a state where it does not accept mtHeadCommands */
        void reboot(int timeout);

        /** Reads one packat from I/O and returns the packet type
         *
         * The packet's content can be retrieved by the SeaNetPacket API on the
         * value returned by getSeaNetPacket
         *
         * @param timeout the timeout in milliseconds
         * @return the packet type
         */
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

        /** Returns the structure holding the last received packet */
        SeaNetPacket const& getSeaNetPacket() const;

        /** Request the sonar to acquire one beam
         *
         * Calling this method will fail until the corresponding data packet has
         * been received.
         *
         * The timeout is the default write timeout
         */
        void requestData();

        /** Wait for the beam data requested by requestData to be received
         *
         * @param timeout the timeout in milliseconds
         */
        void receiveData(int timeout);

protected:
        void writeHeadCommand(HeadCommand &hc, int timeout);
        virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const;

protected:
        SeaNetPacket sea_net_packet;
        DeviceType device_type;
        bool has_pending_data;
}; };
#endif

