/**
 * Author: Matthias Goldhoorn (matthias.goldhoorn@dfki.de)
 * Company: Deutsches Forschungszentrum für Künstliche Intelligenz - Robotics Innovation Center (DFKI RIC)
 * Year 2010
 * Desc:
 *
*/
#include <stdio.h>
#include "SonarInterface.h"
#include <stdlib.h>

class Interface: public SonarHandler{
	public:
	Interface(){
	}

	void processDepth(base::Time const& time, double depth){
		printf("Got Ground distance: %f\n",depth);
	}

	void processSonarScan(SonarScan const& scan){
		printf("Got Scan\n");
		for(unsigned int i=0;i<scan.scanData.size();i++){
			printf("%hu ",scan.scanData[i]);
		}
		printf("\n");
	};
	
	virtual void processSonarScan(ProfilerScan const& scan){
		printf("Got Profiler Scan\n");
		for(unsigned int i=0;i<scan.scanData.size();i++){
			printf("%f ",scan.scanData[i]*10e-6*1500.0/2.0);
		}
		printf("\n");
	}
};


int main(int argc, char* argv[]) {
  SonarInterface si;
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
  //si.sendHeadData(); 
  si.sendHeadDataProfiling();
  si.sendHeadDataProfiling();
    //si.requestData();
    while(1){
      try {
	si.processSerialData(1000);
        si.requestData();
      }catch(timeout_error t) {
	printf("Timeout\n");
      }
  }
}



