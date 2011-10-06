/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include "SeaNet.h"
#include <stdio.h>
#include <string.h>
#include <boost/thread/thread.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define WRITETIMEOUT 400

namespace SeaNet{

Protocol::Protocol(uint8_t rxNode, uint8_t txNode, bool createPts): 
  IODriver(MAX_PACKET_SIZE,false),
  rxNode(rxNode),
  txNode(txNode),
  createPts(createPts),
  pts_fd(0)
{
    headDataChanged=false;
    initialized=false;
    timeCnt=0;
}

Protocol::~Protocol() {
}

void Protocol::reboot()
{
    sendPacked(mtReBoot,0);
}

void Protocol::requestVersion()
{
    sendPacked(mtSendVersion,0);
}

void Protocol::requestData() {
    if(headDataChanged)
   	sendPacked(mtHeadCommand,headData);
    sendPacked(mtSendData,0);
}

void Protocol::sendPacked(MsgType type, uint8_t *data) {

    uint16_t length = 5+MsgLength[type];
    uint8_t msg[length+6];
    //Header
    msg[0] = '@';
    //Length in ASCII and binary
    snprintf((char*)msg+1,5,"%04X",length);

    msg[5] = length & 255;
    msg[6] = (length>>8) & 255;
    msg[7] = txNode;
    msg[8] = rxNode;
    msg[9] = MsgLength[type];
    switch (type) {

    case mtNull:
        fprintf(stderr,"Cannot send null-Packet\n");
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
	*/
        case mtReBoot:
        	msg[10] = mtReBoot;
	        msg[11] = 0x80;
	        msg[12] = rxNode;
	        msg[13] = 0x0A;
        	break;
	/*
        case mtPerformanceData:
        	break;
        */
    case mtHeadCommand:
        msg[10] = mtHeadCommand;
        msg[11] = 0x80;
        msg[12] = rxNode;
        memcpy(msg+13,data,68);
        msg[81] = PACKED_END;
	headDataChanged=false;
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
        msg[13] = PACKED_END;
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
        uint32_t time = timeCnt++; //TODO zeit ausm system holen
        memcpy(msg+13,&time,4);
        msg[17]= PACKED_END;
        break;
    }
    /*case mtSendPreferenceData:
    	break;*/
    default:
        fprintf(stderr,"Cannot create unknown packet with id: %02X \n",type);
    }

    
    bool written = writePacket(msg,length+6,WRITETIMEOUT);
	

    if (!written) {
        fprintf(stderr,"Couldn't send Packet\n");
#if 0 //Debug code
    } else {
	fprintf(stdout,"Sended packet with length: %i\n",(length+6));
        for (int i=0;i< length+6;i++) {
            fprintf(stdout,"%02X ",msg[i]);
        }
        fprintf(stdout,"\n");
#endif 
    }

}

bool Protocol::processSerialData(int timeout){
	uint8_t packed[MAX_PACKET_SIZE];
	for(int i=0;i<MAX_PACKET_SIZE;i++)packed[i]=0;

	try{
		readPacket(packed,MAX_PACKET_SIZE,timeout,timeout);
		processMessage(packed);
                return true;
      	}catch(timeout_error t) {
		printf("Timeout from readPacked\n");
	}
        return false;
}

/**
 * Given message started on binary lengh field
 **/
