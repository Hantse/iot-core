#include "Arduino.h"
#include "MqttService.h"
#include "OtaService.h"
#include "CommandHandler.h"
#include "BatteryHandler.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "StoreService.h"
#include "SleepService.h"

#define LED_BUILTIN 33

static char mqttServer[80];
static char deviceId[40];
static char modemName[80];
static char modemInfo[80];
static int port = 1883;

static WiFiClient espClient;
static OtaService otaUpdateService;
static CommandHandler *commandHandler;
static BatteryHandler *batteryHandler;
static esp_chip_info_t chip_info;
static StoreService *storeService;
static SleepService *sleepService;
static PubSubClient mqttClient(espClient);

MqttService::MqttService(char *mqttServerInput, char *deviceIdInput)
{
	esp_chip_info(&chip_info);
	strcpy(mqttServer, mqttServerInput);
	strcpy(deviceId, deviceIdInput);
}

MqttService::MqttService(char *mqttServerInput, char *deviceIdInput, int portInput)
{
	esp_chip_info(&chip_info);
	strcpy(mqttServer, mqttServerInput);
	strcpy(deviceId, deviceIdInput);
	port = portInput;
}

void MqttService::injectStoreService(StoreService *storeServiceInjected, SleepService *sleepServiceInjected)
{
	storeService = storeServiceInjected;
	sleepService = sleepServiceInjected;
}

void MqttService::setup()
{
	mqttClient.setKeepAlive(2);
	mqttClient.setCallback(callback);
	mqttClient.setServer(mqttServer, port);
	pinMode(LED_BUILTIN, OUTPUT);
}

void MqttService::setModemData(char *modemNameInput, char *modemInfoInput)
{
	strcpy(modemName, modemNameInput);
	strcpy(modemInfo, modemInfoInput);
}

void MqttService::callback(char *topic, byte *message, unsigned int length)
{
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	Serial.println("");
	String topicAsString = String((char *)topic);
	String deviceIdAsString = String(deviceId);
	if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/update") == 1)
	{
		handleUpdate(message, length);
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
	else if (topicAsString.equals("mqtt/device/" + deviceIdAsString + "/reset") == 1)
	{
		handleReset();
	}
	else
	{
		if (commandHandler != NULL)
		{
			commandHandler->handleCommand(topic, message, length);
		}
	}
}

void MqttService::handleUpdate(byte *message, unsigned int length)
{
	Serial.println("Update trigger, starting process.");
	sleepService->setCanSleep(false);
	char json[1024];
	DynamicJsonDocument jsonDoc(1024);
	for (int i = 0; i < length; i++)
	{
		json[i] = ((char)message[i]);
	}
	deserializeJson(jsonDoc, json);

	String uri = jsonDoc["uri"];
	int port = jsonDoc["port"];

	otaUpdateService.update(uri, port, deviceId, &mqttClient);
	delay(2500);
	ESP.restart();
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
	doc["modemName"] = modemName;
	doc["modemInfo"] = modemInfo;

	serializeJson(doc, output);
	publishData("data/board", output);
}

void MqttService::handleReset()
{
	storeService->clearStorage();
	sleepService->setCanSleep(false);
	publishData("data/reset", "true");
	delay(500);
	ESP.restart();
}

void MqttService::handleVoltage()
{
	if (batteryHandler != NULL)
	{
		char* output = batteryHandler->handleCommand();
		publishData("data/battery", output);
	}
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
			Serial.println("Connection to MQTT etablish, starting subscribe.");
			String deviceIdAsString = String(deviceId);
			if (mqttClient.connect(deviceId))
			{
				this->registerToTopic("esp32");
				this->registerToTopic("mqtt/device/ping");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/update");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/commands/*");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/identify");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/voltage");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/board");
				this->registerToTopic("mqtt/device/" + deviceIdAsString + "/reset");
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
	Serial.println("Set LocalCommand Handler in MQTT");
	commandHandler = handler;
}

void MqttService::setBatteryHandler(BatteryHandler *handler)
{
	Serial.println("Set Battery Handler in MQTT");
	batteryHandler = handler;
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
