/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#ifndef MICRONCONFIGURATION_H
#define MICRONCONFIGURATION_H

#include <inttypes.h>

class MicronConfiguration
{
  enum boolValueList{
    adc8on=1,
    cont=2,
    scanright=4,
    invert=8,
    chan2=16,
    applyoffset=32,
    pingpong=64
  };

public:
    MicronConfiguration();
    ~MicronConfiguration();
    // copy con
    MicronConfiguration(const MicronConfiguration &copy);
    // clone
    MicronConfiguration* clone();

    bool getadc8on();
    bool getcont();
    bool getscanright();
    bool getinvert();
    bool getchan2();
    bool getapplyoffset();
    bool getpingpong();
    uint16_t getrangeScale();
    uint16_t getleftLimit();
    uint16_t getrightLimit();
    uint8_t getadSpan();
    uint8_t getadLow();
    uint8_t getinitialGain();
    uint8_t getmotorStepDelayTime();
    uint8_t getmotorStepAngleSize();
    uint16_t getadInterval();
    uint16_t getnumberOfBins();
    uint16_t getmaxADBuff();
    uint8_t getadcSetpointCh();

    void setadc8on(bool);
    void setcont(bool);
    void setscanright(bool);
    void setinvert(bool);
    void setchan2(bool);
    void setapplyoffset(bool);
    void setpingpong(bool);
    void setrangeScale(uint16_t);
    void setleftLimit(uint16_t);
    void setrightLimit(uint16_t);
    void setadSpan(uint8_t);
    void setadLow(uint8_t);
    void setinitialGain(uint8_t);
    void setmotorStepDelayTime(uint8_t);
    void setmotorStepAngleSize(uint8_t);
    void setadInterval(uint16_t);
    void setnumberOfBins(uint16_t);
    void setmaxADBuff(uint16_t);
    void setadcSetpointCh(uint8_t);

private:
    uint16_t rangeScale;
    uint16_t leftLimit;
    uint16_t rightLimit;
    uint8_t adSpan;
    uint8_t adLow;
    uint8_t initialGain;
    uint8_t motorStepDelayTime;
    uint8_t motorStepAngleSize;
    uint16_t adInterval;
    uint16_t numberOfBins;
    uint16_t maxADBuff;
    uint8_t adcSetpointCh;

    uint16_t boolValues;
};

#endif // MICRONCONFIGURATION_H
