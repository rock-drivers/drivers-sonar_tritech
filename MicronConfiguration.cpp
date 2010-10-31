/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include "MicronConfiguration.h"

MicronConfiguration::MicronConfiguration(){
    rangeScale=0;
    leftLimit=0;
    rightLimit=0;
    adSpan=0;
    adLow=0;
    initialGain=0;
    motorStepDelayTime=0;
    motorStepAngleSize=0;
    adInterval=0;
    numberOfBins=0;
    maxADBuff=0;
    adcSetpointCh=0;
    boolValues=0;
}

MicronConfiguration::~MicronConfiguration(){

}

MicronConfiguration* MicronConfiguration::clone()
{
	return new MicronConfiguration(*this);
}


MicronConfiguration::MicronConfiguration(const MicronConfiguration &copy)
{
    rangeScale=copy.rangeScale;
    leftLimit=copy.leftLimit;
    rightLimit=copy.rightLimit;
    adSpan=copy.adSpan;
    adLow=copy.adLow;
    initialGain=copy.initialGain;
    motorStepDelayTime=copy.motorStepDelayTime;
    motorStepAngleSize=copy.motorStepAngleSize;
    adInterval=copy.adInterval;
    numberOfBins=copy.numberOfBins;
    maxADBuff=copy.maxADBuff;
    adcSetpointCh=copy.adcSetpointCh;
    boolValues=copy.boolValues;
}

bool MicronConfiguration::getadc8on(){
  return boolValues&adc8on;
}

bool MicronConfiguration::getcont(){
  return boolValues&cont;
}

bool MicronConfiguration::getscanright(){
  return boolValues&scanright;
}

bool MicronConfiguration::getinvert(){
  return boolValues&invert;
}

bool MicronConfiguration::getchan2(){
  return boolValues&chan2;
}

bool MicronConfiguration::getapplyoffset(){
  return boolValues&applyoffset;
}

bool MicronConfiguration::getpingpong(){
  return boolValues&pingpong;
}

uint16_t MicronConfiguration::getrangeScale(){
  return rangeScale;
}


uint16_t MicronConfiguration::getleftLimit(){
  return leftLimit;
}

uint16_t MicronConfiguration::getrightLimit(){
  return rightLimit;
}

uint8_t MicronConfiguration::getadSpan(){
  return adSpan;
}

uint8_t MicronConfiguration::getadLow(){
  return adLow;
}


uint8_t MicronConfiguration::getinitialGain(){
  return initialGain;
}

uint8_t MicronConfiguration::getmotorStepDelayTime(){
  return motorStepDelayTime;
}

uint8_t MicronConfiguration::getmotorStepAngleSize(){
  return motorStepAngleSize;
}

uint16_t MicronConfiguration::getadInterval(){
  return adInterval;
}

uint16_t MicronConfiguration::getnumberOfBins(){
  return numberOfBins;
}

uint16_t MicronConfiguration::getmaxADBuff(){
  return maxADBuff;
}

uint8_t MicronConfiguration::getadcSetpointCh(){
  return adcSetpointCh;
}

void MicronConfiguration::setadc8on(bool b){
  if(b)
    boolValues |= adc8on;
  else
    boolValues &= ~adc8on;
}

void MicronConfiguration::setcont(bool b){
  if(b)
    boolValues |= cont;
  else
    boolValues &= ~cont;
}

void MicronConfiguration::setscanright(bool b){
  if(b)
    boolValues |= scanright;
  else
    boolValues &= ~scanright;
}

void MicronConfiguration::setinvert(bool b){
  if(b)
    boolValues |= invert;
  else
    boolValues &= ~invert;
}

void MicronConfiguration::setchan2(bool b){
  if(b)
    boolValues |= chan2;
  else
    boolValues &= ~chan2;
}

void MicronConfiguration::setapplyoffset(bool b){
  if(b)
    boolValues |= applyoffset;
  else
    boolValues &= ~applyoffset;
}

void MicronConfiguration::setpingpong(bool b){
  if(b)
    boolValues |= pingpong;
  else
    boolValues &= ~pingpong;
}

void MicronConfiguration::setrangeScale(uint16_t v){
  rangeScale = v;
}

void MicronConfiguration::setleftLimit(uint16_t v){
   leftLimit = v;
}

void MicronConfiguration::setrightLimit(uint16_t v){
  rightLimit = v;
}

void MicronConfiguration::setadSpan(uint8_t v){
  adSpan = v;
}

void MicronConfiguration::setadLow(uint8_t v){
   adLow = v;
}

void MicronConfiguration::setinitialGain(uint8_t v){
  initialGain = v;
}

void MicronConfiguration::setmotorStepDelayTime(uint8_t v){
  motorStepDelayTime = v;
}

void MicronConfiguration::setmotorStepAngleSize(uint8_t v){
  motorStepAngleSize = v;
}

void MicronConfiguration::setadInterval(uint16_t v){
  adInterval = v;
}

void MicronConfiguration::setnumberOfBins(uint16_t v){
  numberOfBins= v;
}

void MicronConfiguration::setmaxADBuff(uint16_t v){
  maxADBuff = v;
}

void MicronConfiguration::setadcSetpointCh(uint8_t v){
  adcSetpointCh = v;
}
