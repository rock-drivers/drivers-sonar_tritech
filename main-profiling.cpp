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
  si.registerHandler(&i);
  si.sendHeadData(si.getDefaultHeadData()); 
    si.requestData();
    while(1){
      try {
	si.processSerialData(2000);
        si.requestData();
      }catch(iodrivers_base::TimeoutError t) {
	printf("Timeout\n");
      }
  }
}



