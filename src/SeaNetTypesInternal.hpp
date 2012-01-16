#ifndef _SEANET_TYPES_INTERNAL_H_
#define _SEANET_TYPES_INTERNAL_H_

namespace sea_net
{
    namespace micron
    {
        struct HeadConfig
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

        enum HeadConfigFlags
        {
            ADC8ON = 1, CONT = 2, SCANRIGHT = 4, INVERT = 8,
            MOTOFF = 16, TXOFF = 32, SPARE = 64, CHAN2 = 128,
            RAW = 256, HASMOT = 512, APPLYOFFSET = 1024, PINGPONG = 2048,
            STARELLIM = 4096, REPLYASL = 8192, REPLYTHR = 16384,
            IGNORESENSOR = 32768
        };
    }

    namespace profiling
    {
        enum HeadConfigFlags{
            ADAPTIVE_GAIN = 1,
            PRF_ALT = 2,
            SCANRIGHT = 4,
            INVERT = 8,
            MOTOFF = 16,
            TXOFF = 32,
            PRF_T10 = 64,
            CHAN2 = 128,
            PRF_FIRST = 256,
            HASMOT = 512,
            PRF_PINGSYNC = 1024,
            PRF_SCANSYNC = 2048,
            STARELLIM = 4096,
            PRF_MASTER = 8192,
            PRF_MIRROR = 16384,
            IGNORESENSOR = 32768
        };

        struct HeadConfig{
            uint8_t V3BParams; uint16_t headCtl; uint8_t headType;
            uint32_t txnch1; uint32_t txnch2; uint32_t rxnch1;
            uint32_t rxnch2; uint16_t txPulseLength; uint16_t rangeScale;
            uint16_t leftLimit; uint16_t rightLimit; uint8_t adThreashold;
            uint8_t filterGain;	uint8_t maxAge;	uint8_t setPoint;	
            uint16_t slopeCh1; uint16_t slopeCh2; uint8_t motorTime;	
            uint8_t stepSize; uint16_t scanTime; uint16_t PrfSpl;
            uint16_t PrfCtl2; uint16_t lockout; uint16_t minorAxisDir;
            uint8_t majorAxisPan; uint8_t ctl2; uint16_t scanZ;
            uint8_t adThrCh1; uint8_t adThrCh2; uint8_t filterGainCh1;
            uint8_t filterGainCh2; uint8_t maxAgeCh1; uint8_t maxAgeCh2;
            uint8_t setPointCh1; uint8_t setPointCh2; uint16_t slope_Ch1;
            uint16_t slope_Ch2; uint16_t slopeDelayCh1; uint16_t slopeDelayCh2;

            bool operator!=(const headControl &other) const{
                return !(
                        other.V3BParams		== V3BParams&&
                        other.headCtl		== headCtl&&
                        other.headType		== headType&&
                        other.txnch1		== txnch1&&
                        other.txnch2		== txnch2&&
                        other.rxnch1		== rxnch1&&
                        other.rxnch2		== rxnch2&&
                        other.txPulseLength	== txPulseLength&&
                        other.rangeScale	== rangeScale&&
                        other.leftLimit		== leftLimit&&
                        other.rightLimit	== rightLimit&&
                        other.adThreashold	== adThreashold&&
                        other.filterGain	== filterGain&&		
                        other.maxAge		== maxAge &&	
                        other.setPoint		== setPoint&&		
                        other.slopeCh1		== slopeCh1&&
                        other.slopeCh2		== slopeCh2&&
                        other.motorTime		== motorTime&&		
                        other.stepSize		== stepSize&&
                        other.scanTime		== scanTime&&
                        other.PrfSpl		== PrfSpl&&
                        other.PrfCtl2		== PrfCtl2&&
                        other.lockout		== lockout&&
                        other.minorAxisDir	== minorAxisDir&&
                        other.majorAxisPan	== majorAxisPan&&
                        other.ctl2		== ctl2&&
                        other.scanZ		== scanZ&&
                        other.adThrCh1		== adThrCh1&&
                        other.adThrCh2		== adThrCh2&&
                        other.filterGainCh1	== filterGainCh1&&
                        other.filterGainCh2	== filterGainCh2&&
                        other.maxAgeCh1		== maxAgeCh1&&
                        other.maxAgeCh2		== maxAgeCh2&&
                        other.setPointCh1	== setPointCh1&&
                        other.setPointCh2	== setPointCh2&&
                        other.slope_Ch1		== slope_Ch1&&
                        other.slope_Ch2		== slope_Ch2&&
                        other.slopeDelayCh1	== slopeDelayCh1&&
                        other.slopeDelayCh2	== slopeDelayCh2
                        );
            }
        }  __attribute__ ((packed)) __attribute__((__may_alias__)); 
    }

    /**
     * Length definition of packages
     * 0 menas (most time) not implemented yet
     */
    static int MsgLength[] = {
        3, //mtNULL = 3,
        3, //mtVersionData=3,
        71, //mtHeadData=0,
        0, //mtSpectData=0,
        0, //mtAlive=0,
        0, //mtPrgAck=0,
        0, //mtBBUserData=0,
        0, //mtTestData=0,
        0, //mtAuxData=0,
        0, //mtAdcData=0,
        0, //mtLdcReq=0,
        0, //unknown
        0, //unknown
        0, //mtLanStatus=0,
        0, //mtSetTime=0,
        0, //mtTimeout=0,
        0, //mtReBoot=0,
        0, //mtPerformanceData=0,
        0, //unknown
        71, //mtHeadCommand=0,
        0, //mtEraseSector=0,
        0, //mtProgBlock=0,
        0, //mtCopyBootBlk=0,
        3, //mtSendVersion=0,
        0, //mtSendBBUser=0,
        7, //mtSendData=0,
        0 //mtSendPreferenceData=0
    };


}
#endif 
