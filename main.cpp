/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include <stdio.h>
#include "Micron.h"
#include <stdlib.h>


class Interface : public SeaNet::SonarHandler{
	public:
	Interface(){
	}
/*
	void processDepth(base::Time const& time, double depth){
		printf("Got Ground distance: %f\n",depth);
	}
*/
	void processSonarScan(SonarScan const *scan){
		
		const MicronScan *micron = dynamic_cast<const MicronScan*>(scan);
		printf("Got Scan with size: %i\n",micron->scanData.size());
		for(unsigned int i=0;i<micron->scanData.size();i++){
			printf("%hu ",micron->scanData[i]);
		}
		printf("\n");
		
	};
/*	
	virtual void processSonarScan(ProfilerScan const& scan){
		printf("Got Profiler Scan\n");
		for(unsigned int i=0;i<scan.scanData.size();i++){
			printf("%f ",scan.scanData[i]*10e-6*1500.0/2.0);
		}
		printf("\n");
	}
*/
};


int main(int argc, char* argv[]) {
  SeaNet::Micron::Driver si;
  Interface i;
  si.init(std::string(argv[1]));
 /* 
  if(argc == 4){
      llpc.setLEDs(atoi(argv[2]));
    if(argv[3][0] == '1'){
      llpc.setLaserOverride(true);
    }else{
      llpc.setLaserOverride(false);
    }
  }else{
  */
  si.registerHandler(&i);
  si.sendHeadData(); 
    si.requestData();
    while(1){
      try {
	si.processSerialData(1000);
        si.requestData();
      }catch(timeout_error t) {
	printf("Timeout\n");
      }
  }
}



