/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#ifndef _SONARSCAN_H_
#define _SONARSCAN_H_

#include <stdint.h>
#include <base/time.h>
#include <vector>


class ProfilerScan{
public:
	base::Time time;
	uint16_t range;
	uint32_t TxN;
	uint8_t gain;
	uint16_t leftLimit;
	uint16_t rightLimit;
	uint8_t stepSize;
	uint16_t scanTime;
	std::vector<uint16_t> scanData;


ProfilerScan(
	uint16_t range,
	uint32_t TxN,
	uint8_t gain,
	uint16_t leftLimit,
	uint16_t rightLimit,
	uint8_t stepSize,
	uint16_t scanTime,
	uint16_t dataSize,
	uint16_t *data
);

};

class SonarScan {
public:
    base::Time time;

SonarScan();

SonarScan(SonarScan const& other);

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
	std::vector<uint8_t> scanData;

	double getScale() const;
};

std::ostream& operator<<(std::ostream &stream, const SonarScan& scan);
std::istream& operator>>(std::istream& stream, SonarScan& scan);

#endif
