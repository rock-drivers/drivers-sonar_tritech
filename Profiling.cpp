/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include "Profiling.h"
#include <stdio.h>
#include <string.h>
#include <boost/thread/thread.hpp>

#define WRITETIMEOUT 200


namespace SeaNet{ namespace Profiling{

Driver::Driver(): Protocol(0x14,0xFF) //0x14 means Driver Sonar
{
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
	if(nodeType== 0x14){
		printf("got length or multitype: %i\n",message[9]);
		uint8_t headStatus     	= message[16];
		uint8_t sweep		= message[17];
		uint16_t headControl   	= message[18] | (message[19]<<8);
		uint16_t range		= message[20] | (message[21]<<8);
		uint32_t txn           	= message[22] | (message[23]<<8) | (message[24]<<16) | (message[25]<<24);
		uint8_t gain           	= message[26];
		uint16_t slope          = message[27] | (message[28]<<8);
		uint8_t adThrs         	= message[29];
		uint8_t filterGain     	= message[30];
		uint16_t leftLim        = message[31] | (message[32]<<8);
		uint16_t rightLim       = message[33] | (message[34]<<8);
			printf("Left Limit: %i, RightLimit: %i Headcontrol: %02x %02x\n",leftLim,rightLim,message[18],message[19]);
		uint8_t  stepSize      	= message[35];
		uint16_t scanTime       = message[36] | (message[37]<<8);
		uint16_t noPings        = message[38] | (message[39]<<8);
		uint8_t *scanData	= message+40;
		uint16_t dataEnd = 40+(noPings*2);
		if(message[dataEnd+10] != 0x0A){	
			printf("Bla Kaputt: %c\n",message[0]);
			for(int i=dataEnd-2;i<dataEnd+20;i++){
				printf(" %02x",message[i]);
			}
			printf("\n");
			printf("No of Pings: %i, len %i\n",noPings,packedSize);
			fprintf(stderr,"Cannot Parse Head message\n");
		}else{
			uint16_t data[noPings];
			for(int i=0;i<noPings;i++){
				data[i] = scanData[(i*2)] | (scanData[(i*2)+1]<<8);
			}
			notifyPeers(ProfilerScan(range,txn,gain,leftLim,rightLim,stepSize,scanTime,noPings,data));
		}	
	}else{
		fprintf(stderr,"Unknown Device\n");
	}
}

headControl Driver::getDefaultHeadData(){
	headControl hc;
	hc.V3BParams = 0x1D;
	hc.headCtl = 8964 +128; //HF chan2
	hc.headType = 5;
	hc.txnch1 = 77846282;
	hc.txnch2 = 162403450;
	hc.rxnch1 = 138915348;
	hc.rxnch2 = 223472517;
	hc.txPulseLength=30;
	hc.rangeScale = 20;
	//hc.leftLimit = 2134;
	//hc.rightLimit = 4266;
	hc.leftLimit = 0;
	hc.rightLimit = 6399;
	hc.adThreashold = 50;
	hc.filterGain = 1;	
	hc.maxAge = 107;	
	hc.setPoint = 100;	
	hc.slopeCh1 = 110;
	hc.slopeCh2 = 150;
	hc.motorTime = 25;	
	hc.stepSize = 16;
	hc.scanTime = 30;
	hc.PrfSpl = 0;
	hc.PrfCtl2 = 0;
	hc.lockout= 500;
	hc.minorAxisDir = 1600;
	hc.majorAxisPan = 1;
	hc.ctl2 = 0;
	hc.scanZ = 0;
	hc.adThrCh1 = 0x32;
	hc.adThrCh2 = 0x32;
	hc.filterGainCh1 = 1;
	hc.filterGainCh2 = 1;
	hc.maxAgeCh1= 0x6b;
	hc.maxAgeCh2= 0x6b;
	hc.setPointCh1= 0x64;
	hc.setPointCh2= 0x64;
	hc.slope_Ch1 = 110;
	hc.slope_Ch2 = 150;
	hc.slopeDelayCh1 = 0;
	hc.slopeDelayCh2 = 0;
	return hc;

}
   
void Driver::sendHeadData(headControl hc){
	memcpy(headData,&hc,sizeof(hc));	
	headDataChanged=true;	
}

};};

