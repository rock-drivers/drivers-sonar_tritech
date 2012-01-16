#include "SeaNet.hpp"
#include "SeaNetTypesInternal.hpp"
#include <iodrivers_base/Timeout.hpp>

namespace sea_net{

SeaNet::SeaNet(DeviceType type): 
  iodrivers_base::Driver(MAX_PACKET_SIZE,false),
  device_type(type)
{
    sea_net_packet = new SeaNetPacket; //forward declaration 
}

SeaNet::~SeaNet() 
{
    delete sea_net_packet;
}

void SeaNet::openSerial(std::string const& port, int baudrate,int timeout)
{
    ::iodrivers_base::Driver::openSerial(port,baudrate);
    //reboot to be sure the device is in a well defined state
    reboot(timeout);
}

void SeaNet::start()
{
    clear();
    writeSendData(WRITE_TIMEOUT);
    //TODO check if configure was called before
}

void SeaNet::reboot(int timeout)
{
    iodrivers_base::Timeout time_out(timeout);
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(device_type,mtReBoot);
    writePacket(&packet[0],packet.size(),timeout);

    //now wait for an alive message
    while(mtAlive != readPacket(time_out.timeLeft()));
}

void SeaNet::getVersion(VersionData &version,int timeout)
{
    iodrivers_base::Timeout time_out(timeout);

    std::vector<uint8_t> send_packet = SeaNetPacket::createPaket(device_type,mtSendVersion);
    writePacket(&send_packet[0],send_packet.size(),timeout);

    //wait until we received a mtVersionData package
    while(mtVersionData != readPacket(time_out.timeLeft()));

    //extract version data
    const uint8_t *packet;
    size_t size;
    sea_net_packet->getRawData(packet,size);

    version.softwareVersion = packet[13];
    version.infoBits = packet[14];
    version.serialNr = packet[15] | (packet[16] <<8);
    version.programmLength = packet[17] | (packet[18] <<8) | (packet[19] <<16) | (packet[20] <<24);
    version.programmChecksum = packet[21] | (packet[22] << 8);
    version.nodeID = packet[23];
}

SeaNetPacket* SeaNet::getSeaNetPacket()
{
    return sea_net_packet;
}

void SeaNet::getRawData(const uint8_t* &buffer, size_t &size)
{
    sea_net_packet->getRawData(buffer,size);
}

void SeaNet::getAuxData(std::vector<uint8_t> &aux_data)
{
    const uint8_t *buffer;
    size_t size;
    sea_net_packet->getAuxData(buffer,size);
    aux_data.resize(size);
    memcpy(&aux_data[0],buffer,size);
}

sea_net::PacketType SeaNet::readPacket(int timeout)
{
    iodrivers_base::Timeout time_out(timeout);
    while(!time_out.elapsed())
    {
        uint8_t* buffer = sea_net_packet->getPacketPtr();
        size_t size = iodrivers_base::Driver::readPacket(buffer,MAX_PACKET_SIZE,time_out.timeLeft());
        sea_net_packet->setSize(size);
        if(!sea_net_packet->validate())
            throw std::runtime_error("extractPacket has extracted an invalid package!");  //this should never happen
        
        //trigger head again to get the next beam
        PacketType type = sea_net_packet->getPacketType();
        if(type == mtHeadData)
            writeSendData(time_out.timeLeft());
        return type;
    }
    return mtNull;
}

void SeaNet::writeSendData(int timeout) 
{
    std::vector<uint8_t> packet = SeaNetPacket::createSendDataPaket(device_type);
    writePacket(&packet[0],packet.size(),timeout);
}

int SeaNet::extractPacket(uint8_t const* buffer, size_t buffer_size) const
{
    return SeaNetPacket::isValidPacket(buffer,buffer_size);
}
};

