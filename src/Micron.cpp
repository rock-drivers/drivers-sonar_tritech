#include "Micron.hpp"
#include "SeaNetTypesInternal.hpp"
#include <iodrivers_base/Timeout.hpp>

using namespace sea_net;

enum HeadConfigFlags
{
    ADC8ON = 1, CONT = 2, SCANRIGHT = 4, INVERT = 8,
    MOTOFF = 16, TXOFF = 32, SPARE = 64, CHAN2 = 128,
    RAW = 256, HASMOT = 512, APPLYOFFSET = 1024, PINGPONG = 2048,
    STARELLIM = 4096, REPLYASL = 8192, REPLYTHR = 16384,
    IGNORESENSOR = 32768
};

Micron::Micron(): SeaNet(IMAGINGSONAR)
{ 
    head_config = new HeadConfigPacket;   //forward declaration 
}

Micron::~Micron() 
{
    delete head_config;
}

void Micron::getSonarBeam(base::samples::SonarBeam &sonar_beam)
{
    SeaNetPacket &sea_net_packet = *getSeaNetPacket();

    if(!sea_net_packet.isValid())
        throw std::runtime_error("Micron::getSonarBeam: Message is not valid!");

    if(!sea_net_packet.getPacketType() != mtHeadData)
        throw std::runtime_error("Micron::getSonarBeam: Wrong message type!");

    if(!sea_net_packet.getDeviceType() != IMAGINGSONAR)
        throw std::runtime_error("Micron::getSonarBeam: Wrong device type");

    const uint8_t *packet;
    size_t size;
    sea_net_packet.getRawData(packet,size);

    ///////////////////////////////////
    //not all bytes are read !!!
    ///////////////////////////////////
    //uint8_t nodeType = packet[12];
    //uint8_t type = packet[10];
    //uint16_t packedSize    = packet[13] | (packet[14]<<8);
    //uint8_t deviceType     = packet[15];
    //uint8_t headStatus     = packet[16];
    //uint8_t sweepCode      = packet[17];
    uint16_t headControl   = packet[18] | (packet[19]<<8);
    //uint16_t range         = packet[20] | (packet[21]<<8);
    //uint32_t txn           = packet[22] | (packet[23]<<8) | (packet[24]<<16) | (packet[25]<<24);
    //uint8_t gain           = packet[26];
    //uint16_t slope         = packet[27] | (packet[28]<<8);
    //uint8_t adSpawn        = packet[29];
    //uint8_t adLow          = packet[30];
    //uint16_t headingOffset = packet[31] | (packet[32]<<8);
    uint16_t adInterval    = packet[33] | (packet[34]<<8);
    uint16_t leftLimit     = packet[35] | (packet[36]<<8);
    uint16_t rightLimit    = packet[37] | (packet[38]<<8);
    //uint8_t steps          = packet[39];
    uint16_t bearing       = packet[40] | (packet[41]<<8);
    uint16_t dataBytes     = packet[42] | (packet[43]<<8);
    const uint8_t *scanData      = &packet[44];

    //TODO check state of the head 

    //check if the configuration is ok
    if(head_config->adInterval != adInterval ||
       head_config->leftLimit != leftLimit ||
       head_config->rightLimit != rightLimit ||
       head_config->numberOfBins != dataBytes || 
       head_config->headControl != headControl)
        throw std::runtime_error("Configuration of the beam differs from the desired one.");

    //copy data into SonarBeam
    sonar_beam.time = base::Time::now();
    sonar_beam.sampling_interval  = ((adInterval*640.0)*1e-9);
    sonar_beam.bearing     = base::Angle::fromRad(M_PI-(bearing/6399.0*2.0*M_PI));
    sonar_beam.speed_of_sound = 1500;
    sonar_beam.beamwidth_vertical = 35.0/180.0*M_PI;
    sonar_beam.beamwidth_horizontal = 3.0/180.0*M_PI;

    //TODO check if we are using low resolution 
    sonar_beam.beam.resize(dataBytes);
    memcpy(&sonar_beam.beam[0], scanData, dataBytes);
}

