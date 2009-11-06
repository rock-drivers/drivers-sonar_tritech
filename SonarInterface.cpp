#include "SonarInterface.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


SonarInterface::SonarInterface(const char* port) {
    readFD = open(port, (O_RDONLY | O_SYNC) );
    writeFD = open(port, (O_WRONLY | O_SYNC) );
    if (readFD == 0 || writeFD == 0) {
        fprintf(stderr,"Cannot Open Device\n");
        exit(-2);
    }
    //Device Adresses
    rxNode = 0x02;
    txNode = 0xFF;
    readTimer.setSingleShot(false);
    readTimer.start();
    readTimer.setInterval(2000);
    connect(&readTimer,SIGNAL(timeout()),this,SLOT(readTimeout()));
    timeCnt=0;
    fileCnt=0;
    initialized=false;
    skippedBytes=0;
}

void SonarInterface::readTimeout() {
    requestData();
    fprintf(stderr,"Timeout for waiting data reached\n");
}

SonarInterface::~SonarInterface() {
}

void SonarInterface::requestVersion()
{
    sendPacked(mtSendVersion,0);
}

void SonarInterface::requestData() {
    readTimer.start(); //Restart
    sendPacked(mtSendData,0);
}

void SonarInterface::sendHeadData(bool adc8on,bool cont,bool scanright,bool invert,bool chan2,bool applyoffset,
		bool pingpong,uint16_t rangeScale, uint16_t leftLimit, uint16_t rightLimit, uint8_t adSpan, 
		uint8_t adLow, uint8_t initialGain, uint8_t motorStepDelayTime, uint8_t motorStepAngleSize,
		uint16_t adInterval, uint16_t numberOfBins, uint16_t adcSetpoint) {
    uint8_t data[68];

    uint8_t V3BParams = 0x1D;
    //bool adc8on = true;
    //bool cont = true;
    //bool scanright = false;
    //bool invert = true;
    bool motorOff = false;
    bool txOff= false;
    bool spare = false;
    //bool chan2 = true;
    bool raw = true;
    bool hasmot = true;
    //bool applyoffset = false;
    //bool pingpong = false;
    bool starellim = false;
    bool replyasl = true;
    bool replyThr = false;
    bool ignoreSensor = false;
    uint8_t headType = 0x02;
    uint32_t txnCh1 = 0; //Ignored
    uint32_t txnCh2 = 0; //Ignored
    uint32_t rxnCh1 = 0; //Ignored
    uint32_t rxnCh2 = 0; //Ignored
    uint16_t pulseLength = (0x28) | 0; //Ignored
    //uint16_t rangeScale  = (0x3C) | 0; //6 Meter
    //uint16_t leftLimit   = (0x01) | 0; //1
    //uint16_t rightLimit  = (0xFF) | (0x18<<8);
    //uint8_t adSpan = 0x51;
    //uint8_t adLow = 0x8;
    //uint8_t initialGainCh1 = 0x90;//54;
    //uint8_t initialGainCh2 = 0x90;//54;
    uint8_t initialGainCh1 = initialGain;
    uint8_t initialGainCh2 = initialGain;
    uint16_t slopeCh1 = 0; //Ignored
    uint16_t slopeCh2 = 0; //Ignored
    //uint8_t motorStepDelayTime = 0x19;
    //uint8_t motorStepAngleSize = 0x10;
    //uint16_t adInterval = (0x8D) | (0<<8);
    //uint16_t numberOfBins = (0xFF) | (0<<8);
    uint16_t maxADBuff = (0xE8) | (3<<8);
    uint16_t lockoutTime = (0x97) | (0x03<<8);
    uint16_t minorAxisDir = (0x40) | (0x06<<8);
    uint8_t majorAxisPan = (0x01);
    uint8_t crtl2 = 0;
    uint16_t scanZ = 0;
    
    //uint8_t adSpanCh1 = 0x50;
    //uint8_t adSpanCh2 = 0x51;
    //uint8_t adLowCh1 = 0x09;
    //uint8_t adLowCh2 = 0x08;
    //uint8_t initialGainCh1_2 = 0x90;//0x54;
    //uint8_t initialGainCh2_2 = 0x90;//0x54;
    //uint8_t adcSetpointCh1 = 0x00;
    //uint8_t adcSetpointCh2 = 0x00;
    
    uint8_t adSpanCh1 = adSpan;
    uint8_t adSpanCh2 = adSpan;
    uint8_t adLowCh1 = adLow;
    uint8_t adLowCh2 = adLow;
    uint8_t initialGainCh1_2 = initialGain;
    uint8_t initialGainCh2_2 = initialGain;
    uint8_t adcSetpointCh1 = adcSetpoint;
    uint8_t adcSetpointCh2 = adcSetpoint;

    uint16_t slopeCh1_2 =0; //Ignored
    uint16_t slopeCh2_2 =0; //Ignored
    uint16_t slopeDelayCh1 =0; //Ignored
    uint16_t slopeDelayCh2 =0; //Ignored

    uint16_t headControl =
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

    //headControl = (headControl&255)<<8 | (headControl>>8)&255;

    int pos=0;
    memcpy(data,&V3BParams,1);
    pos+=1;
    memcpy((data+pos),&headControl,2);
    pos+=2;
    memcpy((data+pos),&headType,1);
    pos+=1;
    memcpy((data+pos),&txnCh1,4);
    pos+=4;
    memcpy((data+pos),&txnCh2,4);
    pos+=4;
    memcpy((data+pos),&rxnCh1,4);
    pos+=4;
    memcpy((data+pos),&rxnCh2,4);
    pos+=4;
    memcpy((data+pos),&pulseLength,2);
    pos+=2;
    memcpy((data+pos),&rangeScale,2);
    pos+=2;
    memcpy((data+pos),&leftLimit,2);
    pos+=2;
    memcpy((data+pos),&rightLimit,2);
    pos+=2;
    memcpy((data+pos),&adSpan,1);
    pos++;
    memcpy((data+pos),&adLow,1);
    pos++;
    memcpy((data+pos),&initialGainCh1,1);
    pos++;
    memcpy((data+pos),&initialGainCh2,1);
    pos++;
    memcpy((data+pos),&slopeCh1,2);
    pos+=2;
    memcpy((data+pos),&slopeCh2,2);
    pos+=2;
    memcpy((data+pos),&motorStepDelayTime,1);
    pos++;
    memcpy((data+pos),&motorStepAngleSize,1);
    pos++;
    memcpy((data+pos),&adInterval,2);
    pos+=2;
    memcpy((data+pos),&numberOfBins,2);
    pos+=2;
    memcpy((data+pos),&maxADBuff,2);
    pos+=2;
    memcpy((data+pos),&lockoutTime,2);
    pos+=2;
    memcpy((data+pos),&minorAxisDir,2);
    pos+=2;
    memcpy((data+pos),&majorAxisPan,1);
    pos++;
    memcpy((data+pos),&crtl2,1);
    pos++;
    memcpy((data+pos),&scanZ,2);
    pos+=2;
    memcpy((data+pos),&adSpanCh1,1);
    pos++;
    memcpy((data+pos),&adSpanCh2,1);
    pos++;
    memcpy((data+pos),&adLowCh1,1);
    pos++;
    memcpy((data+pos),&adLowCh2,1);
    pos++;
    memcpy((data+pos),&initialGainCh1_2,1);
    pos++;
    memcpy((data+pos),&initialGainCh2_2,1);
    pos++;
    memcpy((data+pos),&adcSetpointCh1,1);
    pos++;
    memcpy((data+pos),&adcSetpointCh2,1);
    pos++;
    memcpy((data+pos),&slopeCh1_2,2);
    pos+=2;
    memcpy((data+pos),&slopeCh2_2,2);
    pos+=2;
    memcpy((data+pos),&slopeDelayCh1,2);
    pos+=2;
    memcpy((data+pos),&slopeDelayCh2,2);
    pos+=2;
/*
fprintf(stdout,"\n%s,%s,%s,%s,%s,%s,%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",mcr->getadc8on()?"true":"false",mcr->getcont()?"true":"false",
  mcr->getscanright()?"true":"false",mcr->getinvert()?"true":"false",mcr->getchan2()?"true":"false",mcr->getapplyoffset()?"true":"false",
  mcr->getpingpong()?"true":"false",mcr->getrangeScale(),mcr->getleftLimit(),mcr->getrightLimit(),mcr->getadSpan(),mcr->getadLow(),mcr->getinitialGain(),
  mcr->getmotorStepDelayTime(),mcr->getmotorStepAngleSize(),mcr->getadInterval(),mcr->getnumberOfBins(),mcr->getadcSetpointCh());
  */
  /*
  fprintf(stdout,"Head command data:\n");
  for(int i=0;i<68;i++){
    	fprintf(stdout,"%02X ",data[i]);
   }
   fprintf(stdout,"Head command data Finished.\n");
  */
   sendPacked(mtHeadCommand,data);
}

