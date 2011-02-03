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
#include <iodrivers_base.hh>
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
#ifdef __orogen
};
#else
}  __attribute__ ((packed)) __attribute__((__may_alias__)); 
#endif


#ifndef __orogen
class Driver: public Protocol 
{
public:
	Driver();
	~Driver();
	static headControl getDefaultHeadData();
	void sendHeadData(headControl hc);
	void processHeadData(u8 *message);

};
#endif

};};

#endif
