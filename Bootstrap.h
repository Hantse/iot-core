/*
  Bootstrap.h
*/
#ifndef Bootstrap_h
#define Bootstrap_h

#include "Arduino.h"
#include "MqttService.h"
#include "CommandHandler.h"

class Bootstrap
{
  public:
    Bootstrap(char* ssidInput, char* passwordInput, char* hostInput, char* mqttServerInput);
    void setup();
    void startMainProcess();
    TaskHandle_t mainThread;
    MqttService* mqttService;
    void publishData(char* topic);
    void publishData(char* topic, char* dataInput);
    void publishData(char* topic, uint8_t* dataInput);
    void setHandler(CommandHandler* handler);
    String getDeviceName();
    char* getDeviceNameAsChar();
  private:
    void setupWifi();
    void generateHostname();
    static void singleProcess(void *pvParameters);
};

#endif