void Protocol::processMessage(uint8_t *message) {

    base::Time timestamp = base::Time::now();
    uint8_t type = message[10];

    switch (type) {
    case mtNull:
        fprintf(stderr,"Cannot handle null-Packet\n");
        break;
    case mtVersionData:
    {
        softwareVersion = message[13];
        infoBits = message[14];
        serialNr = message[15] | (message[16] <<8);
        programmLength = message[17] | (message[18] <<8) | (message[19] <<16) | (message[20] <<24);
        programmChecksum = message[21] | (message[22] << 8);
        nodeID = message[23];
        fprintf(stdout,"Software Version: 0x%02X\n",softwareVersion);
        fprintf(stdout,"InfoBits: 0x%02X\n",infoBits);
        fprintf(stdout,"Serial Number: %u\n",serialNr);
        fprintf(stdout,"Programm Length: %u\n",programmLength);
        fprintf(stdout,"Programm Checksum: %u\n",programmChecksum);
        fprintf(stdout,"Node ID: %u\n",nodeID);
        break;
    }
    case mtHeadData:
	processHeadData(message);
        break;
    case mtSpectData:
        fprintf(stderr,"Cannot handle SpectData-Packet\n");
        break;
    case mtAlive:
        lastKeepAlive = timestamp;
	requestData();
	requestData();
        fprintf(stderr,"Got an Alive packet, Found Sonar, should not happten, rerequesting Data!\n");
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
	uint16_t packedSize = message[13] | (message[14]<<8);
	if(pts_fd){
		int toWrite = packedSize+1;
		while(toWrite > 0){
			int written = write(pts_fd,message+15,toWrite);
			if(written > 0){
				toWrite-=written;
			}else{
				fprintf(stderr,"Cannot write to PTS\n\t%s\n",strerror(errno));
				break;
			}
		}
	}
        if(message[15+packedSize-1] == 0x0d){ //end of depth message 
            char tmp[packedSize];
            memcpy(tmp,message+15,packedSize);
            tmp[packedSize-1] = '\0'; //make sure string ends correctly
            int depth = 0;
            if(sscanf(tmp,"%dmm",&depth) == 1){ //Parsed like 14mm packge
                double reading = (float)depth/1000.0;
                GroundDistance gd;
                gd.time = base::Time::now();
                gd.distance = reading;
                notifyPeers(gd);
                //printf("Current Depth is: %f\n",reading);
            }
        }

        printf("\n");

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


void Protocol::registerHandler(SonarHandler *handler){
	handlers.push_back(handler);
}

void Protocol::unregisterHandler(SonarHandler *handler){
	handlers.remove(handler);
}

void Protocol::notifyPeers(SonarScan const &scan){
	for(std::list<SonarHandler*>::iterator it = handlers.begin(); it != handlers.end();it++){
		(*it)->processSonarScan(&scan);
	}
}

bool Protocol::init(std::string const &port, int speed){
	if(createPts){
		pts_fd = posix_openpt(O_RDWR | O_NOCTTY);	
		if(pts_fd > 0){
			pts_slave = ptsname(pts_fd);	
			if(pts_slave < 0){
				fprintf(stderr,"Cannot get Virtual Sub TTY for subdevice\n\t%s\n",strerror(errno));
				return false;
			}
                        if (grantpt(pts_fd) < 0){
                            ::close(pts_fd);
                            return false;
                        }
                        if (unlockpt(pts_fd) < 0){
                            ::close(pts_fd);
                            return false;
                        }

		}else{
			fprintf(stderr,"Cannot create Virtual TTY:\n\t%s\n",strerror(errno));
			return false;
		}
	}
	return openSerial(port,speed);
}

const char *Protocol::getSlavePTS(){
	return pts_slave;
}

int Protocol::extractPacket(uint8_t const* buffer, size_t buffer_size) const{
	if(buffer_size<7) return 0;
		
	size_t readPos=0;

	//Packed starts with an @ every time, so search for it
        while (buffer[readPos] != '@' && readPos < buffer_size) {
		readPos++;
        }
	
        //We are at the end and cannot find the start
	if(readPos>0){
#if 0
		fprintf(stdout,"Skipping %lu bytes.\n",readPos);
#endif
		return -readPos;
	}
	

	//Double check length, it's twice in the stream, once in binay, once in ascii coded hex value
        size_t len = (buffer[5] | (buffer[6]<<8 )) +5;
	uint16_t hexlen = 0;
	char hexsum[5];
	memcpy(hexsum,buffer+1,4);
	hexsum[4]=0;
	
	sscanf(hexsum,"%hx",&hexlen);

        //packed len is smaller than hexlen, use it as simple package check
	if(len-5 != hexlen){
#if 0
		printf("Kaput %lu\n", buffer_size);
		printf("hex: %c%c%c%c\n",buffer[1],buffer[2],buffer[3],buffer[4]);
		printf("hex: %x %x %x %x\n",buffer[1],buffer[2],buffer[3],buffer[4]);
		printf("bin: %i %i\n",buffer[5],buffer[6]);
		printf("0x%02x (%c)\n",buffer[0],buffer[0]);
		printf("len is: %lu, hexlen is %hu\n",len-5,hexlen);
		//Seems this is not the real packed start so we skip...
		
		fprintf(stdout,"Message:\n");
            	for (unsigned int i=0;i<buffer_size;i++)
                	fprintf(stderr,"%02X ",buffer[i]);
		fprintf(stderr,"\n");
#endif
		return -1;
	}
	
	if(len+1 > buffer_size){
		//Packed Start correct, but not complete yet
		return 0;
	}

        //Cecking for the end of thepackage, if everything is ok, returning it
        if (buffer[len] == PACKED_END || buffer[10] == 0x08) {
            	return len+1;
        } else {
#if 0
            fprintf(stderr,"Message doesn't ended with an newline was(%lu): %02X, skip message:\n",len,buffer[len]);
#endif
	    return -1;
        }

	//this should be never reached
        return -1;
}


};

