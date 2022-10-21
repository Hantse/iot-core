/*
  CommandHandler.h
*/
#ifndef CommandHandler_h
#define CommandHandler_h

#include "Arduino.h"

class CommandHandler
{
public:
	virtual ~CommandHandler() {}
	virtual void handleCommand(char *topic, byte *message, unsigned int length);
};

#endif
