#include "SeaNetTypesInternal.hpp"
#include <stdio.h>
#include <iostream>
#include <base-logging/Logging.hpp>

using namespace sea_net;

int SeaNetPacket::isValidPacket(uint8_t const *buffer, size_t buffer_size)
{
    //this should never happen!
    if(buffer_size > SEA_NET_MAX_PACKET_SIZE)
        throw std::runtime_error("Packet size is bigger than the buffer!!!");


    //Packet is too small to be valid
    if(buffer_size<14) 
        return 0;

    //Packets starts with a @, so search for it and
    //discard all data before
    size_t readPos=0;
    while (readPos != buffer_size && buffer[readPos] != PACKET_START) {
        ++readPos;
    }
    if(readPos>0){
        LOG_WARN_S << "Corrupted packet: Skipping " << readPos << " bytes because no start was found.";
        return -readPos;
    }

    //decode packet len which is stored twice in two different formats
    //this len is the size of the packet from byte 6 onwards
    size_t len;
    size_t hexlen = 0;
    sscanf((const char*)&buffer[1],"%4lX",&hexlen);
    len = (buffer[5] | (buffer[6]<<8 ));

    //check if both lengths are equal (simple check)
    if(len != hexlen)
    {
        LOG_WARN_S << "Corrupted packet: Binary packet size differs from hexadecimal: bin:"
                   << len << " hex: "<< hexlen ;
        return -1;      
    }

    //the first 5 bytes and the PACKED_END are not included in the len therefore 
    //add them to get the total size of the message
    len += 6;
    if(len > buffer_size)
        return 0;               //packet is to small wait for more data

    //DeviceType device_type = (DeviceType) buffer[8];
    PacketType type = (PacketType) buffer[10];
    //checking for the end of the packet 
    if (buffer[len-1] == PACKET_END) 
    {
        LOG_DEBUG_S << "Found packet of type "<< type << " and size "<< len ;

        //check packet size
        switch(type)
        {
        case mtHeadCommand:
            return len;
            break;
        case mtSendData:
            if(len == 18)
                return len;
            break;
        case mtSendVersion:
            if(len == 14)
                return len;
            break;
        case mtHeadData:
            if(len > 45)
            {
                //read number of data bytes
                uint16_t total = buffer[13]|(((uint16_t)buffer[14])<<8);
                if((DeviceType)buffer[7] == PROFILINGSONAR)
                {
                    uint16_t data_count = buffer[38]|(((uint16_t)buffer[39])<<8);
                    uint16_t data_bytes = data_count * 2;
                    if((uint16_t)len == data_bytes+51 && (uint16_t)len == total+14 )
                        return len;
                }
                else
                {
                    uint16_t data_bytes = buffer[42]|(((uint16_t)buffer[43])<<8);
                    if((uint16_t)len == data_bytes+45 && (uint16_t)len == total+14 )
                        return len;
                }
                LOG_WARN_S << "Corrupted mtHeadData: Size miss match" ;
            }
            break;
        case mtAuxData:
            if(len > 16)
            {
                uint16_t payload_size = buffer[13] | (buffer[14]<<8);
                if((uint16_t)len == payload_size+16)
                    return len;
                LOG_WARN_S << "Corrupted mtAuxData: Size miss match" ;
            }
            break;
        case mtVersionData:
            if(len == 25)
                return len;
            break;
        case mtReBoot:
            if(len == 14)
                return len;
            break;
        case mtAlive:
            if(len == 22)
                return len;
            break;
        case mtSendBBUser:
            if(len == 14)
                return len;
            break;
        case mtBBUserData:
            if(len == 175 || len == 264)  //dst sonars have a bigger packet 
                return len;
            break;
        default:
            return len;
        }
        LOG_WARN_S << "Corrupted packet: Wrong packet size for packet type:"<< type <<" . size:" 
                   << len ;
        return -1;
    }
    LOG_WARN_S << "Corrupted packet (possibly type " << type << "): no message end was found, packet size " << len;
    return -1; 
}

