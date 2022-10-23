/*
  OtaService.h
*/
#ifndef OtaService_h
#define OtaService_h

#include "Arduino.h"
#include <PubSubClient.h>
#include "SleepService.h"

class OtaService
{
  public: 
    OtaService();
    void update(String url, int port, char* deviceId, PubSubClient *mqttClient);
    void injectService(SleepService *sleepServiceInjected);
};

#endif
