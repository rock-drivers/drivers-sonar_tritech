#include <stdio.h>
#include "SonarInterface.h"
#include <stdlib.h>
 
int main(int argc, char* argv[]) {
  SonarInterface si;
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
    si.sendHeadData(); 
    while(1){
      try {
	si.processSerialData();
      }catch(timeout_error t) {
	printf("Timeout\n");
      }
  }
}
