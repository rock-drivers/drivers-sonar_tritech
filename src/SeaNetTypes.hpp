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

    /** Acquisition mode for the profiling sonar
     *
     * This governs what information the sonar returns
     */
    enum PROFILING_MODE
    {
        /** Return the first echo that is above the ad_threshold parameter
         *
         * This is the fastest mode (as the head can continue after the first
         * echo), but is obviously dependent on choosing both a good gain and
         * threshold
         */
        PROFILING_FIRST,
        PROFILING_NORMAL,
        /** Return the highest echo */
        PROFILING_MAX
    };

    /** Complete configuration of the SeaKing
     *
     * Note that ProfilingAcquisitionConfig allows to set a subset of the
     * parameters in a much faster way
     */
    struct ProfilingConfig
    {
        PROFILING_MODE mode;

        base::Angle left_limit;
        base::Angle right_limit;
        base::Angle angular_resolution; 

        /** Which channel to use */
        unsigned int select_channel;
        unsigned int frequency_chan1;
        unsigned int frequency_chan2;
        
        /** Maximum distance. A higher distance usually means longer acquisition
         * times as the sonar has to wait longer for an echo before going on
         */
        double max_distance;
        /** Minimum distance under which echoes will be ignored (in m) */
        double min_distance;
        /** The speed of sound (by default 1500 m/s) used to convert
         * configuration parameters in meters to device parameters that are
         * usually in seconds
         */
        double speed_of_sound;
        
        /** Whether scanning should go always in the same direction or
         * "ping-pong". When scanning a small segment, a non-continuous mode is
         * recommended
         */
        bool continous;
        
        ProfilingConfig() : 
            mode(PROFILING_FIRST),
            left_limit(base::Angle::fromRad(M_PI)),
            right_limit(base::Angle::fromRad(-M_PI)),
            angular_resolution(base::Angle::fromRad(5.0/180.0*M_PI)),
            select_channel(2),
            frequency_chan1(600000),
            frequency_chan2(1200000),
            max_distance(15.0),
            min_distance(1.0),
            speed_of_sound(1500.0),
            continous(false)
        {};
      
        bool operator==(const ProfilingConfig &other) const
        {
            return (other.mode == mode &&
                    other.left_limit == left_limit &&
                    other.right_limit == right_limit &&
                    other.angular_resolution == angular_resolution &&
                    other.select_channel == select_channel &&
                    other.frequency_chan1 == frequency_chan1 &&
                    other.frequency_chan2 == frequency_chan2 &&
                    other.max_distance == max_distance && 
                    other.min_distance == min_distance && 
                    other.speed_of_sound== speed_of_sound &&
                    other.continous == continous);
        };
        
        bool operator!=(const ProfilingConfig &other) const
        {
            return !(other == *this);
        };
    };

    /** Smaller configuration structure for acquisition parameters that can be
     * changed live on the Tritech seaking.
     *
     * Changing these parameters is almost instantaneous, unlike using the
     * ProfilingConfig which involves resetting the whole device
     */
    struct ProfilingAcquisitionConfig
    {
        /** The acquisition gain in [0, 1]. Documentation recommends to start
         * with 40% (0.4)
         */
        double gain;
        /** Signal under this threshold will be ignored by the sonar. The
         * documentation recommends to stick with 0.2
         *
         * The value is in percent, i.e. [0, 1]
         */
        double ad_threshold;

        ProfilingAcquisitionConfig()
            : gain(0.4)
            , ad_threshold(0.2) {}
    };

    //some values are not working for the micron dst like lock_out time
    //invert, etc
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
        bool continous;

        MicronConfig():
            left_limit(base::Angle::fromRad(M_PI)),
            right_limit(base::Angle::fromRad(-M_PI)),
            angular_resolution(base::Angle::fromRad(5.0/180.0*M_PI)),
            max_distance(15.0),
            min_distance(1.0),
            resolution(0.1),
            speed_of_sound(1500.0),
            gain(0.4),
            low_resolution(false),
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
                    other.continous == continous);
        };
        bool operator!=(const MicronConfig &other) const
        {
            return !(other == *this);
        };
    };
}
#endif 
