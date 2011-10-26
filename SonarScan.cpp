/*
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include "SonarScan.h"
#include <string.h>

MicronScan::MicronScan(MicronScan const& other):
	packedSize(other.packedSize),
	deviceType(other.deviceType),
	headStatus(other.headStatus),
	sweepCode(other.sweepCode),
	headControl(other.headControl),
	range(other.range),
	txn(other.txn),
	gain(other.gain),
	slope(other.slope),
	adSpawn(other.adSpawn),
	adLow(other.adLow),
	headingOffset(other.headingOffset),
	adInterval(other.adInterval),
	leftLimit(other.leftLimit),
	rightLimit(other.rightLimit),
	steps(other.steps),
	bearing(other.bearing)
{
    scanData = other.scanData;
}

MicronScan::MicronScan():
	packedSize(0),
	deviceType(0),
	headStatus(0),
	sweepCode(0),
	headControl(0),
	range(0),
	txn(0),
	gain(0),
	slope(0),
	adSpawn(0),
	adLow(0),
	headingOffset(0),
	adInterval(0),
	leftLimit(0),
	rightLimit(0),
	steps(0),
	bearing(0)
{

}

MicronScan::MicronScan(
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
):
	packedSize(packedSize),
	deviceType(deviceType),
	headStatus(headStatus),
	sweepCode(sweepCode),
	headControl(headControl),
	range(range),
	txn(txn),
	gain(gain),
	slope(slope),
	adSpawn(adSpawn),
	adLow(adLow),
	headingOffset(headingOffset),
	adInterval(adInterval),
	leftLimit(leftLimit),
	rightLimit(rightLimit),
	steps(steps),
	bearing(bearing)
{
    this->scanData.resize(dataBytes);
    memcpy(&this->scanData[0], scanData, dataBytes);
}


ProfilerScan::ProfilerScan(
	uint8_t  sweep_code,
        uint16_t hd_Ctrl,
        uint16_t range,
	uint32_t TxN,
	uint8_t gain,
	uint16_t leftLimit,
	uint16_t rightLimit,
	uint8_t stepSize,
	uint16_t scanTime,
	uint16_t dataSize,
	uint16_t *data
):
sweep_code(sweep_code),
hd_Ctrl(hd_Ctrl),
range(range),
TxN(TxN),
gain(gain),
leftLimit(leftLimit),
rightLimit(rightLimit),
stepSize(stepSize),
scanTime(scanTime){
	this->scanData.resize(dataSize);
	memcpy(&this->scanData[0],data,sizeof(this->scanData[0])*dataSize);
}

double MicronScan::getScale() const
{
  return  (((scanData.size()*adInterval*640.0)/1000000000.0)*1500.0/2.0)/scanData.size();
}

std::ostream& operator<<(std::ostream &stream, const MicronScan& scan){ 
  stream << scan.packedSize;
  stream << scan.deviceType;
  stream << scan.headStatus;
  stream << scan.sweepCode;
  stream << scan.headControl;
  stream << scan.range;
  stream << scan.txn;
  stream << scan.gain;
  stream << scan.slope;
  stream << scan.adSpawn;
  stream << scan.adLow;
  stream << scan.headingOffset;
  stream << scan.adInterval;
  stream << scan.leftLimit;
  stream << scan.rightLimit;
  stream << scan.steps;
  stream << scan.bearing;
  stream << scan.scanData.size();
  for(int i=0;i<scan.scanData.size();i++) 
  	stream << scan.scanData[i];	
  return stream;
}

std::istream& operator>>(std::istream& stream, MicronScan& scan){
  //stream >> scan.packedSize;
  //stream >> scan.deviceType;
  //stream >> scan.headStatus;
  //stream >> scan.sweepCode;
  //stream >> scan.headControl;
  //stream >> scan.range;
  //stream >> scan.txn;
  //stream >> scan.gain;
  //stream >> scan.slope;
  //stream >> scan.adSpawn;
  //stream >> scan.adLow;
  //stream >> scan.headingOffset;
  //stream >> scan.adInterval;
  //stream >> scan.leftLimit;
  //stream >> scan.rightLimit;
  //stream >> scan.steps;
  //stream >> scan.bearing;

  //size_t scan_size;
  //stream >> scan_size;
  //scan.scanData.resize(scan_size);
  //for(int i=0;i<scan_size;i++) 
  //	stream >> scan.scanData[i];	
  return stream;
}

