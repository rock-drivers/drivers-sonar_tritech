/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#ifndef _MICRON_H_
#define _MICRON_H_ 

#include "SonarScan.h"
#include "SeaNet.h" 
#include <list>
#include <iodrivers_base.hh>
#include <base/time.h>

namespace SeaNet{


enum HeadControl{
	ADC8ON = 1,
	CONT = 2,
	SCANRIGHT = 4,
	INVERT = 8,
	MOTOFF = 16,
	TXOFF = 32,
	SPARE = 64,
	CHAN2 = 128,
	RAW = 256,
	HASMOT = 512,
	APPLYOFFSET = 1024,
	PINGPONG = 2048,
	STARELLIM = 4096,
	REPLYASL = 8192,
	REPLYTHR = 16384,
	IGNORESENSOR = 32768
};

struct headControl{
    uint8_t V3BParams;
    uint16_t headControl;
    uint8_t headType;
    uint32_t txnCh1;
    uint32_t txnCh2;
    uint32_t rxnCh1;
    uint32_t rxnCh2;
    uint16_t pulseLength;
    uint16_t rangeScale;
    uint16_t leftLimit;
    uint16_t rightLimit;
    uint8_t adSpan;
    uint8_t adLow;
    uint8_t initialGainCh1;
    uint8_t initialGainCh2;
    uint16_t slopeCh1;
    uint16_t slopeCh2;
    uint8_t motorStepDelayTime;
    uint8_t motorStepAngleSize;
    uint16_t adInterval;
    uint16_t numberOfBins;
    uint16_t maxADBuff;
    uint16_t lockoutTime;
    uint16_t minorAxisDir;
    uint8_t majorAxisPan;
    uint8_t crtl2;
    uint16_t scanZ;
    uint8_t adSpanCh1;
    uint8_t adSpanCh2;
    uint8_t adLowCh1;
    uint8_t adLowCh2;
    uint8_t igainCh1;
    uint8_t igainCh2;
    uint8_t adcSetPointCh1;
    uint8_t adcSetPointCh2;
    uint16_t slopeCH1;
    uint16_t slopeCH2;
    uint16_t slopeDelayCh1;
    uint16_t slopeDelayCh2;
} __attribute__ ((packed)) __attribute__((__may_alias__));

/*
class MicronHandler: SonarHandler{
	public:
	virtual void processSonarScan(MicronScan const&)=0;
};
*/

class Micron : public Protocol 
{

public:
	Micron();
	~Micron();
	void sendHeadData(bool adc8on=true,bool cont=true,bool scanright=false,bool invert=false,bool chan2=false,bool applyoffset=false, bool pingpong=false,uint16_t rangeScale=30, uint16_t leftLimit=1, uint16_t rightLimit=6399, uint8_t adSpan=false, uint8_t adLow=8, uint8_t initialGain=84, uint8_t motorStepDelayTime=25, uint8_t motorStepAngleSize=16,uint16_t adInterval=20, uint16_t numberOfBins=10, uint16_t adcSetpoint=0);
	void processHeadData(u8 *message);
	void sendHeadData(headControl hc);
	headControl getDefaultHeadData(bool adc8on,bool cont,bool scanright,bool invert,bool chan2,bool applyoffset=false, bool pingpong=false,uint16_t rangeScale=30, uint16_t leftLimit=1, uint16_t rightLimit=6399, uint8_t adSpan=false, uint8_t adLow=8, uint8_t initialGain=84, uint8_t motorStepDelayTime=25, uint8_t motorStepAngleSize=16,uint16_t adInterval=20, uint16_t numberOfBins=10, uint16_t adcSetpoint=0);


	base::Time lastPackage;

private:
//	void notifyPeers(MicronScan const& scan);
};

};

#endif

