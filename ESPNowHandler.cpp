#include "Arduino.h"
#include "MqttService.h"
#include "ESPNowHandler.h"
#include <esp_now.h>

typedef struct struct_message
{
    char data[228];
} struct_message;

struct_message dataPayload;

static MqttService *mqttService;

ESPNowHandler::ESPNowHandler()
{
}

void ESPNowHandler::injectService(MqttService *mqttServiceInjected)
{
    mqttService = mqttServiceInjected;
}

void ESPNowHandler::OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&dataPayload, incomingData, sizeof(dataPayload));
    mqttService->publishData("thirdpart", dataPayload.data);
}

void ESPNowHandler::setup()
{
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);
}