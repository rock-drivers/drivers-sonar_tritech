#ifndef _SEANET_TYPES_INTERNAL_H_
#define _SEANET_TYPES_INTERNAL_H_

#include "SeaNetTypes.hpp"

namespace sea_net
{
    static const char PACKET_START = '@';
    static const char PACKET_END = 0x0A;
    static const int WRITE_TIMEOUT = 500;  //im ms
    static const uint32_t MAX_PACKET_SIZE = 1500+45; // mtHeadData is the biggest package 

    struct HeadConfigPacket
    {
        uint8_t V3BParams; uint16_t headControl; uint8_t headType;
        uint32_t txnCh1; uint32_t txnCh2; uint32_t rxnCh1; uint32_t rxnCh2;
        uint16_t pulseLength; uint16_t rangeScale; uint16_t leftLimit;
        uint16_t rightLimit; uint8_t adSpan; uint8_t adLow; uint8_t initialGainCh1;
        uint8_t initialGainCh2; uint16_t slopeCh1; uint16_t slopeCh2; uint8_t motorStepDelayTime;
        uint8_t motorStepAngleSize; uint16_t adInterval; uint16_t numberOfBins;
        uint16_t maxADBuff; uint16_t lockoutTime; uint16_t minorAxisDir;
        uint8_t majorAxisPan; uint8_t crtl2; uint16_t scanZ; uint8_t adSpanCh1;
        uint8_t adSpanCh2; uint8_t adLowCh1; uint8_t adLowCh2; uint8_t igainCh1;
        uint8_t igainCh2; uint8_t adcSetPointCh1; uint8_t adcSetPointCh2;
        uint16_t slopeCH1; uint16_t slopeCH2; uint16_t slopeDelayCh1;
        uint16_t slopeDelayCh2;
    } __attribute__ ((packed)) __attribute__((__may_alias__));


    class SeaNetPacket
    {
        public:
            static int isValidPacket(uint8_t const *buffer,size_t size);
            static std::vector<uint8_t> createPaket(DeviceType device_type,
                                                    PacketType packet_type, 
                                                    uint8_t* payload = NULL,
                                                    size_t payload_size = 0);

            static std::vector<uint8_t> createSendDataPaket(DeviceType device_type);

        public:
            void setSize(size_t size);
            void invalidate();
            bool validate();
            uint8_t *getPacketPtr();

            bool isValid() const;
            PacketType getPacketType() const;
            DeviceType getDeviceType() const;

            void getRawData(const uint8_t * &buffer,size_t &size)const;
            void getAuxData(const uint8_t * &buffer,size_t &size)const;

        private:
        //we do not use a std::vector because this would introduce
        //dynamic memory allocation if different packet types are stored
            uint8_t packet[MAX_PACKET_SIZE];
            size_t size;
            bool valid;
    };

}
#endif 