std::vector<uint8_t> SeaNetPacket::createPaket(DeviceType device_type,
        PacketType packet_type, 
        uint8_t* payload,
        size_t payload_size)
{
    LOG_DEBUG_S << "Create new packet of type "<< packet_type << " with payload_size "<< payload_size ;
    
    std::vector<uint8_t> packet;

    //calculate packet size
    size_t size = payload_size + 14;    //14 Bytes is the size of a packet without user payload
    packet.resize(size,0);
    size_t size2 = size - 6;            //first 5 Bytes and the last one are not included 
    //in the packet size encoded into the package
    //Header
    packet[0] = PACKET_START;
    sprintf((char*)&packet[1],"%04lX",size2);

    packet[5] = (size2) & 255;
    packet[6] = ((size2)>>8) & 255;
    packet[7] = 255;                        //this is always 255 for a PC
    packet[8] = (uint8_t)device_type;       //receiver type
    packet[9] = payload_size+3;              
    packet[10] = (uint8_t)packet_type;                  //this is part of the packet payload but
    //each packet must have Byte 10-12 therefore
    //we consider it as part of the header
    packet[11] = 0x80;                         //Sequence end
    packet[12] = (uint8_t)device_type;         //receiver type

    //user payload, packet payload without Byte 10,11,12
    if(payload_size && payload)
        memcpy(&packet[13],payload,payload_size);

    packet[size-1] = PACKET_END;

    //check if packet is valid
    if(size != (unsigned int)isValidPacket(&packet[0],packet.size()))
        throw std::runtime_error("Invalid package was created");
    return packet;
}


std::vector<uint8_t> SeaNetPacket::createSendDataPaket(DeviceType device_type)
{
    //TODO fill with real time
    uint8_t time[4];
    memset(&time,0,4);
    return createPaket(device_type,mtSendData,&time[0],4);
}

uint8_t* SeaNetPacket::getPacketPtr()
{
    return &packet[0];
}

void SeaNetPacket::setSize(size_t size)
{
    this->size = size;
}

void SeaNetPacket::invalidate()
{
    valid = false;
}

bool SeaNetPacket::validate()
{
    valid = isValid();
    return valid;
}

PacketType SeaNetPacket::getPacketType()const
{
    if(!isValid())
        throw std::runtime_error("Package is not valid!");
    return (PacketType) packet[10];
}

DeviceType SeaNetPacket::getReceiverType()const
{
    if(!isValid())
        throw std::runtime_error("Package is not valid!");
    return (DeviceType) packet[8];
}

DeviceType SeaNetPacket::getSenderType()const
{
    if(!isValid())
        throw std::runtime_error("Package is not valid!");
    return (DeviceType) packet[7];
}

bool SeaNetPacket::isValid() const
{
    //if validate was already called do not check it again
    if(valid)
        return true;
    if((unsigned int)SeaNetPacket::isValidPacket(&packet[0],size) != size)
        return false;
    return true;
}

void SeaNetPacket::getRawData(const uint8_t * &buffer,size_t &size)const
{
    buffer = &packet[0];
    size = this->size;
}

void SeaNetPacket::decodeAliveData(AliveData &data) const
{
    if(getPacketType() != mtAlive)
        throw std::runtime_error("Cannot decode AliveData. Wrong packet type is buffered.");

    //extract alive data data
    data.ready = packet[20]&2;
    data.motor_on = packet[20]&8;
    data.scanning = packet[20]&32;
    data.no_config = packet[20]&64;
    data.config_send = packet[20]&128;
}

