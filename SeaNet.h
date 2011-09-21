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


/**
 * Length definition of packages
 * 0 menas (most time) not implemented yet
 */
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


/**
 * Classes which would use the sonar scans must be implement this handler and check
 * by an cast which type of sonarScan is arrived
 */
class SonarHandler{
public:
	virtual void processSonarScan(const SonarScan *scan)=0;
};


/**
 * Each Tritech (aka SeaNet Protocoll) device can derivate from this class und setting
 * the head data and must be implement an handler for head data
 * the whole protocoll handling is done by this class
 */
class Protocol : public IODriver
{

public:
        /**
         * rxNode, txNode Node identifier, unique for each device-class
         * createPTS, should be  subdevice created for aux-data?
         */
	Protocol(const uint8_t rxNode, const uint8_t txNode, bool createPTS=false);
	~Protocol();

        /**
         * Requesting Firmware version of device
         */
	void requestVersion();

        /**
         * virtual function that processes the incomming message from the sonars
         */
	virtual void processHeadData(u8 *message)=0;

        /**
         * Requesting next scan
         */
	void requestData();
	
        /**
         * Handler based mode, register an new subscriber
         */
        void registerHandler(SonarHandler*);
	void unregisterHandler(SonarHandler*);

        /**
         * Initializaes the port
         */
	virtual bool init(std::string const &port, int speed=115200);
	bool processSerialData(int timeout = 400);
	void reboot();

        /**
         * Gets the uni file corresponding to the creates psydo tty
         */
	const char *getSlavePTS();

        base::Time lastKeepAlive;
	base::Time lastPackage;

protected:
        static const char PACKED_END = 0x0A;
	bool initialized;
	
        void notifyPeers(SonarScan const &scan);
	std::list<SonarHandler*> handlers;
	void sendPacked(MsgType type,uint8_t *data);
	void processMessage(uint8_t *message);
	int timeCnt;
	const uint8_t rxNode;
	const uint8_t txNode;
	const bool createPts;
	int pts_fd;
	char *pts_slave;

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

