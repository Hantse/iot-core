/*
  ESPNowHandler.h
*/
#ifndef ESPNowHandler_h
#define ESPNowHandler_h

#include "Arduino.h"
#include "MqttService.h"
#include <esp_now.h>

class ESPNowHandler
{
public:
	ESPNowHandler();

    void injectService(MqttService *mqttServiceInjected);
    void setup();

private:
    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

};

#endif