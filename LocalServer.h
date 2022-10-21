/*
  LocalServer.h
*/
#ifndef LocalServer_h
#define LocalServer_h

#include "Arduino.h"

class LocalServer
{
public:
  LocalServer();
  LocalServer(char* ssidInput, char* passwordInput);
  void startServer();

private:
  
};

#endif
