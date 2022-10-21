/*
  Bootstrap.h
*/
#ifndef Bootstrap_h
#define Bootstrap_h

#include "Arduino.h"
#include "MqttService.h"
#include "LocalServer.h"
#include "CommandHandler.h"

class Bootstrap
{
  public:
    Bootstrap();
    Bootstrap(char* hostInput, char* mqttServerInput);
    Bootstrap(char* ssidInput, char* passwordInput, char* hostInput, char* mqttServerInput);
    void setConfiguration(char* ssidInput, char* passwordInput, char* hostInput, char* mqttServerInput);
    void setup();
    void startMainProcess();
    void publishData(char* topic);
    void publishData(char* topic, char* dataInput);
    void publishData(char* topic, uint8_t* dataInput);
    void setHandler(CommandHandler* handler);
    String getDeviceName();
    char* getDeviceNameAsChar();
    TaskHandle_t mainThread;
    MqttService* mqttService;
    LocalServer localServer;

  private:
    void setupWifi();
    void generateHostname();
    static void singleProcess(void *pvParameters);
};

#endif