void SeaNetPacket::decodeHeadData(ImagingHeadData &data) const
{
    if(getPacketType() != mtHeadData)
        throw std::runtime_error("Cannot decode mtHeadData. Wrong packet type is buffered.");
    
    if(getSenderType() != IMAGINGSONAR)
        throw std::runtime_error("Cannot decode mtHeadData. Sender isn't an imaging sonar.");

    //extract alive data data
    data.node_type = packet[12];
    data.type = packet[10];
    data.packed_size    = packet[13] | (packet[14]<<8);
    data.device_type     = packet[15];
    data.head_status     = packet[16];
    data.sweep_code      = packet[17];
    data.head_control   = packet[18] | (packet[19]<<8);
    data.range         = packet[20] | (packet[21]<<8);
    data.txn           = packet[22] | (packet[23]<<8) | (packet[24]<<16) | (packet[25]<<24);
    data.gain           = packet[26];
    data.slope         = packet[27] | (packet[28]<<8);
    data.ad_span        = packet[29];
    data.ad_low          = packet[30];
    data.heading_offset = packet[31] | (packet[32]<<8);
    data.ad_interval    = packet[33] | (packet[34]<<8);
    data.left_limit     = packet[35] | (packet[36]<<8);
    data.right_limit    = packet[37] | (packet[38]<<8);
    data.motor_step_angle_size          = packet[39];
    data.bearing       = packet[40] | (packet[41]<<8);
    data.data_bytes     = packet[42] | (packet[43]<<8);
    data.scan_data      = &packet[44];
}

void SeaNetPacket::decodeHeadData(ProfilingHeadData& data) const
{
    if(getPacketType() != mtHeadData)
        throw std::runtime_error("Cannot decode mtHeadData. Wrong packet type is buffered.");
    
    if(getSenderType() != PROFILINGSONAR)
        throw std::runtime_error("Cannot decode mtHeadData. Sender isn't a profiling sonar.");
    
    //extract alive data
    data.node_type      = packet[12];
    data.type           = packet[10];
    data.packed_size    = packet[13] | (packet[14]<<8);
    data.device_type    = packet[15];
    data.head_status    = packet[16];
    data.sweep_code     = packet[17];
    data.head_control   = packet[18] | (packet[19]<<8);
    data.range          = packet[20] | (packet[21]<<8);
    data.txn            = packet[22] | (packet[23]<<8) | (packet[24]<<16) | (packet[25]<<24);
    data.gain           = packet[26];
    data.slope          = packet[27] | (packet[28]<<8);
    data.ad_threshold   = packet[29];
    data.filt_gain      = packet[30];
    data.left_limit     = packet[31] | (packet[32]<<8);
    data.right_limit    = packet[33] | (packet[34]<<8);
    data.motor_step_angle_size = packet[35];
    data.scan_time      = packet[36] | (packet[37]<<8);
    data.data_count     = packet[38] | (packet[39]<<8);
    data.scan_data      = &packet[40];
}

void SeaNetPacket::decodeAuxData(std::vector<uint8_t> &aux_data) const
{
    if(getPacketType() != mtAuxData)
        throw std::runtime_error("SeaNet: Wrong packet is stored in the buffer!");

    size_t aux_size = packet[13] | (packet[14]<<8);
    if(this->size != 16+aux_size)
        throw std::runtime_error("SeaNet: Packet is too small!");

    aux_data.resize(aux_size);
    memcpy(&aux_data[0],&packet[15],aux_size);
}

void SeaNetPacket::decodeVersionData(VersionData &version) const
{
    if(getPacketType() != mtVersionData)
        throw std::runtime_error("SeaNet: Wrong packet is stored in the buffer!");

    version.softwareVersion = packet[13];
    version.infoBits = packet[14];
    version.serialNr = packet[15] | (packet[16] <<8);
    version.programmLength = packet[17] | (packet[18] <<8) | (packet[19] <<16) | (packet[20] <<24);
    version.programmChecksum = packet[21] | (packet[22] << 8);
    version.nodeID = packet[23];
}

void SeaNetPacket::decodeBBUserData(BBUserData &data) const
{
    if(getPacketType() != mtBBUserData)
        throw std::runtime_error("SeaNet: Wrong packet is stored in the buffer!");
    data.full_duplex = !packet[146];
}
