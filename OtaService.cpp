#include "Arduino.h"
#include "OtaService.h"
#include "MqttService.h"
#include <WiFi.h>
#include <Update.h>
#include <ESP32httpUpdate.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

int contentLength = 0;
bool isValidContentType = false;
WiFiClient client;

OtaService::OtaService(){
}

void OtaService::update(String url, int port, char* deviceId, PubSubClient *mqttClient) {
    Serial.println(url);
    String deviceName = String(deviceId);
    String topicAsString = "update-device";
    String topic = "mqtt/device/" + deviceName + "/" + topicAsString;
    DynamicJsonDocument doc(1024);
    char output[1024];
    doc["uri"] = url;

    t_httpUpdate_return ret = ESPhttpUpdate.update(url); 

    switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                doc["success"] = false;
                doc["error"] = ESPhttpUpdate.getLastErrorString().c_str();
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                doc["success"] = true;
                doc["update"] = false;
                break;

            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                doc["success"] = true;
                doc["update"] = true;
                break;
        }

    Serial.println("Publish result.");
    serializeJson(doc, output);
    char boardIdBuffer[160];
    topic.toCharArray(boardIdBuffer, 160);
    mqttClient->publish(boardIdBuffer, output, false);    
}
