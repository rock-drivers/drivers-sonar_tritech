#ifndef _PROFILING_H_
#define _PROFILING_H_ 

#include "SeaNet.h"

namespace sea_net{

class Profiling: public SeaNet
{
public:
	Driver(bool usePTS=false);
	~Driver();
	static headControl getDefaultHeadData();
	void sendHeadData(headControl hc);
	void processHeadData(u8 *message);
}; };

#endif
