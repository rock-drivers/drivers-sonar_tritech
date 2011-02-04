/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include <stdio.h>
#include "Profiling.h"
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
	void processSonarScan(SonarScan const *s){
		const ProfilerScan *scan = dynamic_cast<const ProfilerScan*>(s);
		for(unsigned int i=0;i<scan->scanData.size();i++){
			printf("%f ",scan->scanData[i]*10e-6*1500.0/2.0);
		}
		printf("\n");
	};
	
};


int main(int argc, char* argv[]) {
  SeaNet::Profiling::Driver si;
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
  si.sendHeadData(si.getDefaultHeadData()); 
    si.requestData();
    while(1){
      try {
	si.processSerialData(2000);
        si.requestData();
      }catch(timeout_error t) {
	printf("Timeout\n");
      }
  }
}



