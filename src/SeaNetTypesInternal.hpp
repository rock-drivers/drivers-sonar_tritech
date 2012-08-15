#ifndef _SEANET_TYPES_INTERNAL_H_
#define _SEANET_TYPES_INTERNAL_H_

#include "SeaNetTypes.hpp"
#define SEA_NET_MAX_PACKET_SIZE 1545 // mtHeadData is the biggest package 

namespace sea_net
{

    static const char PACKET_START = '@';
    static const char PACKET_END = 0x0A;
    static const int WRITE_TIMEOUT = 500;  //im ms

    struct HeadCommand
    {
        uint8_t V3B_params; uint16_t head_control; uint8_t head_type;
        uint32_t txn_ch1; uint32_t txn_ch2; uint32_t rxn_ch1; uint32_t rxn_ch2;
        uint16_t pulse_length; uint16_t range_scale; uint16_t left_limit;
        uint16_t right_limit; uint8_t ad_span; uint8_t ad_low; uint8_t initial_gain_ch1;
        uint8_t initial_gain_ch2; uint16_t slope_ch1; uint16_t slope_ch2; uint8_t motor_step_delay_time;
        uint8_t motor_step_angle_size; uint16_t ad_interval; uint16_t number_of_bins;
        uint16_t max_ad_buff; uint16_t lockout_time; uint16_t minor_axis_dir;
        uint8_t major_axis_pan; uint8_t crtl2; uint16_t scan_z; uint8_t ad_span_ch1;
        uint8_t ad_span_ch2; uint8_t ad_low_ch1; uint8_t ad_low_ch2; uint8_t igain_ch1;
        uint8_t igain_ch2; uint8_t adc_set_point_ch1; uint8_t adc_set_point_ch2;
        uint16_t advanced_slope_ch1; uint16_t advance_slope_ch2; uint16_t advanced_slope_delay_ch1;
        uint16_t advanced_slope_delay_ch2;

        HeadCommand()
        {
            //clear everything 
            memset(this,0,sizeof(*this));
        }
    } __attribute__ ((packed)) __attribute__((__may_alias__));

    struct AliveData
    {
       bool ready;
       bool motor_on;
       bool scanning;
       bool no_config;
       bool config_send;
       AliveData():
           ready(false),motor_on(false),scanning(false),
           no_config(true),config_send(false){};
    };

    struct HeadData
    {
        uint8_t node_type; 
        uint8_t type;
        uint16_t packed_size;
        uint8_t device_type;
        uint8_t head_status;
        uint8_t sweep_code;
        uint16_t head_control;
        uint16_t range;
        uint32_t txn;
        uint8_t gain;
        uint16_t slope;
        uint8_t ad_span;
        uint8_t ad_low;
        uint16_t heading_offset;
        uint16_t ad_interval;
        uint16_t left_limit;
        uint16_t right_limit;
        uint8_t motor_step_angle_size;
        uint16_t bearing;
        uint16_t data_bytes;
        const uint8_t *scan_data;
    };

    struct BBUserData
    {
        bool full_dublex;
        BBUserData():
            full_dublex(0){};
    };

    class SeaNetPacket
    {
        public:
            static int isValidPacket(uint8_t const *buffer,size_t size);
            static std::vector<uint8_t> createPaket(DeviceType device_type,
                                                    PacketType packet_type, 
                                                    uint8_t* payload = NULL,
                                                    size_t payload_size = 0);

            static std::vector<uint8_t> createSendDataPaket(DeviceType device_type);

        public:
            void setSize(size_t size);
            void invalidate();
            bool validate();
            uint8_t *getPacketPtr();

            bool isValid() const;
            PacketType getPacketType() const;
            DeviceType getReceiverType() const;
            DeviceType getSenderType() const;

            void getRawData(const uint8_t * &buffer,size_t &size)const;

            void decodeAliveData(AliveData &data);
            void decodeHeadData(HeadData &data);
            void decodeAuxData(std::vector<uint8_t> &aux_data);
            void decodeVersionData(VersionData &version);
            void decodeBBUserData(BBUserData &data);

        private:
        //we do not use a std::vector because this would introduce
        //dynamic memory allocation if different packet types are stored
            uint8_t packet[SEA_NET_MAX_PACKET_SIZE];
            size_t size;
            bool valid;
    };

}
#endif 