void SonarInterface::sendPacked(MsgType type, uint8_t *data) {

    uint16_t length = 5+MsgLength[type]; //TODO
    uint8_t msg[length+6];
    //Header
    msg[0] = '@';
    //Length in ASCII and binary
    snprintf((char*)msg+1,5,"%04X",length);

    //printf((char*)msg+1);

    msg[5] = length & 255;
    msg[6] = (length>>8) & 255;
    msg[7] = txNode;
    msg[8] = rxNode;
    msg[9] = MsgLength[type];
    switch (type) {

    case mtNull:
        fprintf(stderr,"Cannot create null-Packet\n");
        break;
        /*
        case mtHeadData:
        	break;
        case mtSpectData:
        	break;
        case mtAlive:
        	break;
        case mtPrgAck:
        	break;
        case mtBBUserData:
        	break;
        case mtTestData:
        	break;
        case mtAuxData:
        	break;
        case mtAdcData:
        	break;
        case mtLdcReq:
        	break;
        case mtLanStatus:
        	break;
        case mtSetTime:
        	break;
        case mtTimeout:
        	break;
        case mtReBoot:
        	break;
        case mtPerformanceData:
        	break;
        	*/
    case mtHeadCommand:
        msg[10] = mtHeadCommand;
        msg[11] = 0x80;
        msg[12] = rxNode;
        memcpy(msg+13,data,68);
        msg[81] = 0x0A;
        break;
        /*
        case mtEraseSector:
        	break;
        case mtProgBlock:
        	break;
        case mtCopyBootBlk:
        	break;*/
    case mtSendVersion:
    {
        msg[10] = mtSendVersion;
        msg[11] = 0x80;
        msg[12] = rxNode;
        msg[13] = 0x0A;
        break;
    }
    /*
    case mtSendBBUser:
    	break;
    */
    case mtSendData:
    {
        msg[10] = mtSendData;
        msg[11] = 0x80;
        msg[12] = rxNode;
        uint32_t time = 134145+timeCnt++; //TODO zeit ausm system holen
        memcpy(msg+13,&time,4);
        msg[17]=0x0A;
        break;
    }
    /*case mtSendPreferenceData:
    	break;*/
    default:
        fprintf(stderr,"Cannot create unknown packet with id: %02X \n",type);
    }

    
    int written = write(writeFD,msg,length+6);
    if (written != length+6) {
        fprintf(stderr,"Couldn't send Packet\n");
    } else {
        /*
	fprintf(stdout,"Sended packet with length: %i\n",(length+6));
        for (int i=0;i< length+6;i++) {
            fprintf(stdout,"%02X ",msg[i]);
        }
        fprintf(stdout,"\n");
	*/
    }

}


