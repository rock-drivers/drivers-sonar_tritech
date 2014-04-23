#include <iostream>
#include "SeaNetMicron.hpp"
#include <iodrivers_base/Driver.hpp>

const int baudrate = 115200;
const int udpPort = 4000;

class UDPDriver : public iodrivers_base::Driver {
    public:
    UDPDriver() : Driver(1024){
    
    }
    bool init(std::string udpServer, int port){
        openUDP(udpServer,port);
        return isValid();
    }

    void sendSonarBeam(const base::samples::SonarBeam &beam){
        std::stringstream s;
        s << beam.time.toSeconds() << " " <<
            beam.bearing.rad << " " <<
            beam.getSpatialResolution() << " " <<
            beam.beam.size() << " ";
        for(size_t i = 0;i<beam.beam.size();i++){
            s << (unsigned int)beam.beam[i] << " ";
        }
        s << "\n";
        size_t len = s.str().size();
        if(!isValid()){
            std::cout << len << " " << s.str() << std::endl;
            std::cout << "Warning socket is no open (yet?)" << std::endl;
        }else{
            std::cout << len << " " << s.str() << std::endl;
            writePacket((const uint8_t*)s.str().c_str(),len);
        }
    }

    private:
    virtual int extractPacket(uint8_t const* buffer, size_t buffer_size) const{
        //We don't read data here
        return 0;
    }
};



int main(int argc, char** argv)
{
    sea_net::Micron driver;
    UDPDriver udp;
    if(argc != 3 && argc != 2){
        std::cerr << "Please call this application with the corresponding device given and the udp server to connect" << std::endl;
        std::cerr << "The remote server is optional" << std::endl;
        return -1;
    }

    sea_net::MicronConfig config;
    config.max_distance = 30;
    config.gain = 0.4;
    
    if(argc == 3){
        if(!udp.init(std::string(argv[2]),udpPort)){
            std::cerr << "Could not open UDP server" << std::endl;
        }
    }

    driver.openSerial(argv[1],baudrate);
    base::samples::RigidBodyState rbs;
    driver.configure(config,1000);
    driver.start();
    sea_net::PacketType packet_type = sea_net::mtNull;
    base::samples::SonarBeam sonar_beam;
    while(1){
        packet_type = driver.readPacket(1000);
        switch(packet_type){
        case sea_net::mtHeadData:
            {
                 driver.decodeSonarBeam(sonar_beam);
                 udp.sendSonarBeam(sonar_beam);
                 break;
            }
        default:
            break;
        }
    }
    return 0;
}
