#include "SonarScan.h"
#include <string.h>

SonarScan::SonarScan(SonarScan *other):
	packedSize(other->packedSize),
	deviceType(other->deviceType),
	headStatus(other->headStatus),
	sweepCode(other->sweepCode),
	headControl(other->headControl),
	range(other->range),
	txn(other->txn),
	gain(other->gain),
	slope(other->slope),
	adSpawn(other->adSpawn),
	adLow(other->adLow),
	headingOffset(other->headingOffset),
	adInterval(other->adInterval),
	leftLimit(other->leftLimit),
	rightLimit(other->rightLimit),
	steps(other->steps),
	bearing(other->bearing),
    	dataBytes(other->dataBytes)
{
  this->scanData = new uint8_t[dataBytes];
  memcpy(this->scanData,other->scanData,dataBytes);
	
}

SonarScan::SonarScan():
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
	bearing(0),
    	dataBytes(0)
{

}

SonarScan::SonarScan(
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
	bearing(bearing),
    	dataBytes(dataBytes)
{
  this->scanData = new uint8_t[dataBytes];
  memcpy(this->scanData,scanData,dataBytes);
}

SonarScan::~SonarScan(){
  delete scanData;
}

uint16_t SonarScan::getpackedSize(){
  return packedSize;
}

uint8_t SonarScan::getdeviceType(){
  return deviceType;
}

uint8_t SonarScan::getheadStatus(){
  return headStatus;
}

uint8_t SonarScan::getsweepCode(){
  return sweepCode;
}

uint16_t SonarScan::getheadControl(){
  return headControl;
}

uint16_t SonarScan::getrange(){
  return range;
}

uint32_t SonarScan::gettxn(){
  return txn;
}

uint8_t SonarScan::getgain(){
  return gain;
}

uint16_t SonarScan::getslope(){
  return slope;
}

uint8_t SonarScan::getadSpawn(){
  return adSpawn;
}

uint8_t SonarScan::getadLow(){
  return adLow;
}

uint16_t SonarScan::getheadingOffset(){
  return headingOffset;
}

uint16_t SonarScan::getadInterval(){
  return adInterval;
}

uint16_t SonarScan::getleftLimit(){
  return leftLimit;
}

uint16_t SonarScan::getrightLimit(){
  return rightLimit;
}

uint8_t SonarScan::getsteps(){
  return steps;
}

uint16_t SonarScan::getbearing(){
  return bearing;
}

uint16_t SonarScan::getdataBytes(){
  return dataBytes;
}

const uint8_t* SonarScan::getScanData(){
  return scanData;
}



std::ostream& operator<<(std::ostream &stream, const SonarScan& scan){ 
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
  stream << scan.dataBytes;
  for(int i=0;i<scan.dataBytes;i++) 
  	stream << scan.scanData[i];	
  //stream.writeRawData((char*)scan.scanData,scan.dataBytes);
  return stream;
}


std::istream& operator>>(std::istream& stream, SonarScan& scan){
  stream >> scan.packedSize;
  stream >> scan.deviceType;
  stream >> scan.headStatus;
  stream >> scan.sweepCode;
  stream >> scan.headControl;
  stream >> scan.range;
  stream >> scan.txn;
  stream >> scan.gain;
  stream >> scan.slope;
  stream >> scan.adSpawn;
  stream >> scan.adLow;
  stream >> scan.headingOffset;
  stream >> scan.adInterval;
  stream >> scan.leftLimit;
  stream >> scan.rightLimit;
  stream >> scan.steps;
  stream >> scan.bearing;
  stream >> scan.dataBytes;
  scan.scanData = new uint8_t[scan.dataBytes];
  for(int i=0;i<scan.dataBytes;i++) 
  	stream >> scan.scanData[i];	
//  stream.readRawData((char*)scan.scanData,scan.dataBytes);
  return stream;
}