/**
 * Given message started on binary lengh field
 **/
void SonarInterface::processMessage(uint8_t *message) {

    uint8_t type = message[5];

    switch (type) {

    case mtNull:
        fprintf(stderr,"Cannot handle null-Packet\n");
        break;
    case mtVersionData:
    {
        softwareVersion = message[8];
        infoBits = message[9];
        serialNr = message[10] | (message[11] <<8);
        programmLength = message[12] | (message[13] <<8) | (message[14] <<16) | (message[15] <<24);
        programmChecksum = message[16] | (message[17] << 8);
        nodeID = message[18];
        fprintf(stdout,"Software Version: 0x%02X\n",softwareVersion);
        fprintf(stdout,"InfoBits: 0x%02X\n",infoBits);
        fprintf(stdout,"Serial Number: %u\n",serialNr);
        fprintf(stdout,"Programm Length: %u\n",programmLength);
        fprintf(stdout,"Programm Checksum: %u\n",programmChecksum);
        fprintf(stdout,"Node ID: %u\n",nodeID);
        initialized=true;
        break;
    }
    case mtHeadData:
    {
    uint16_t packedSize = message[8] | (message[9]<<8);
	uint8_t deviceType = message[10];
	uint8_t headStatus = message[11];
	uint8_t sweepCode = message[12];
	uint16_t headControl = message[13] | (message[14]<<8);
	uint16_t range = message[15] | (message[16]<<8);
	uint32_t txn = message[17] | (message[18]<<8) | (message[19]<<16) | (message[20]<<24);
	uint8_t gain = message[21];
	uint16_t slope = message[22] | (message[23]<<8);
	uint8_t adSpawn = message[24];
	uint8_t adLow = message[25];
	uint16_t headingOffset = message[26] | (message[27]<<8);
	uint16_t adInterval = message[28] | (message[29]<<8);
	uint16_t leftLimit = message[30] | (message[31]<<8);
	uint16_t rightLimit = message[32] | (message[33]<<8);
	uint8_t steps = message[34];
	uint16_t bearing = message[35] | (message[36]<<8);
    uint16_t dataBytes = message[37] | (message[38]<<8);

	
	//uint8_t debug[dataBytes];
	
	//for(int i=0;i<dataBytes;i++){
	//	debug[i]=i;
	//}
	
	SonarScan *scan = new SonarScan(packedSize,deviceType,headStatus,sweepCode,headControl,range,txn,gain,slope,adSpawn,adLow,headingOffset,adInterval,leftLimit,rightLimit,steps,bearing,dataBytes,message+39);

	emit scanComplete(scan);
	
	/*
	for (int i=0;i<packedSize;i++) {
            printf("%02X ",message[i]);
        }
	*/
	//TODO remove hack
        //if (bearing==0) {
            //sendHeadData();
        //}
        uint8_t data[dataBytes];
        memcpy(data,message+39,dataBytes);
        //fprintf(stdout,"Actual Bearing: %u.\n",bearing);
        //fprintf(stdout,"DataBytes recived: %u.\n",dataBytes);
	/*
        for (int i=0;i<dataBytes;i++) {
            int writePos = ((bearing/16))+i*398;
            //printf("ScanPosition %i: %u (%u,%u,%u)\n",writePos,data[i],bearing,dataBytes,i);
            scanData[writePos] = data[i];
        }
	*/
	//fprintf(stdout,"Requesting data\n");
        requestData();

        //fprintf(stderr,"Cannot handle HeadData-Packet\n");
        break;
    }
    case mtSpectData:
        fprintf(stderr,"Cannot handle SpectData-Packet\n");
        break;
    case mtAlive:
        //requestData();
        fprintf(stderr,"Got an Alive packet, Found Sonar!\n");
        break;
    case mtPrgAck:
        fprintf(stderr,"Cannot handle PrgAck-Packet\n");
        break;
    case mtBBUserData:
        fprintf(stderr,"Cannot handle BBUserData-Packet\n");
        break;
    case mtTestData:
        fprintf(stderr,"Cannot handle TestrData-Packet\n");
        break;
    case mtAuxData:
		{
    	uint16_t packedSize = message[8] | (message[9]<<8);
		float depth=0;
		if(packedSize == 9 && message[13] == '.' && message[17] == 'm'){
			depth+= ((message[10]-48)) * 100;
			depth+= ((message[11]-48)) * 10;
			depth+= ((message[12]-48));
			depth+= ((message[14]-48))/10.0;
			depth+= ((message[15]-48))/100.0;
			depth+= ((message[16]-48))/1000.0;
		}else{
			fprintf(stderr,"Cannot HAndle Unknown-AUX port data %s:%s",__FUNCTION__,__FILE__);
		}
		
		//for(int i=10;i<packedSize+10;i++){
		//	fprintf(stdout,"%c ",message[i]);
		//}
		fprintf(stdout,"CalcDepth = %f\n",depth);
    		emit newDepth(depth);    
		break;
		}
    case mtAdcData:
        fprintf(stderr,"Cannot handle AdcData-Packet\n");
        break;
    case mtLdcReq:
        fprintf(stderr,"Cannot handle LdcReq-Packet\n");
        break;
    case mtLanStatus:
        fprintf(stderr,"Cannot handle LanStatus-Packet\n");
        break;
    case mtSetTime:
        fprintf(stderr,"Cannot handle SetTime-Packet\n");
        break;
    case mtTimeout:
        fprintf(stderr,"Cannot handle Timeout-Packet\n");
        break;
    case mtReBoot:
        fprintf(stderr,"Cannot handle ReBoot-Packet\n");
        break;
    case mtPerformanceData:
        fprintf(stderr,"Cannot handle PerformanceData-Packet\n");
        break;
    case mtHeadCommand:
        fprintf(stderr,"Cannot handle HeadCommand-Packet\n");
        break;
    case mtEraseSector:
        fprintf(stderr,"Cannot handle EraseSector-Packet\n");
        break;
    case mtProgBlock:
        fprintf(stderr,"Cannot handle ProgBlock-Packet\n");
        break;
    case mtCopyBootBlk:
        fprintf(stderr,"Cannot handle CopyBootBlock-Packet\n");
        break;
    case mtSendVersion:
        fprintf(stderr,"Cannot handle SendVersion-Packet\n");
        break;
    case mtSendBBUser:
        fprintf(stderr,"Cannot handle SendBBUser-Packet\n");
        break;
    case mtSendData:
        fprintf(stderr,"Cannot handle SendData-Packet\n");
        break;
    case mtSendPreferenceData:
        fprintf(stderr,"Cannot handle SendPreferenceData-Packet\n");
        break;
    default:
        fprintf(stderr,"Cannot Handle unknown packet\n");
    }
}


