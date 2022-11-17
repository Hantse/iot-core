/*
  MqttService.h
*/
#ifndef MqttService_h
#define MqttService_h

#include "Arduino.h"
#include "CommandHandler.h"
#include "StoreService.h"
#include "SleepService.h"
#include "BatteryHandler.h"

class MqttService
{
public:
  MqttService(char *mqttServerInput, char *deviceIdInput);
  MqttService(char *mqttServerInput, char *deviceIdInput, int portInput);
  void injectStoreService(StoreService *storeServiceInjected, SleepService *sleepServiceInjected);
  void setup();
  void setModemData(char *modemNameInput, char *modemInfoInput);
  void handleMqttClient();
  static void publishData(char *topicInput);
  static void publishData(char *topicInput, char *dataInput);
  static void publishData(char *topicInput, uint8_t *dataInput);
  static void publishData(char *topicInput, String dataInput);
  static void publishData(char *topicInput, float dataInput);
  void setHandler(CommandHandler *handler);
  void setBatteryHandler(BatteryHandler *handler);

private:
  void sendPing();
  static void callback(char *topic, byte *message, unsigned int length);
  void reconnect();
  void registerToTopic(String topic);
  static void handleUpdate(byte *message, unsigned int length);
  static void handleIdentify(byte *message, unsigned int length);
  static void handleVoltage();
  static void handleBoardInformations();
  static void handleReset();
};

#endif
