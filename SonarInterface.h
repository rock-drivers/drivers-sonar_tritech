#ifndef _SONARINTERFACE_H_
#define _SONARINTERFACE_H_ 

#include <QThread>
#include <QTimer>
#include "SonarScan.h"

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

class SonarInterface : public QThread
{
	Q_OBJECT

public:
	SonarInterface(char* port);
	~SonarInterface();
	void requestVersion();
	void sendHeadData(bool adc8on,bool cont,bool scanright,bool invert,bool chan2,bool applyoffset,
		bool pingpong,uint16_t rangeScale, uint16_t leftLimit, uint16_t rightLimit, uint8_t adSpan, 
		uint8_t adLow, uint8_t initialGain, uint8_t motorStepDelayTime, uint8_t motorStepAngleSize,
		uint16_t adInterval, uint16_t numberOfBins, uint16_t adcSetpoint);
	//void sendHeadData();
	void requestData();
	uint8_t scanData[6369*256];	


public slots:
	void readTimeout();

signals:
	void scanComplete(SonarScan*);
	void newDepth(float);

private:
	void run();
	void sendPacked(MsgType type,uint8_t *data);
	void processMessage(uint8_t *message);
	QTimer readTimer;
	int readFD;
	int timeCnt;
	int writeFD;
	uint8_t txNode;
	uint8_t rxNode;
	int fileCnt;
	bool initialized;
	int skippedBytes;


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