void SonarInterface::run() {
    uint8_t buffer[6000];
    //uint16_t pos=0;

    while (1) {
        //Todo besser handlen das man die ganzen header auf
        //einmal liest
        int readed= read(readFD,buffer,1);
        while (buffer[0] != '@') {
			fprintf(stdout," \n");
            read(readFD,buffer,1);
	    	fprintf(stdout,"%02X ",buffer[0]);
	    	skippedBytes++;
			fprintf(stdout," Skipped %i bytes.\n",skippedBytes);
        }
        //Header lesen
        readed = read(readFD,buffer,6);
        uint16_t len = buffer[4] | (buffer[5] << 8);

        uint8_t message[len];
        //Read 2 bytes less because we have aleready read the length
        //But we also tryiing to read the endline char into the msg
	int readLen = len-1;
	//int expectedLen=len-1;
	

        //readed=read(readFD,message+2,readLen) + 2;
	readed=read(readFD,message+2,readLen);
	int retrycount = 10;
	while(readed != readLen && retrycount--){
	  int newreaded = read(readFD,message+2+readed,readLen-readed);
	  //fprintf(stdout,"Tryinig again.. trying to read %i bytes (%i != %i) \n",(readLen-readed),(readed),readLen);
	  if(newreaded != -1){
	    readed+=newreaded;
	    usleep(10);
	  }else{
	    fprintf(stderr,"Cannot read the right bytes count so skipping\n");
	    break;
	  }
	}
        if (readed != readLen) {
            fprintf(stderr,"I'm not readed the right len %u|%u , oops\n",readed,len);
            continue;
        }


        if (message[len] == 0x0A) {
            message[0] = buffer[4];
            message[1] = buffer[5];
            processMessage(message);

            //char fname[200];
            //sprintf(fname,"message-%i.raw",fileCnt++);
            //int fd = open(fname,O_WRONLY | O_CREAT);
            //write(fd,message,readed-1);
            //close(fd);

        } else {
            fprintf(stderr,"Message doesn't ended with an newline was: %02X, skip message:\n",message[readed-1]);
            for (int i=0;i<=len;i++)
                fprintf(stderr,"%02X ",message[i]);
        }
    }
}