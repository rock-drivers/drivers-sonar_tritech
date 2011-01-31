/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2011
 * Desc:
 *
*/
#ifndef _SEANET_H_
#define _SEANET_H_ 

#include "SonarScan.h"
#include <list>
#include <iodrivers_base.hh>
#include <base/time.h>


namespace SeaNet{


typedef uint16_t u16;
typedef uint8_t  u8;
typedef uint32_t u32;

class SonarInterface;



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


class SonarHandler{
public:
	virtual void processSonarScan(const SonarScan *scan)=0;
};


class Protocol : public IODriver
{

public:

	Protocol(const uint8_t rxNode, const uint8_t txNode);
	~Protocol();
	void requestVersion();

///	virtual void sendHeadData()=0;
	virtual void processHeadData(u8 *message)=0;
	void requestData();
	
	void registerHandler(SonarHandler*);
	void unregisterHandler(SonarHandler*);
	bool init(std::string const &port, int speed=115200);
	bool processSerialData(int timeout = 400);
	void reboot();

        base::Time lastKeepAlive;
	base::Time lastPackage;

protected:
	bool initialized;
	//int waitCounter;
/*
	void notifyPeers(base::Time const& time, float depth);
	void notifyPeers(SonarScan const& scan);
	void notifyPeers(ProfilerScan const& scan);
	uint8_t scanData[6369*256];	
*/
	void notifyPeers(SonarScan const &scan);
	std::list<SonarHandler*> handlers;
	void sendPacked(MsgType type,uint8_t *data);
	void processMessage(uint8_t *message);
	//int timeCnt;
	const uint8_t rxNode;
	const uint8_t txNode;
	//int fileCnt;
	static const int MAX_PACKET_SIZE = 5000;
	virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const;

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


};

#endif

