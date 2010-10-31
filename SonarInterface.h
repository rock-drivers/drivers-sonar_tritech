/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#ifndef _SONARINTERFACE_H_
#define _SONARINTERFACE_H_ 

#include "SonarScan.h"
#include <list>
#include <iodrivers_base.hh>
#include <base/time.h>

class SonarInterface;

class SonarHandler{
	public:
	virtual void processDepth(base::Time const& time, double depth)=0;
	virtual void processSonarScan(SonarScan const&)=0;
};

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

enum MsgType{
	mtNull = 0,
	mtVersionData,
	mtHeadData,
	mtSpectData,
	mtAlive,
	mtPrgAck,
	mtBBUserData,
	mtTestData,
	mtAuxData,
	mtAdcData,
	mtLdcReq,
	na11,
	na12,
	mtLanStatus,
	mtSetTime,
	mtTimeout,
	mtReBoot,
	mtPerformanceData,
	na18,
	mtHeadCommand,
	mtEraseSector,
	mtProgBlock,
	mtCopyBootBlk,
	mtSendVersion,
	mtSendBBUser,
	mtSendData,
	mtSendPreferenceData
};

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

class SonarInterface : public IODriver
{

public:
	SonarInterface();
	~SonarInterface();
	void requestVersion();
	void sendHeadData(bool adc8on=true,bool cont=true,bool scanright=false,bool invert=false,bool chan2=false,bool applyoffset=false,
	bool pingpong=false,uint16_t rangeScale=30, uint16_t leftLimit=1, uint16_t rightLimit=6399, uint8_t adSpan=false, 
	uint8_t adLow=8, uint8_t initialGain=84, uint8_t motorStepDelayTime=25, uint8_t motorStepAngleSize=16,
	uint16_t adInterval=20, uint16_t numberOfBins=600, uint16_t adcSetpoint=0);
	void requestData();
	void registerHandler(SonarHandler*);
	void unregisterHandler(SonarHandler*);
	bool init(std::string const &port);
	bool processSerialData(int timeout = 400);
	void reboot();

        base::Time lastKeepAlive;
	base::Time lastPackage;

private:
	bool initialized;
	int waitCounter;
	void notifyPeers(base::Time const& time, float depth);
	void notifyPeers(SonarScan const& scan);
	uint8_t scanData[6369*256];	
	std::list<SonarHandler*> handlers;
	void sendPacked(MsgType type,uint8_t *data);
	void processMessage(uint8_t *message);
	int timeCnt;
	uint8_t txNode;
	uint8_t rxNode;
	int fileCnt;
	static const int MAX_PACKET_SIZE = 9048;
	virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const;

	uint8_t currentMotorStepAngleSize;
    uint8_t headData[68];
    bool headDataChanged;

/* Sonar informations */
      /* Informations from HeadCommand */
      uint8_t softwareVersion;
      uint8_t infoBits;
      uint8_t serialNr;
      uint8_t nodeID;
      uint32_t programmLength;
      uint16_t programmChecksum;

};

#endif
