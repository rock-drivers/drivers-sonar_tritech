#ifndef _SEANET_TYPES_H_
#define _SEANET_TYPES_H_

#include <base/angle.h>
#include <stdint.h>

namespace sea_net
{
    struct VersionData
    {
        uint8_t softwareVersion;
        uint8_t infoBits;
        uint8_t serialNr;
        uint8_t nodeID;
        uint32_t programmLength;
        uint16_t programmChecksum;
    };

    enum DeviceType
    {
        IMAGINGSONAR = 2,
        SIDESCAN = 10,
        SUBBOTTOM = 15,
        PROFILINGSONAR = 20,
        BATHYMETER = 40,
    };

    enum PacketType{
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

    };

    struct AliveData
    {
       bool ready;
       bool motor_on;
       bool scanning;
       bool no_config;
       bool config_send;
       AliveData():
           ready(false),motor_on(false),scanning(false),
           no_config(true),config_send(false){};
    };

    struct HeadData
    {
        uint8_t node_type; 
        uint8_t type;
        uint16_t packed_size;
        uint8_t device_type;
        uint8_t head_status;
        uint8_t sweep_code;
        uint16_t head_control;
        uint16_t range;
        uint32_t txn;
        uint8_t gain;
        uint16_t slope;
        uint8_t ad_spawn;
        uint8_t ad_low;
        uint16_t heading_offset;
        uint16_t ad_interval;
        uint16_t left_limit;
        uint16_t right_limit;
        uint8_t steps;
        uint16_t bearing;
        uint16_t data_bytes;
        const uint8_t *scan_data;
    };

    struct BBUserData
    {
        bool full_dublex;
        BBUserData():
            full_dublex(0){};
    };

    struct MicronConfig
    {
        base::Angle left_limit;
        base::Angle right_limit;
        base::Angle angular_resolution; 

        double max_distance;
        double min_distance;
        double resolution;
        double speed_of_sound;
        double gain;

        bool low_resolution;
        bool invert;
        bool continous;

        MicronConfig():
            left_limit(base::Angle::fromRad(M_PI)),
            right_limit(base::Angle::fromRad(M_PI)),
            angular_resolution(base::Angle::fromRad(5.0/180.0*M_PI)),
            max_distance(15.0),
            min_distance(1.0),
            resolution(0.1),
            speed_of_sound(1500.0),
            gain(0.4),
            low_resolution(false),
            invert(false),
            continous(true)
        {};

        bool operator==(const MicronConfig &other) const
        {
            return (other.left_limit == left_limit &&
                    other.right_limit == right_limit &&
                    other.angular_resolution == angular_resolution &&
                    other.max_distance == max_distance && 
                    other.min_distance == min_distance &&
                    other.resolution == resolution &&
                    other.speed_of_sound== speed_of_sound &&
                    other.gain == gain &&
                    other.low_resolution == low_resolution &&
                    other.invert == invert &&
                    other.continous == continous);
        };
        bool operator!=(const MicronConfig &other) const
        {
            return !(other == *this);
        };
    };
}
#endif 
