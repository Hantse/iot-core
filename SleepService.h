/*
  OtaService.h
*/
#ifndef SleepService_h
#define SleepService_h

#include "Arduino.h"

class SleepService
{
public:
    SleepService(int timeToSleep);
   	void startSleep();
	void printWakeupReason();
    void setCanSleep(bool sleepStatus);
private:
};

#endif
