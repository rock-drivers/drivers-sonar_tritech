#include "SeaNet.hpp"
#include "SeaNetTypesInternal.hpp"
#include <iodrivers_base/Timeout.hpp>
#include <iostream>
#include <base/logging.h>

namespace sea_net{

SeaNet::SeaNet(DeviceType type): 
  iodrivers_base::Driver(SEA_NET_MAX_PACKET_SIZE,false),
  device_type(type),bstart(false)
{
}

SeaNet::~SeaNet() 
{
}

void SeaNet::openSerial(std::string const& port, int baudrate)
{
    LOG_DEBUG_S <<"opening serial port: "<<  port << " with baudrate: " << baudrate ;
    ::iodrivers_base::Driver::openSerial(port,baudrate);
}

void SeaNet::start()
{
    clear();
    bstart = true;
    writeSendData(WRITE_TIMEOUT);
    //TODO check if configure was called before
}

void SeaNet::stop()
{
    bstart = false;
}

void SeaNet::reboot(int timeout)
{
    LOG_DEBUG_S << "rebooting device" ;
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,mtReBoot);
    writePacket(&packet[0],packet.size());
    clear();

    //now wait for an alive message
    waitForPacket(mtAlive,timeout);
}

void SeaNet::getVersion(VersionData &version,int timeout)
{
    std::vector<uint8_t> send_packet = SeaNetPacket::createPaket(device_type,mtSendVersion);
    writePacket(&send_packet[0],send_packet.size());

    //wait until we received a mtVersionData package
    waitForPacket(mtVersionData,timeout);
    sea_net_packet.decodeVersionData(version);
}

void SeaNet::waitForPacket(PacketType type,int timeout)
{
    LOG_DEBUG_S << "wait for packet type: "<<type << " timeout " << timeout ;
    iodrivers_base::Timeout time_out(timeout);
    while(type != readPacket(time_out.timeLeft()));
}

bool SeaNet::isFullDublex(int timeout)
{
    return isFullDuplex(timeout);
}

bool SeaNet::isFullDuplex(int timeout)
{
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,mtSendBBUser);
    writePacket(&packet[0],packet.size());
    waitForPacket(mtBBUserData,timeout);
    BBUserData settings;
    sea_net_packet.decodeBBUserData(settings);
    return settings.full_duplex;
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
                if(bstart)
                    writeSendData(time_out.timeLeft());
                break;
            case mtAlive:
                break;
            default:
                break;
        }
        
        LOG_DEBUG_S << "read packet of type: "<< type ;
        return type;
    }
    return mtNull;
}

void SeaNet::writeHeadCommand(HeadCommand &head_config, int timeout)
{
    AliveData alive_data;
    bool received_alive = false;
    bool was_started = bstart;

    iodrivers_base::Timeout time_out(timeout);
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,
                                                           mtHeadCommand,
                                                           (uint8_t*)&head_config,
                                                            sizeof(head_config));
    stop();
    //we have to wait until the device is no longer sending HeadData
    try
    {
        while(true) waitForPacket(mtHeadData,300); 
    }
    catch(std::runtime_error e)
    {}

    LOG_DEBUG_S <<"Sent mtHeadCommand packet" ;
    writePacket(&packet[0],packet.size());

    //wait for an alive packet to check if the sonar is configured
    while(!alive_data.ready && alive_data.no_config && !alive_data.config_send)
    {
        try
        {
            waitForPacket(mtAlive,time_out.timeLeft());
        }
        catch(std::runtime_error e)
        {
            if(received_alive)
                throw std::runtime_error("Configure failed: Configuration was not accepted by the device.");
            else
                throw std::runtime_error("Configure failed: Device is not responding. Timeout too small?");

        }
        received_alive = true;
        sea_net_packet.decodeAliveData(alive_data);
    }
    if(was_started)
    {
        start();
        PacketType packet_type = mtNull;
        while(packet_type != sea_net::mtHeadData)
        {
            packet_type = readPacket(time_out.timeLeft());
            switch(packet_type)
            {
            case sea_net::mtAlive:
                //send start packet again 
                //this is necessary because 
                //the sonar is not accepting mtSendData
                //after configuration for some milliseconds 
                writeSendData(time_out.timeLeft());             
                break;
            default:
                break;
            }
        }
    }
}

void SeaNet::writeSendData(int timeout) 
{
    LOG_DEBUG_S << "write mtSendData packet" ;
    std::vector<uint8_t> packet = SeaNetPacket::createSendDataPaket(device_type);
    writePacket(&packet[0],packet.size());
}

int SeaNet::extractPacket(uint8_t const* buffer, size_t buffer_size) const
{
    LOG_DEBUG_S << "trying to extract package buffer size " << buffer_size ;
    return SeaNetPacket::isValidPacket(buffer,buffer_size);
}

void SeaNet::setWriteTimeout(uint32_t timeout)
{
    Driver::setWriteTimeout(base::Time::fromMicroseconds(timeout*1000));
}


}