void Micron::configure(const MicronConfig &config,uint32_t timeout)
{
    //some conversions
    int adInterval = (((config.resolution/config.speed_of_sound)*2.0)*1e9)/640.0;
    int numberOfBins = config.max_distance/config.resolution;
    uint16_t leftLimit = ((config.left_limit.rad+M_PI)/(M_PI*2.0))*6399.0;
    uint16_t rightLimit = ((config.right_limit.rad+M_PI)/(M_PI*2.0))*6399.0;
    uint8_t motorStepAngleSize = (config.angular_resolution.rad/(M_PI*2.0))*6399.0;
    uint8_t initialGain = config.gain*210;
    uint16_t lockout_time = config.min_distance*2.0/config.speed_of_sound*1e6;  //in microseconds 

    //check configuration 
    if(config.gain < 0.0 || config.gain > 1.0 ||
       config.angular_resolution.rad < 0 || config.angular_resolution.rad / (0.05625/180.0*M_PI) > 255.0 ||
       adInterval < 0 || adInterval > 1500 || numberOfBins < 0 || numberOfBins > 1500 || config.min_distance < 0 ||
       config.min_distance > 98.0 )
    {
        throw std::runtime_error("Micron::Micron: invalid configuration.");
    }

    //values which cannot configured so far or are ignored by the microns dst 
    bool motorOff = false;
    bool txOff= false;
    bool spare = false;
    bool raw = true;
    bool hasmot = true;
    bool starellim = false;
    bool replyasl = true;
    bool replyThr = false;
    bool ignore_sensor = false;
    bool ping_pong = false;     //this is usually used for side scan sonars
    bool apply_offset = false;
    bool invert = false;
    uint8_t adSpan = 81;
    uint8_t adLow = 8;
    uint16_t adcSetpoint = 0;
    uint16_t rangeScale = 30;
    uint8_t motorStepDelayTime = 25;
    bool chan2 = true;

    //generate head data
    head_config->V3BParams = 0x1D;
    head_config->headType = 0x02;
    head_config->txnCh1 = 77846282;
    head_config->txnCh2 = 162403450;
    head_config->rxnCh1 = 138915348;
    head_config->rxnCh2 = 223472517;
    head_config->pulseLength=30;
    head_config->rangeScale  = rangeScale;
    head_config->leftLimit   = leftLimit; 
    head_config->rightLimit  = rightLimit;
    head_config->adSpan = adSpan;
    head_config->adLow = adLow;
    head_config->initialGainCh1 = initialGain;
    head_config->initialGainCh2 = initialGain;
    head_config->slopeCh1 = 0; //Ignored
    head_config->slopeCh2 = 0; //Ignored
    head_config->motorStepDelayTime = motorStepDelayTime;
    head_config->motorStepAngleSize = motorStepAngleSize;
    head_config->adInterval = adInterval;
    head_config->numberOfBins = numberOfBins;
    head_config->maxADBuff = (0xE8) | (3<<8);
    head_config->lockoutTime = lockout_time;
    head_config->minorAxisDir = (0x40) | (0x06<<8);
    head_config->majorAxisPan = (0x01);
    head_config->crtl2 = 0;
    head_config->scanZ = 0;

    head_config->adSpanCh1 = adSpan;
    head_config->adSpanCh2 = adSpan;
    head_config->adLowCh1 = adLow;
    head_config->adLowCh2 = adLow;
    head_config->igainCh1 = initialGain;
    head_config->igainCh2 = initialGain;
    head_config->adcSetPointCh1 = adcSetpoint;
    head_config->adcSetPointCh2 = adcSetpoint;
    head_config->slopeCH1 =0; //Ignored
    head_config->slopeCH2 =0; //Ignored
    head_config->slopeDelayCh1 =0; //Ignored
    head_config->slopeDelayCh2 =0; //Ignored

    head_config->headControl =
        (config.low_resolution?ADC8ON:0) | (config.continous?CONT:0) |
        (config.scan_right?SCANRIGHT:0) | (invert?INVERT:0) |
        (motorOff?MOTOFF:0) | (txOff?TXOFF:0) |
        (spare?SPARE:0) | (chan2?CHAN2:0) |
        (raw?RAW:0) | (hasmot?HASMOT:0) |
        (apply_offset?APPLYOFFSET:0) | (ping_pong?PINGPONG:0) |
        (starellim?STARELLIM:0) | (replyasl?REPLYASL:0) |
        (replyThr?REPLYTHR:0) | (ignore_sensor?IGNORESENSOR:0);

    iodrivers_base::Timeout time_out(timeout);
    std::vector<uint8_t> packet = SeaNetPacket::createPaket(IMAGINGSONAR,mtHeadCommand,
                                                            (uint8_t*)head_config,sizeof(*head_config));
    writePacket(&packet[0],packet.size(),time_out.timeLeft());

    //clear all old packets
    clear();
    writeSendData(time_out.timeLeft());

    //now wait until we get a package with a proper configuration
    int counter = 0;
    while(true)
    {
        if(mtHeadData == readPacket(time_out.timeLeft()))
        {
            try
            {
                base::samples::SonarBeam sonar_beam;
                getSonarBeam(sonar_beam);
            }
            catch(std::runtime_error e)
            {
                counter++;
                //two wrong package are allowed 
                if(counter>2)
                    throw std::runtime_error("Micron::configuration: The Microns DST is not accepting the given configuration!");
            }
        }
    }
}


