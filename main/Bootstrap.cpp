#include "Arduino.h"
#include "Bootstrap.h"
#include "UpdateServer.h"
#include "MqttService.h"
#include "CommandHandler.h"

#include <WiFi.h>
#include <WiFiClient.h>

static char ssid[60];
static char password[60];
static char host[60];
static char ip[40];
static char mqttServer[80];
static esp_chip_info_t chip_info;

Bootstrap::Bootstrap(char* ssidInput, char* passwordInput, char* hostInput, char* mqttServerInput)
{
  esp_chip_info(&chip_info);
  strcpy(ssid, ssidInput);
  strcpy(password, passwordInput);
  strcpy(host, hostInput);
  strcpy(mqttServer, mqttServerInput);
}

void Bootstrap::generateHostname(){
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

void Bootstrap::setup()
{
  Serial.begin(115200);
  this->generateHostname();
  // this->updateServer = new UpdateServer(host);
  this->mqttService = new MqttService(mqttServer, host);
  this->setupWifi();
  // this->updateServer->setup();
  this->mqttService->setup();
}

void Bootstrap::setupWifi(){
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

void Bootstrap::startMainProcess()
{
  xTaskCreatePinnedToCore(this->singleProcess, "MainThread", 50000, this, 1, &mainThread, 0);
}

void Bootstrap::singleProcess(void *pvParameters)
{
  Bootstrap *l_bootstrap = (Bootstrap *)pvParameters;
  Serial.println("");
  Serial.print("Main SDK Task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("");
  for (;;)
  {
    // l_bootstrap->updateServer->handleServerUpdate();
    l_bootstrap->mqttService->handleMqttClient();
    delay(1);
  }
}

void Bootstrap::setHandler(CommandHandler* handler){
  this->mqttService->setHandler(handler);
}

void Bootstrap::publishData(char* topic){
  this->mqttService->publishData(topic);
}

void Bootstrap::publishData(char* topic, char* dataInput){
  this->mqttService->publishData(topic, dataInput);
}
