#include "Arduino.h"
#include "MqttService.h"
#include "OtaService.h"
#include "CommandHandler.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define LED_BUILTIN 33

static char mqttServer[80];
static char deviceId[40];
static WiFiClient espClient;
static OtaService otaUpdateService;
static CommandHandler *commandHandler;
static esp_chip_info_t chip_info;

static PubSubClient mqttClient(espClient);

MqttService::MqttService(char *mqttServerInput, char *deviceIdInput)
{
  esp_chip_info(&chip_info);
  strcpy(mqttServer, mqttServerInput);
  strcpy(deviceId, deviceIdInput);
}

void MqttService::setup()
{
  mqttClient.setKeepAlive(5);
  mqttClient.setCallback(callback);
  mqttClient.setServer(mqttServer, 1883);
  pinMode(LED_BUILTIN, OUTPUT);
}

void MqttService::callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicAsString = String((char *)topic);
  String deviceIdAsString = String(deviceId);
  if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/update/") == 1)
  {
    otaUpdateService.update(topicAsString, 6001);
  }
  else if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/identify") == 1)
  {
    handleIdentify(message, length);
  }
  else if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/voltage") == 1)
  {
    handleVoltage();
  }
  else if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/board") == 1)
  {
    handleBoardInformations();
  }
  else
  {
    if (commandHandler != NULL)
    {
      commandHandler->handleCommand(topic, message, length);
    }
  }
}

void MqttService::handleBoardInformations()
{
  DynamicJsonDocument doc(1024);
  char output[1024];
  doc["freeHeap"] = esp_get_free_heap_size();
  doc["cores"] = chip_info.cores;
  doc["features"] = chip_info.features;
  doc["revision"] = chip_info.revision;
  doc["spi_flash_size"] = spi_flash_get_chip_size();
  
  serializeJson(doc, output);
  publishData("data/board", output);
}

void MqttService::handleVoltage()
{
  float batteryLevel = map(3.5, 0.0f, 4095.0f, 0, 100);
  publishData("data/voltage", batteryLevel);
}

void MqttService::handleIdentify(byte *message, unsigned int length)
{
  Serial.println("Identify device.");

  char json[1024];
  DynamicJsonDocument jsonDoc(1024);

  for (int i = 0; i < length; i++)
  {
    json[i] = ((char)message[i]);
  }
  deserializeJson(jsonDoc, json);
  int commandValue = jsonDoc["value"];

  Serial.print("Value ");
  Serial.print(commandValue);
  digitalWrite(LED_BUILTIN, commandValue);
}

void MqttService::reconnect()
{
  if (!mqttClient.connected())
  {
    Serial.println("Connection to MQTT not etablish, starting connection loop.");
    while (!mqttClient.connected())
    {
      String deviceIdAsString = String(deviceId);
      if (mqttClient.connect(deviceId))
      {
        this->registerToTopic("esp32");
        this->registerToTopic("mqtt/device/ping");
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/update/*");
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/commands/*");
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/identify");
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/voltage");
        this->registerToTopic("mqtt/device/" + deviceIdAsString + "/board");
        this->publishData("data/ip", WiFi.localIP().toString());
      }
      else
      {
        Serial.print("Failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

void MqttService::setHandler(CommandHandler *handler)
{
  Serial.println("Set Handler in MQTT");
  commandHandler = handler;
}

void MqttService::registerToTopic(String topic)
{
  char topicBuffer[80];
  topic.toCharArray(topicBuffer, 80);
  mqttClient.subscribe(topicBuffer);
}

void MqttService::publishData(char *topicInput)
{
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, NULL, false);
}

void MqttService::publishData(char *topicInput, float dataInput)
{
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  String valueString = String(dataInput, 3);
  char valueBuffer[40];
  valueString.toCharArray(valueBuffer, 40);
  mqttClient.publish(boardIdBuffer, valueBuffer, false);
}

void MqttService::publishData(char *topicInput, char *dataInput)
{
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, dataInput, false);
}

void MqttService::publishData(char *topicInput, String dataInput)
{
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString + "/" + dataInput;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, NULL, false);
}

void MqttService::publishData(char *topicInput, uint8_t *dataInput)
{
  String deviceName = String(deviceId);
  String topicAsString = String(topicInput);
  String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
  char boardIdBuffer[100];
  topic.toCharArray(boardIdBuffer, 100);
  mqttClient.publish(boardIdBuffer, dataInput, false);
}

void MqttService::handleMqttClient()
{
  reconnect();
  mqttClient.loop();
}