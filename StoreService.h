/*
  OtaService.h
*/
#ifndef StoreService_h
#define StoreService_h

#include "Arduino.h"

class StoreService
{
public:
    StoreService();
    void setup();
    bool isConfigure();
    void storeConfiguration(String wifiSsid, String wifiPassword);
    char* getWifiSsid();
    char* getWifiPassword();
};

#endif
