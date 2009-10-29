#ifndef _SONARSCAN_H_
#define _SONARSCAN_H_

#include "inttypes.h"
#include <QDataStream>


class SonarScan {
public:
SonarScan();
SonarScan(
	uint16_t packedSize,
	uint8_t deviceType,
	uint8_t headStatus,
	uint8_t sweepCode,
	uint16_t headControl,
	uint16_t range,
	uint32_t txn,
	uint8_t gain,
	uint16_t slope,
	uint8_t adSpawn,
	uint8_t adLow,
	uint16_t headingOffset,
	uint16_t adInterval,
	uint16_t leftLimit,
	uint16_t rightLimit,
	uint8_t steps,
	uint16_t bearing,
        uint16_t dataBytes,
	uint8_t *scanData
);

  virtual ~SonarScan();
  uint16_t getpackedSize();
  uint8_t getdeviceType();
  uint8_t getheadStatus();
  uint8_t getsweepCode();
  uint16_t getheadControl();
  uint16_t getrange();
  uint32_t gettxn();
  uint8_t getgain();
  uint16_t getslope();
  uint8_t getadSpawn();
  uint8_t getadLow();
  uint16_t getheadingOffset();
  uint16_t getadInterval();
  uint16_t getleftLimit();
  uint16_t getrightLimit();
  uint8_t getsteps();
  uint16_t getbearing();
  uint16_t getdataBytes();
  const uint8_t* getScanData();


private:
	uint16_t packedSize;
	uint8_t deviceType;
	uint8_t headStatus;
	uint8_t sweepCode;
	uint16_t headControl;
	uint16_t range;
	uint32_t txn;
	uint8_t gain;
	uint16_t slope;
	uint8_t adSpawn;
	uint8_t adLow;
	uint16_t headingOffset;
	uint16_t adInterval;
	uint16_t leftLimit;
	uint16_t rightLimit;
	uint8_t steps;
	uint16_t bearing;
        uint16_t dataBytes;
	uint8_t *scanData;

  friend QDataStream& operator<<(QDataStream&, const SonarScan& scan);
  friend QDataStream& operator>>(QDataStream&, SonarScan& scan);

};

  QDataStream& operator<<(QDataStream&, const SonarScan& scan);
  QDataStream& operator>>(QDataStream&, SonarScan& scan);
//QDataStream &operator>>(SonarScan &s);

#endif
