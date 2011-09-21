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


/**
 * This Class handles the complete Configuration for the Tritech MicronDST Scanning Sonar
 **/
class MicronConfiguration
{
  /**
   * Binary Values
   * adc8on = Use eight bit (default) instead of 4 bits for echo-strength representation
   * cont = scan continues, independed of left and right limit (whole 360deg scan)
   * scanright = scan in the inverse direction
   * invert = invert angle
   * chan2 = use different frequence (not used (lt. doc))
   * applyoffset = if set to False, offset value is not used
   * pingpong = do PingPong Scan between left<->right limit (only applicable if cont = false)
   */
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

    /**
     * Range Scale Factor, not used by the Sonar, only passtru in headData
     */
    void setrangeScale(uint16_t);

    /**
     * Left limit between 0->6399 == 0° -> 360°
     */
    void setleftLimit(uint16_t);
    
    /**
     * Right limit between 0->6399 == 0° -> 360°
     */
    void setrightLimit(uint16_t);

    /**
     * Not used by the Sonar directly, see Documentation for Vendor Application
     */
    void setadSpan(uint8_t);
    
    /**
     * Not used by the Sonar directly, see Documentation for Vendor Application
     */
    void setadLow(uint8_t);
    
    /**
     * Initial Gain Factor applyed by the Sonar to the recived Echo
     * Value means percentage gain level in percent
     * 84 menas 84%
     */
    void setinitialGain(uint8_t);

    /**
     * Maximum motor Speed leve, typiccal value of 25 
     * in reality not change is detected
     */
    void setmotorStepDelayTime(uint8_t);

    /**
     * Motor Step angle Size between the pings in Gradian
     * Typical Values (but still every else value possible)
     * 32 = 2 Gradian == 1.8°
     * 16 = 1 Gradian == 0.9°
     * 8 = 0.5 Gradian == 0.45°
     * 4 = 0.25 Grdian == 0.225°
     */
    void setmotorStepAngleSize(uint8_t);

    /**
     * The Time between 2 bins in one Ping
     * the result isvalue * 640 nanoseconds.
     * It's coupled with the ad interval (see formular below)
     */
    void setadInterval(uint16_t);

    /**
     * The numbers of bins in on result.
     * This values sets together with the adInterval the resulting range of the sonar
     */
    void setnumberOfBins(uint16_t);

    /**
     * System setting sould be keeped at 500 (limit 1000)
     */
    void setmaxADBuff(uint16_t);

    /**
     * Undocumented
     */
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
