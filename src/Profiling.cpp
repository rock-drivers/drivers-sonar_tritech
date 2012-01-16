#include "Profiling.hpp"
#include "SeaNetTypesInternal.hpp"

namespace sea_net { 

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

Profiling::Profiling(bool usePTS): Protocol(0x14,0xFF,usePTS) //0x14 means Profiling Profiling
{
    headDataChanged=false;
}

Profiling::~Profiling() {
}


void Profiling::processHeadData(u8 *message){
        base::Time timestamp = base::Time::now();
        uint8_t nodeType = message[12];
        uint16_t packedSize    = message[13] | (message[14]<<8);
	if(nodeType==rxNode){
		uint8_t  sweep_code      = message[17];
		uint16_t hd_Ctrl        = message[18] | (message[19]<<8);
		uint16_t range		= message[20] | (message[21]<<8);
		uint32_t txn           	= message[22] | (message[23]<<8) | (message[24]<<16) | (message[25]<<24);
		uint8_t gain           	= message[26];
		uint16_t leftLim        = message[31] | (message[32]<<8);
		uint16_t rightLim       = message[33] | (message[34]<<8);
		uint8_t  stepSize      	= message[35];
		uint16_t scanTime       = message[36] | (message[37]<<8);
		uint16_t noPings        = message[38] | (message[39]<<8);
		uint8_t *scanData	= message+40;
		uint16_t dataEnd = 40+(noPings*2);
		if(message[dataEnd+10] != PACKED_END){
#if 0 //debug
			printf("Bla Kaputt: %c\n",message[0]);
			for(int i=dataEnd-2;i<dataEnd+20;i++){
				printf(" %02x",message[i]);
			}
			printf("\n");
			printf("No of Pings: %i, len %i\n",noPings,packedSize);
			fprintf(stderr,"Cannot Parse Head message\n");
#endif
		}else{
			uint16_t data[noPings];
			for(int i=0;i<noPings;i++){
				data[i] = scanData[(i*2)] | (scanData[(i*2)+1]<<8);
			}
			notifyPeers(ProfilerScan(sweep_code,hd_Ctrl,range,txn,gain,leftLim,rightLim,stepSize,scanTime,noPings,data));
		}	
	}else{
		fprintf(stderr,"Unknown Device\n");
	}
}

headControl Profiling::getDefaultHeadData(){
	headControl hc;
	hc.V3BParams = 0x1D;
	hc.headCtl = PRF_ALT | CHAN2 |  PRF_FIRST | HASMOT | PRF_MASTER ;
	hc.headType = 5;
	hc.txnch1 = 77846282;
	hc.txnch2 = 162403450;
	hc.rxnch1 = 138915348;
	hc.rxnch2 = 223472517;
	hc.txPulseLength=30;
	hc.rangeScale = 20;
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
   
void Profiling::sendHeadData(headControl hc){
	memcpy(headData,&hc,sizeof(hc));	
	headDataChanged=true;	
}

};};

