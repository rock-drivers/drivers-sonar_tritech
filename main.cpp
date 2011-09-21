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
	
        void processSonarScan(SonarScan const *scan){
		
		const MicronScan *micron = dynamic_cast<const MicronScan*>(scan);
		printf("Got Scan with size: %i\n",micron->scanData.size());
		for(unsigned int i=0;i<micron->scanData.size();i++){
			printf("%hu ",micron->scanData[i]);
		}
		printf("\n");
		
	};
};


int main(int argc, char* argv[]) {
  SeaNet::Micron::Driver si(true);
  Interface i;
  si.init(std::string(argv[1]));

  printf("Sub PTS Name is: %s\n",si.getSlavePTS());
  
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



