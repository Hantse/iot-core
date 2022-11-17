#include "Arduino.h"
#include "Bootstrap.h"
#include "MqttService.h"
#include "CommandHandler.h"
#include "LocalServer.h"
#include "ESPNowHandler.h"

#include "EEPROM.h"
#include <WiFi.h>
#include <WiFiClient.h>

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024
#define SerialAT Serial1

static char ssid[60];
static char password[60];
static char host[60];
static char ip[40];
static char mqttServer[80];
static int port = 1883;
static char apn[80];
static char gprsUser[80];
static char gprsPass[80];
static int sleepTime = 15;
static int gsmPin = 0;
static char modemName[80];
static char modemInfo[80];

static esp_chip_info_t chip_info;

Bootstrap::Bootstrap()
{
	Serial.begin(115200);
	esp_chip_info(&chip_info);
}

Bootstrap::Bootstrap(char *hostInput, int gsmPinInput)
{
	Serial.begin(115200);
	esp_chip_info(&chip_info);
	strcpy(host, hostInput);
	gsmPin = gsmPinInput;
}

Bootstrap::Bootstrap(char *hostInput, char *mqttServerInput)
{
	Serial.begin(115200);
	esp_chip_info(&chip_info);
	strcpy(host, hostInput);
	strcpy(mqttServer, mqttServerInput);
}

Bootstrap::Bootstrap(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput)
{
	Serial.begin(115200);
	esp_chip_info(&chip_info);
	strcpy(ssid, ssidInput);
	strcpy(password, passwordInput);
	strcpy(host, hostInput);
	strcpy(mqttServer, mqttServerInput);
}

Bootstrap::Bootstrap(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput, int portInput)
{
	Serial.begin(115200);
	esp_chip_info(&chip_info);
	strcpy(ssid, ssidInput);
	strcpy(password, passwordInput);
	strcpy(host, hostInput);
	strcpy(mqttServer, mqttServerInput);
	port = portInput;
}

void Bootstrap::setConfiguration(char *ssidInput, char *passwordInput, char *hostInput, char *mqttServerInput)
{
	strcpy(ssid, ssidInput);
	strcpy(password, passwordInput);
	strcpy(host, hostInput);
	strcpy(mqttServer, mqttServerInput);
}

void Bootstrap::setModemData(String modemNameInput, String modemInfoInput)
{
	modemNameInput.toCharArray(modemName, 80);
	modemInfoInput.toCharArray(modemInfo, 80);
}

String Bootstrap::getDeviceName()
{
	return String(host);
}

char *Bootstrap::getDeviceNameAsChar()
{
	return host;
}

void Bootstrap::generateHostname()
{
	String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
	chipId.toUpperCase();
	String hostAsString = String(host);
	String hostnameWithId = hostAsString + "-" + chipId;
	char hostnameBuffer[60];
	hostnameWithId.toCharArray(hostnameBuffer, 50);
	strcpy(host, hostnameBuffer);
	Serial.print("Generated hostname : ");
	Serial.println(host);
}

void Bootstrap::setupStandalone()
{
	this->generateHostname();
	this->sleepService->printWakeupReason();
	this->storeService = new StoreService();
	this->sleepService = new SleepService(sleepTime);
	this->mqttService = new MqttService(mqttServer, host, port);
	this->mqttService->setModemData(modemName, modemInfo);
	this->setupWifi();
	this->mqttService->injectStoreService(storeService, sleepService);
	this->mqttService->setup();
}

void Bootstrap::setup()
{
	this->generateHostname();
	this->sleepService->printWakeupReason();
	this->storeService = new StoreService();
	this->sleepService = new SleepService(sleepTime);
	this->localServer = new LocalServer(host, host, "IotDevicePassword");
	this->storeService->setup();

	if (this->storeService->isConfigure())
	{
		this->mqttService = new MqttService(mqttServer, host, port);
		this->extractWifiCredentials();
		this->setupWifi();
		this->mqttService->injectStoreService(storeService, sleepService);
		this->mqttService->setup();
	}
	else
	{
		this->sleepService->setCanSleep(false);
		this->localServer->injectStoreService(storeService);
		this->localServer->startServer();
	}
}

void Bootstrap::setup(int timeToSleep)
{
	sleepTime = timeToSleep;
	this->setup();
}

void Bootstrap::setupEspNowStandalone()
{
	this->espNowHandler = new ESPNowHandler();
	this->espNowHandler->injectService(this->mqttService);
	this->espNowHandler->setup();
}

void Bootstrap::extractWifiCredentials()
{
	String ssidAsString = this->storeService->getWifiSsid();
	ssidAsString.toCharArray(ssid, ssidAsString.length() + 1);

	String passwordAsString = this->storeService->getWifiPassword();
	passwordAsString.toCharArray(password, passwordAsString.length() + 1);
}

void Bootstrap::setupWifi()
{
	WiFi.begin(ssid, password);
	Serial.println("");

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	strcpy(ip, WiFi.localIP().toString().c_str());
	Serial.print("IP address: ");
	Serial.println(ip);
}

void Bootstrap::startMainProcessStandalone()
{
	xTaskCreatePinnedToCore(this->singleOnboardProcess, "MainThread", 50000, this, 1, &mainThread, 1);
}

void Bootstrap::startMainProcess()
{
	int isConfigure = EEPROM.read(0);
	// if (isConfigure == 1)
	// {
	// 	xTaskCreatePinnedToCore(this->singleProcess, "MainThread", 50000, this, 1, &mainThread, 1);
	// }
	// else
	// {
	xTaskCreatePinnedToCore(this->singleOnboardProcess, "MainThread", 50000, this, 1, &mainThread, 1);
	// }
}

void Bootstrap::singleOnboardProcess(void *pvParameters)
{
	Bootstrap *l_bootstrap = (Bootstrap *)pvParameters;
	Serial.println("");
	Serial.print("Main SDK Task (Onboarding) running on core ");
	Serial.print(xPortGetCoreID());
	Serial.println("");
	for (;;)
	{
		l_bootstrap->mqttService->handleMqttClient();
		// l_bootstrap->localServer->handleClient();
		delay(1);
	}
}

void Bootstrap::singleProcess(void *pvParameters)
{
	Bootstrap *l_bootstrap = (Bootstrap *)pvParameters;
	Serial.println("");
	Serial.print("Main SDK Task (Normal) running on core ");
	Serial.print(xPortGetCoreID());
	Serial.println("");
	for (;;)
	{
		l_bootstrap->localServer->handleClient();
		l_bootstrap->mqttService->handleMqttClient();
		delay(1);
	}
}

void Bootstrap::setHandler(CommandHandler *handler)
{
	Serial.println("Set Handler in Bootstrap");
	this->mqttService->setHandler(handler);
}

void Bootstrap::publishData(char *topic)
{
	this->mqttService->publishData(topic);
}

void Bootstrap::publishData(char *topic, char *dataInput)
{
	this->mqttService->publishData(topic, dataInput);
}

void Bootstrap::publishData(char *topic, String dataInput)
{
	char dataInputAsChar[200];
	dataInput.toCharArray(dataInputAsChar, dataInput.length() + 1);
	this->mqttService->publishData(topic, dataInputAsChar);
}

void Bootstrap::publishData(char *topic, uint8_t *dataInput)
{
	this->mqttService->publishData(topic, dataInput);
}

void Bootstrap::startSleep()
{
	this->sleepService->startSleep();
}
