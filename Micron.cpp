/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include "Micron.h"
#include <stdio.h>
#include <string.h>
#include <boost/thread/thread.hpp>

#define WRITETIMEOUT 200

namespace SeaNet{ namespace Micron{

Driver::Driver(bool usePTS): Protocol(0x02,0xFF,usePTS){ //Reciving node 0x02 means micron
    headDataChanged=false;
}

Driver::~Driver() {
}


void Driver::processHeadData(u8 *message){
	base::Time timestamp = base::Time::now();
	uint8_t nodeType = message[12];
	//uint8_t type = message[10];
	uint16_t packedSize    = message[13] | (message[14]<<8);
	uint8_t deviceType     = message[15];
	if(nodeType==rxNode){ 
		uint8_t headStatus     = message[16];
		uint8_t sweepCode      = message[17];
		uint16_t headControl   = message[18] | (message[19]<<8);
		uint16_t range         = message[20] | (message[21]<<8);
		uint32_t txn           = message[22] | (message[23]<<8) | (message[24]<<16) | (message[25]<<24);
		uint8_t gain           = message[26];
		uint16_t slope         = message[27] | (message[28]<<8);
		uint8_t adSpawn        = message[29];
		uint8_t adLow          = message[30];
		uint16_t headingOffset = message[31] | (message[32]<<8);
		uint16_t adInterval    = message[33] | (message[34]<<8);
		uint16_t leftLimit     = message[35] | (message[36]<<8);
		uint16_t rightLimit    = message[37] | (message[38]<<8);
		uint8_t steps          = message[39];
		uint16_t bearing       = message[40] | (message[41]<<8);
		uint16_t dataBytes     = message[42] | (message[43]<<8);
		uint8_t *scanData      = message+44;
		MicronScan scan(packedSize,deviceType,headStatus,sweepCode,headControl,
			range,txn,gain,slope,adSpawn,adLow,headingOffset,adInterval,
			leftLimit,rightLimit,steps,bearing,dataBytes,scanData);
		scan.time = timestamp;
		notifyPeers(scan);
	}else{
		fprintf(stderr,"Unknown Device\n");
	}
}



void Driver::sendHeadData(headControl hc){
	//TODO
	memcpy(headData,&hc,sizeof(hc));
	headDataChanged=true;
}

headControl Driver::getDefaultHeadData(bool adc8on,bool cont,bool scanright,bool invert,bool chan2,bool applyoffset,
		bool pingpong,uint16_t rangeScale, uint16_t leftLimit, uint16_t rightLimit, uint8_t adSpan, 
		uint8_t adLow, uint8_t initialGain, uint8_t motorStepDelayTime, uint8_t motorStepAngleSize,
		uint16_t adInterval, uint16_t numberOfBins, uint16_t adcSetpoint){


    headControl hc; 
    hc.V3BParams = 0x1D;
    bool motorOff = false;
    bool txOff= false;
    bool spare = false;
    //hc.chan2 = true;
    bool raw = true;
    bool hasmot = true;
    //hc.applyoffset = false;
    //hc.pingpong = false;
    bool starellim = false;
    bool replyasl = true;
    bool replyThr = false;
    bool ignoreSensor = false;
    hc.headType = 0x02;
    hc.txnCh1 = 0; //Ignored
    hc.txnCh2 = 0; //Ignored
    hc.rxnCh1 = 0; //Ignored
    hc.rxnCh2 = 0; //Ignored
    hc.pulseLength = (0x28) | 0; //Ignored
    hc.rangeScale  = rangeScale;
    //hc.rangeScale  = (0x3C) | 0; //6 Meter
    hc.leftLimit   = leftLimit; //1
    //hc.leftLimit   = (0x01) | 0; //1
    hc.rightLimit  = rightLimit;
    //hc.rightLimit  = (0xFF) | (0x18<<8);
    hc.adSpan = adSpan;
    //hc.adSpan = 0x51;
    //hc.adLow = 0x8;
    hc.adLow = adLow;
    //hc.initialGainCh1 = 0x90;//54;
    //hc.initialGainCh2 = 0x90;//54;
    hc.initialGainCh1 = initialGain;
    hc.initialGainCh2 = initialGain;
    hc.slopeCh1 = 0; //Ignored
    hc.slopeCh2 = 0; //Ignored
    hc.motorStepDelayTime = motorStepDelayTime;
    //hc.motorStepDelayTime = 0x19;
    hc.motorStepAngleSize = motorStepAngleSize;
    //hc.motorStepAngleSize = 0x10;
    hc.adInterval = adInterval;
    //hc.adInterval = (0x8D) | (0<<8);
    //hc.numberOfBins = (0xFF) | (0<<8);
    hc.numberOfBins = numberOfBins;
    hc.maxADBuff = (0xE8) | (3<<8);
    hc.lockoutTime = (0x97) | (0x03<<8);
    hc.minorAxisDir = (0x40) | (0x06<<8);
    hc.majorAxisPan = (0x01);
    hc.crtl2 = 0;
    hc.scanZ = 0;
    
    //hc.adSpanCh1 = 0x50;
    //hc.adSpanCh2 = 0x51;
    //hc.adLowCh1 = 0x09;
    //hc.adLowCh2 = 0x08;
    //hc.initialGainCh1_2 = 0x90;//0x54;
    //hc.initialGainCh2_2 = 0x90;//0x54;
    //hc.adcSetpointCh1 = 0x00;
    //hc.adcSetpointCh2 = 0x00;
    
    hc.adSpanCh1 = adSpan;
    hc.adSpanCh2 = adSpan;
    hc.adLowCh1 = adLow;
    hc.adLowCh2 = adLow;
    hc.igainCh1 = initialGain;
    hc.igainCh2 = initialGain;
    hc.adcSetPointCh1 = adcSetpoint;
    hc.adcSetPointCh2 = adcSetpoint;
    hc.slopeCH1 =0; //Ignored
    hc.slopeCH2 =0; //Ignored
    hc.slopeDelayCh1 =0; //Ignored
    hc.slopeDelayCh2 =0; //Ignored

    hc.headControl =
        (adc8on?ADC8ON:0) |
        (cont?CONT:0) |
        (scanright?SCANRIGHT:0) |
        (invert?INVERT:0) |
        (motorOff?MOTOFF:0) |
        (txOff?TXOFF:0) |
        (spare?SPARE:0) |
        (chan2?CHAN2:0) |
        (raw?RAW:0) |
        (hasmot?HASMOT:0) |
        (applyoffset?APPLYOFFSET:0) |
        (pingpong?PINGPONG:0) |
        (starellim?STARELLIM:0) |
        (replyasl?REPLYASL:0) |
        (replyThr?REPLYTHR:0) |
        (ignoreSensor?IGNORESENSOR:0)
        ;

	return hc;
}


void Driver::sendHeadData(bool adc8on,bool cont,bool scanright,bool invert,bool chan2,bool applyoffset,
		bool pingpong,uint16_t rangeScale, uint16_t leftLimit, uint16_t rightLimit, uint8_t adSpan, 
		uint8_t adLow, uint8_t initialGain, uint8_t motorStepDelayTime, uint8_t motorStepAngleSize,
		uint16_t adInterval, uint16_t numberOfBins, uint16_t adcSetpoint) {

	headControl hc = getDefaultHeadData(adc8on,cont,scanright,invert,chan2,applyoffset,pingpong,rangeScale,leftLimit,rightLimit,adSpan,adLow,initialGain,motorStepDelayTime,motorStepDelayTime,adInterval,numberOfBins,adcSetpoint); 
	sendHeadData(hc);	
}

};};

