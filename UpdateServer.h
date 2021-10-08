/*
  UpdateServer.h
*/
#ifndef UpdateServer_h
#define UpdateServer_h

#include "Arduino.h"

class UpdateServer
{
  public: 
    UpdateServer(char* hostInput);
    void setup();
    void handleServerUpdate();
};

#endif
