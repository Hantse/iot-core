#include "Arduino.h"
#include "MqttService.h"
#include "OtaService.h"
#include "CommandHandler.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

static char mqttServer[80];
static char deviceId[40];
static WiFiClient espClient;
static OtaService otaUpdateService;
static CommandHandler* commandHandler;

static long lastPing = 0;
PubSubClient mqttClient(espClient);

MqttService::MqttService(char* mqttServerInput, char* deviceIdInput){
  strcpy(mqttServer, mqttServerInput);
  strcpy(deviceId, deviceIdInput);
}

void MqttService::setup(){
  mqttClient.setKeepAlive(1);
  mqttClient.setServer(mqttServer, 1883);
}

void MqttService::callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicAsString = String((char*)topic);
  String deviceIdAsString = String(deviceId);
  if(topicAsString.equals("mqtt/device/" + deviceIdAsString + "/update/") == 1){
    otaUpdateService.update(topicAsString, 6001);
  }else {
    if (commandHandler != NULL){
      commandHandler->handleCommand(topic, message);
    }
  }
}

void MqttService::reconnect()
{
  if (!mqttClient.connected())
  {
    Serial.println("Connection to MQTT not etablish, starting connection loop.");
    while (!mqttClient.connected())
    {
      String deviceIdAsString = String(deviceId);
      if (mqttClient.connect(deviceId)){
        this->registerToTopic("mqtt/device/ping");  
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/update/*");      
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/commands/*"); 
      } else {
        Serial.print("Failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

void MqttService::setHandler(CommandHandler* handler){
  commandHandler = handler;
}

void MqttService::registerToTopic(String topic){
  char topicBuffer[80];
  topic.toCharArray(topicBuffer, 80);
  mqttClient.subscribe(topicBuffer);
}

void MqttService::sendPing()
{
  String deviceName = String(deviceId);
  String topic = "mqtt/device/" + deviceName + "/ping";
  char boardIdBuffer[50];
  topic.toCharArray(boardIdBuffer, 50);
  mqttClient.publish(boardIdBuffer, "", false);
}

void MqttService::publishData(char* topicInput){
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, "TEST", false);
}

void MqttService::publishData(char* topicInput, char* dataInput){
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, dataInput, false);
}

void MqttService::handleMqttClient(){
  reconnect();
  mqttClient.loop();
  long now = millis();
  if (now - lastPing > 60000)
  {
    lastPing = now;
    sendPing();
  }
}
