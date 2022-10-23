/*
  Bootstrap.h
*/
#ifndef Bootstrap_h
#define Bootstrap_h

#include "Arduino.h"
#include "MqttService.h"
#include "LocalServer.h"
#include "CommandHandler.h"
#include "StoreService.h"
#include "SleepService.h"

class Bootstrap
{
public:
	Bootstrap();
	Bootstrap(char *hostInput, char *mqttServerInput);
	Bootstrap(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput);
	void setConfiguration(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput);
	void setupTimeToSleep(int timeToSleep);
	void setup();
	void setup(int timeToSleep);
	void startMainProcess();
	void publishData(char *topic);
	void publishData(char *topic, char *dataInput);
	void publishData(char *topic, String dataInput);
	void publishData(char *topic, uint8_t *dataInput);
	void setHandler(CommandHandler *handler);
	void startSleep();
	String getDeviceName();
	char *getDeviceNameAsChar();
	TaskHandle_t mainThread;
	MqttService *mqttService;
	LocalServer *localServer;
	StoreService *storeService;
	SleepService *sleepService;

private:
	void setupWifi();
	void extractWifiCredentials();
	void generateHostname();
	void configureEeprom();
	static void singleProcess(void *pvParameters);
	static void singleOnboardProcess(void *pvParameters);
};

#endif
