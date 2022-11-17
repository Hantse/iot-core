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
#include "ESPNowHandler.h"


class Bootstrap
{
public:
	Bootstrap();
	Bootstrap(char *hostInput, int gsmPin);
	Bootstrap(char *hostInput, char *mqttServerInput);
	Bootstrap(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput);
	Bootstrap(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput, int portInput);

	void setConfiguration(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput);
	void setupTimeToSleep(int timeToSleep);
	void setup();
	void setupStandalone();
	void setupEspNowStandalone();
	void setModemData(String modemNameInput, String modemInfoInput);
	void setup(int timeToSleep);
	void startMainProcess();
	void startMainProcessStandalone();
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
	ESPNowHandler *espNowHandler;

private:
	void setupWifi();
	void extractWifiCredentials();
	void generateHostname();
	void configureEeprom();
	static void singleProcess(void *pvParameters);
	static void singleOnboardProcess(void *pvParameters);
};

#endif
