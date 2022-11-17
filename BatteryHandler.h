/*
  BatteryHandler.h
*/
#ifndef BatteryHandler_h
#define BatteryHandler_h

#include "Arduino.h"

class BatteryHandler
{
public:
	virtual ~BatteryHandler() {}
	virtual char* handleCommand();
};

#endif