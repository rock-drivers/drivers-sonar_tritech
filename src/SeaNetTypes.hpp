#ifndef _SEANET_TYPES_H_
#define _SEANET_TYPES_H_

#include <base/angle.h>

namespace sea_net
{
    static const char PACKET_START = '@';
    static const char PACKET_END = 0x0A;
    static const int WRITE_TIMEOUT = 500;  //im ms
    static const int MAX_PACKET_SIZE = 1500+45; // mtHeadData is the biggest package 

    struct VersionData
    {
        uint8_t softwareVersion;
        uint8_t infoBits;
        uint8_t serialNr;
        uint8_t nodeID;
        uint32_t programmLength;
        uint16_t programmChecksum;
    };

    //type identifier returned by the device 
    //as rxNode 
    enum DeviceType{
        IMAGINGSONAR 2,
        SIDESCAN 10,
        SUBBOTTOM 15,
        PROFILINGSONAR 20,
        BATHYMETER 40,
    };

    class SeaNetPacket
    {
        public:
            static int isValidPacket(char *buffer,size_t size);
            static std::vector<uint8_t> createPaket(DeviceType device_type,
                                                    PackageType packet_type, 
                                                    uint8_t* payload = NULL,
                                                    size_t payload_size = 0);

            static std::vector<uint8_t> createSendDataPaket(DeviceType device_type);

        public:
            void setSize(uint8_t size);
            void invalidate();
            bool validate();
            uint8_t *getPacketPtr();

            bool isValid() const;
            PacketType getPacketType() const;
            DeviceType getDeviceType() const;

            void getRawData(uint8_t *const &buffer,size_t &size)const;
            void getAuxData(uint8_t *const &buffer,size_t &size)const;

        private:
        //we do not use a std::vector because this would introduce
        //dynamic memory allocation if different packet types are stored
            uint8_t packet[MAX_PACKET_SIZE];
            size_t size;
            bool valid;
    };

    enum PackageType{
        mtNull = 0,
        mtVersionData, mtHeadData, mtSpectData, mtAlive, 
        mtPrgAck, mtBBUserData, mtTestData, mtAuxData, mtAdcData,
        mtLdcReq, na11, na12, mtLanStatus, mtSetTime, mtTimeout,
        mtReBoot, mtPerformanceData, na18, mtHeadCommand, mtEraseSector,
        mtProgBlock, mtCopyBootBlk, mtSendVersion, mtSendBBUser, mtSendData,
        mtSendPreferenceData
    };

    struct ProfilinConfig
    {

    }

    struct MicronConfig
    {
        bool adc8on;
        bool cont;              //remove
        bool scanright;   
        bool invert;            //remove
        bool chan2;             //remove
        bool applyoffset;       //remove
        bool pingpong;

        uint16_t rangeScale;    //remove
        base::Angle leftLimit;
        base::Angle rightLimit;
        uint8_t adSpan;         //remove
        uint8_t adLow;          //remove
        double initialGain;     //remove
        uint8_t motorStepDelayTime;     //remove
        base::Angle motorStepAngleSize; //remove
        double maximumDistance;         
        double resolution;              
        uint16_t adcSetpointCh;         //remove

        //base::Angle left_limit;
        //base::Angle right_limit;
        //double max_distance; 
        //double min_distance;_
        //double resolution; 
        //double sound_velocity
        //bool low_resolution;
        //bool scan_right;   
        //bool ping_pong;

        MicronConfig():
            adc8on(true),
            cont(true),
            scanright(false),
            invert(false),
            chan2(true),
            applyoffset(false),
            pingpong(false),
            rangeScale(30),
            leftLimit(base::Angle::fromRad(M_PI)),
            rightLimit(base::Angle::fromRad(M_PI)),
            adSpan(81),
            adLow(8),
            initialGain(0.2),
            motorStepDelayTime(25),
            motorStepAngleSize(base::Angle::fromRad(5.0/180.0*M_PI)),
            maximumDistance(15.0),
            resolution(0.1),
            adcSetpointCh(0)
        {};

        bool operator!=(const SonarConfig &other) const{
            bool b = 
                other.stamp==stamp &&
                other.adc8on==adc8on &&
                other.cont==cont &&
                other.scanright==scanright &&
                other.invert==invert &&
                other.chan2==chan2 &&
                other.applyoffset==applyoffset &&
                other.pingpong==pingpong &&
                other.rangeScale==rangeScale &&
                other.leftLimit==leftLimit &&
                other.rightLimit==rightLimit &&
                other.adSpan==adSpan &&
                other.adLow==adLow &&
                other.initialGain==initialGain &&
                other.motorStepDelayTime==motorStepDelayTime &&
                other.motorStepAngleSize==motorStepAngleSize &&
                other.maximumDistance==maximumDistance&&
                other.resolution==resolution&&
                other.adcSetpointCh==adcSetpointCh;
            return !b;
        };
    };
}
#endif 
