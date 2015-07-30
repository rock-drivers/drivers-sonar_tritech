#include <boost/test/unit_test.hpp>

#include "../src/SeaNetMicron.hpp"


#include <iodrivers_base/Exceptions.hpp>
#include <iostream>

//test micron driver 
BOOST_AUTO_TEST_CASE(micron)
{
    sea_net::Micron micron;

    //check opening of a wrong port
    BOOST_CHECK_THROW(micron.openSerial("/dev/ttyUSB1", 115200),std::runtime_error);

    //check opening 
    micron.openSerial("/dev/ttyUSB0",115200);

    //check duplex
    std::cout << "full_dublex " << micron.isFullDublex(2000) << std::endl;

    //check reconfigure with a too small timeout 
    sea_net::MicronConfig conf;
    base::samples::SonarBeam sonar_beam;
    conf.max_distance = 4;
    
    conf.angular_resolution = base::Angle::fromDeg(1.0);
    micron.configure(conf,10000);
    std::cout <<" TEST 360° Degree Modus with 1° Steps" << std::endl;
    double start_angle = -99;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;;
        if(start_angle == -99)
            start_angle = sonar_beam.bearing.rad;
    }
    std::cout << std::endl;

    conf.angular_resolution = base::Angle::fromDeg(5.0);
    micron.configure(conf,10000);
    std::cout <<" TEST 360° Degree Modus with 5° Steps" << std::endl;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;
    }
    std::cout << std::endl;
    
    conf.angular_resolution = base::Angle::fromDeg(10.0);
    micron.configure(conf,10000);
    std::cout <<" TEST 360° Degree Modus with 10° Steps" << std::endl;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;;
    }
    std::cout << std::endl;


    conf.angular_resolution = base::Angle::fromDeg(5.0);
    conf.left_limit = base::Angle::fromDeg(-15.0);
    conf.right_limit = base::Angle::fromDeg(15.0);
    conf.continous = false;
    micron.configure(conf,10000);
    micron.waitForPacket(sea_net::mtHeadData,10000);
    std::cout <<" TEST LeftRight +-15° Modus with 5° Steps" << std::endl;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;;
    }
    std::cout << std::endl;

    conf.angular_resolution = base::Angle::fromDeg(5.0);
    conf.left_limit = base::Angle::fromDeg(15.0);
    conf.right_limit = base::Angle::fromDeg(30.0);
    micron.configure(conf,10000);
    micron.waitForPacket(sea_net::mtHeadData,1000);
    std::cout <<" TEST LeftRight +15 +30° Modus with 5° Steps" << std::endl;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;;
    }
    std::cout << std::endl;

    conf.angular_resolution = base::Angle::fromDeg(5.0);
    conf.left_limit = base::Angle::fromDeg(-15.0);
    conf.right_limit = base::Angle::fromDeg(-30.0);
    micron.configure(conf,10000);
    micron.waitForPacket(sea_net::mtHeadData,1000);
    std::cout <<" TEST LeftRight -15 -30° Modus with 5° Steps" << std::endl;
    for(int i=0;i<10;++i)
    {
        micron.requestData();
        micron.receiveData(1000);
        micron.decodeSonarBeam(sonar_beam);
        std::cout << "bearing[deg]: " << sonar_beam.bearing.rad /M_PI*180 << std::endl;;
    }
    std::cout << std::endl;
}
