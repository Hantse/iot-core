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
    void clearStorage();
    String getWifiSsid();
    String getWifiPassword();
};

#endif
