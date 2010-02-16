#include <stdio.h>
#include "SonarInterface.h"
#include <stdlib.h>

class Interface: public SonarHandler{
	public:
	Interface(){
	}

	void processDepth(const double depth){
		printf("Got Ground distance: %f\n",depth);
	}

	void processSonarScan(SonarScan*){
		printf("Got Scan\n");
	};
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
  si.sendHeadData(); 
    while(1){
      try {
	si.processSerialData();
      }catch(timeout_error t) {
	printf("Timeout\n");
      }
  }
}



