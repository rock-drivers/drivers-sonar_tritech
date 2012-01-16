#include "SeaNetTypes.h"
#include <stdio.h>

using namespace sea_net;

static int SeaNetPacket::isValidPacket(char *buffer, size_t size)
{
    //this should never happen!
    if(size > MAX_PACKET_SIZE)
        throw std::runtime_error("Packet size is bigger than the buffer!!!");

    //Packet is too small to be valid
    if(buffer_size<14) 
        return 0;

    //Packets starts with a @, so search for it and
    //discard all data before
    size_t readPos=0;
    while (buffer[readPos] != PACKET_START && readPos < buffer_size) {
        readPos++;
    }
    if(readPos>0){
        //TODO log this because this means we are loosing data
        return -readPos;
    }

    //decode package len which is stored twice in two different formats
    //this len is the size of the package from byte 6 onwards
    size_t hexlen,len;
    sscanf(buffer+1,"%h4x",&hexlen);
    len = (buffer[5] | (buffer[6]<<8 ));

    //check if both lengths are equal (simple check)
    if(len != hexlen)
    {
        //TODO log this because this means we are loosing data
        return -1;              //do not use this package
    }

    //the first 5 bytes and the PACKED_END are not included in the len therefore 
    //add them to get the total size of the message
    len += 6;
    if(len > buffer_size)
        return 0;               //package is to small wait for more data

    //checking for the end of the package 
    if (buffer[len-1] == PACKED_END) 
    {
        PacketType type = (PackageType) buffer[10];
        //check package size
        switch(type)
        {
        case mtHeadCommand:
            if(len == 82)
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
            if(size > 45)
            {
                //read number of data bytes
                uint16_t total = package[15]|(package[14]<<8);
                uint16_t data_bytes = package[44]|(package[43]<<8);
                if(len == dataBytes+45 && len = total+14 )
                    return len;
            }
            break;
        case mtAuxData:
            if(size > 16)
            {
                uint16_t payload_size = packet[13] | (message[14]<<8);
                if(len == payload_size+16)
                    return len;
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
        default:
            return len;
        }
    }
    //TODO log this because this means we are loosing data
    return -1; 
}

static std::vector<uint8_t> createPaket(DeviceType device_type,
                                        PackageType packet_type, 
                                        uint8_t* payload,
                                        size_t payload_size)
{
    std::vector<uint8_t> packet;

    //calculate packet size
    size_t size = payload_size + 14;    //14 Bytes is the size of a packet without user payload
    paket.resize(size);
    size_t size2 = size - 6;            //first 5 Bytes and the last one are not included 
                                        //in the packet size encoded into the package
                                        //Header
    paket[0] = PACKET_START;
    snprintf(&packet[0]+1,4,"%04X",size2);

    paket[5] = (size2) & 255;
    paket[6] = ((size2)>>8) & 255;
    packet[7] = 255;                        //this is always 255 for a PC
    packet[8] = (uint8_t)device_type;       //receiver type
    packet[9] = payload_size+3;              
    msg[10] = packet_type;                  //this is part of the packet payload but
                                            //each packet must have Byte 10-12 therefore
                                            //we consider it as part of the header
    msg[11] = 0x80;                         //Sequence end
    msg[12] = (uint8_t)device_type;         //receiver type

    //user payload, packet payload without Byte 10,11,12
    if(payload_size && payload)
        memcpy(&packet[0]+13,payload,payload_size);

    packet[size-1] = PACKED_END;

    //check if packet is valid
    if(size != isValidPacket(&packet[0],size))
        throw std::runtime_error("Invalid package was created");
    return packet
}


static std::vector<uint8_t> createSendDataPaket(DeviceType device_type)
{
    //TODO fill with real time
    uint8_t time[5];
    memset(&time,0,4);
    return createPaket(device_type,mtSendData,&time,4);
}

uint8_t* SeaNetPacket::getPacketPtr()
{
    return &packet[0]
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
    return (PackageType) packet[10];
}

DeviceType SeaNetPacket::getDeviceType()const
{
    if(!isValid())
        throw std::runtime_error("Package is not valid!");
    return (DeviceType) packet[10]; /
}

bool SeaNetPacket::isValid() const
{
    //if validate was already called do not check it again
    if(valid)
        return true;
    if(SeaNetPacket::isValidPacket(packet[0].size) != size)
        return false;
    return true;
}

void SeaNetPacket::getAuxData(uint8_t *const &buffer,size_t &size)const;
{
    if(getPacketType != mtAuxData)
        throw std::runtime_error("SeaNet: Wrong packet is stored in the buffer!");

    size = packet[13] | (message[14]<<8);
    if(this->size != 16+size)
        throw std::runtime_error("SeaNet: Packet is too small!");
    buffer = &aux_data+15;
}

void SeaNetPacket::getRawData(uint8_t *const &buffer,size_t &size)const;
{
    buffer = &packet[0];
    size = this->size;
}
