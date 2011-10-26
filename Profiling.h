/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#ifndef _PROFILING_H_
#define _PROFILING_H_ 

#ifndef __orogen
#include "SonarScan.h"
#include "SeaNet.h"
#include <list>
#include <base/time.h>
#endif

typedef uint16_t u16;
typedef uint8_t  u8;
typedef uint32_t u32;

namespace SeaNet{ namespace Profiling{

#ifndef __orogen
enum HeadControl{
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
#endif

struct headControl{
	u8 V3BParams;
	u16 headCtl;
	u8 headType;
	u32 txnch1;
	u32 txnch2;
	u32 rxnch1;
	u32 rxnch2;
	u16 txPulseLength;
	u16 rangeScale;
	u16 leftLimit;
	u16 rightLimit;
	u8 adThreashold;
	u8 filterGain;	
	u8 maxAge;	
	u8 setPoint;	
	u16 slopeCh1;
	u16 slopeCh2;
	u8 motorTime;	
	u8 stepSize;
	u16 scanTime;
	u16 PrfSpl;
	u16 PrfCtl2;
	u16 lockout;
	u16 minorAxisDir;
	u8 majorAxisPan;
	u8 ctl2;
	u16 scanZ;
	u8 adThrCh1;
	u8 adThrCh2;
	u8 filterGainCh1;
	u8 filterGainCh2;
	u8 maxAgeCh1;
	u8 maxAgeCh2;
	u8 setPointCh1;
	u8 setPointCh2;
	u16 slope_Ch1;
	u16 slope_Ch2;
	u16 slopeDelayCh1;
	u16 slopeDelayCh2;
#ifndef __orogen
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
#endif
}  __attribute__ ((packed)) __attribute__((__may_alias__)); 


#ifndef __orogen
class Driver: public Protocol 
{
public:
	Driver(bool usePTS=false);
	~Driver();
	static headControl getDefaultHeadData();
	void sendHeadData(headControl hc);
	void processHeadData(u8 *message);
};
#endif

};};

#endif
