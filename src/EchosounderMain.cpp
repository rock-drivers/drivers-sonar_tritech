#include <iostream>
#include "SeaNetEchosounder.hpp"
int main(int argc, char** argv)
{
    sea_net::Echosounder e;
    if(argc!= 2){
        std::cerr << "Please call this application with the corresponding device given" << std::endl;
        return -1;
    }
    e.openSerial(argv[1]);
    base::samples::RigidBodyState rbs;
    while(1){
        if(e.getDistance(rbs,1000)){
            std::cout << "Ground distance is: " << rbs.position[2] << std::endl;
        }

    }
    return 0;
}
