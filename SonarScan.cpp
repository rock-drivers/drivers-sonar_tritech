#include "SonarScan.h"
#include <string.h>

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


QDataStream& operator<<(QDataStream& stream,const SonarScan& scan){ 
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
  //fprintf(stdout,"Written %u data bytes!\n",scan.dataBytes);
  stream.writeRawData((char*)scan.scanData,scan.dataBytes);
  
  
  /*
  stream.writeRawData(&scan.packedSize,2);
  stream.writeRawData(&scan.deviceType,1);
  stream.writeRawData(&scan.headStatus,1);
  stream.writeRawData(&scan.sweepCode,1);
  stream.writeRawData(&scan.headControl,2);
  stream.writeRawData(&scan.range,2);
  stream.writeRawData(&scan.txn,4);
  stream.writeRawData(&scan.gain,1);
  stream.writeRawData(&scan.slope,2);
  stream.writeRawData(&scan.adSpawn,1);
  stream.writeRawData(&scan.adLow,1);
  stream.writeRawData(&scan.headingOffset,2);
  stream.writeRawData(&scan.adInterval,2);
  stream.writeRawData(&scan.leftLimit,2);
  stream.writeRawData(&scan.rightLimit,2);
  stream.writeRawData(&scan.steps,1);
  stream.writeRawData(&scan.bearing,2);
  stream.writeRawData(&scan.dataBytes,packedSize-31);
*/
  return stream;
}


QDataStream& operator>>(QDataStream& stream, SonarScan& scan){
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
  //fprintf(stdout,"Readed %u data bytes!\n",scan.dataBytes);
  scan.scanData = new uint8_t[scan.dataBytes];
  stream.readRawData((char*)scan.scanData,scan.dataBytes);
    
/*uint16_t packedSize;
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
  uint16_t *dataBytes
*/
/*
  stream.readRawData(scan.packedSize,2);
  stream.readRawData(scan.deviceType,1);
  stream.readRawData(scan.headStatus,1);
  stream.readRawData(scan.sweepCode,1);
  stream.readRawData(scan.headControl,2);
  stream.readRawData(scan.range,2);
  stream.readRawData(scan.txn,4);
  stream.readRawData(scan.gain,1);
  stream.readRawData(scan.slope,2);
  stream.readRawData(scan.adSpawn,1);
  stream.readRawData(scan.adLow,1);
  stream.readRawData(scan.headingOffset,2);
  stream.readRawData(scan.adInterval,2);
  stream.readRawData(scan.leftLimit,2);
  stream.readRawData(scan.rightLimit,2);
  stream.readRawData(scan.steps,1);
  stream.readRawData(scan.bearing,2);
  stream.readRawData(scan.dataBytes,packedSize-31);
*/
  return stream;
}


