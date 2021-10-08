/*
  MqttService.h
*/
#ifndef MqttService_h
#define MqttService_h

#include "Arduino.h"
#include "CommandHandler.h"

class MqttService
{
  public: 
    MqttService(char* mqttServerInput, char* deviceIdInput);
    void setup();
    void handleMqttClient();
    void publishData(char* topicInput);
    void publishData(char* topicInput, char* dataInput);
    void setHandler(CommandHandler* handler);
  private:
    void sendPing();
    static void callback(char* topic, byte* message, unsigned int length);
    void reconnect();
    void registerToTopic(String topic);
};

#endif
