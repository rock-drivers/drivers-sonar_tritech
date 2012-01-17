#include "SeaNet.hpp"
#include "SeaNetTypesInternal.hpp"
#include <iodrivers_base/Timeout.hpp>
#include <iostream>
#include <base/logging.h>

namespace sea_net{

SeaNet::SeaNet(DeviceType type): 
  iodrivers_base::Driver(SEA_NET_MAX_PACKET_SIZE,false),
  device_type(type)
{
}

SeaNet::~SeaNet() 
{
}

void SeaNet::openSerial(std::string const& port, int baudrate)
{
    LOG_DEBUG_S <<"opening serial port: "<<  port << " with baudrate: " << baudrate << std::endl;
    ::iodrivers_base::Driver::openSerial(port,baudrate);
}

void SeaNet::start()
{
    clear();
    writeSendData(WRITE_TIMEOUT);
    //TODO check if configure was called before
}

void SeaNet::reboot(int timeout)
{
    LOG_DEBUG_S << "rebooting device" << std::endl;
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,mtReBoot);
    writePacket(&packet[0],packet.size(),timeout);
    clear();

    //now wait for an alive message
    waitForPackage(mtAlive,timeout);
}

void SeaNet::getVersion(VersionData &version,int timeout)
{
    std::vector<uint8_t> send_packet = SeaNetPacket::createPaket(device_type,mtSendVersion);
    writePacket(&send_packet[0],send_packet.size(),timeout);

    //wait until we received a mtVersionData package
    waitForPackage(mtVersionData,timeout);
    sea_net_packet.decodeVersionData(version);
}

void SeaNet::waitForPackage(PacketType type,int timeout)
{
    LOG_DEBUG_S << "wait for packet type: "<<type << " timeout " << timeout << std::endl;
    iodrivers_base::Timeout time_out(timeout);
    while(type != readPacket(time_out.timeLeft()));
}

bool SeaNet::isFullDublex(int timeout)
{
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,mtSendBBUser);
    writePacket(&packet[0],packet.size(),timeout);
    waitForPackage(mtBBUserData,timeout);
    BBUserData settings;
    sea_net_packet.decodeBBUserData(settings);
    return settings.full_dublex;
}

SeaNetPacket* SeaNet::getSeaNetPacket()
{
    return &sea_net_packet;
}

sea_net::PacketType SeaNet::readPacket(int timeout)
{
    iodrivers_base::Timeout time_out(timeout);
    while(!time_out.elapsed())
    {
        uint8_t* buffer = sea_net_packet.getPacketPtr();
        size_t size = iodrivers_base::Driver::readPacket(buffer,SEA_NET_MAX_PACKET_SIZE,time_out.timeLeft());
        sea_net_packet.setSize(size);
        if(!sea_net_packet.validate())
            throw std::runtime_error("extractPacket has extracted an invalid package!");  //this should never happen
        
        //trigger head again to get the next beam
        PacketType type = sea_net_packet.getPacketType();
        switch(type)
        {
            case mtHeadData:
                writeSendData(time_out.timeLeft());
                break;
            case mtAlive:
                break;
            default:
                break;
        }
        
        LOG_DEBUG_S << "read packet of type: "<< type << std::endl;
        return type;
    }
    return mtNull;
}

void SeaNet::writeSendData(int timeout) 
{
    LOG_DEBUG_S << "write mtSendData packet" << std::endl;
    std::vector<uint8_t> packet = SeaNetPacket::createSendDataPaket(device_type);
    writePacket(&packet[0],packet.size(),timeout);
}

int SeaNet::extractPacket(uint8_t const* buffer, size_t buffer_size) const
{
    LOG_DEBUG_S << "trying to extract package buffer size " << buffer_size << std::endl;
    return SeaNetPacket::isValidPacket(buffer,buffer_size);
}
};

