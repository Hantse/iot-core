# iot-core

## TODO
- [x] Implement voltage handler

### Basic usage
``` cpp
#include <Bootstrap.h>
#include <ESP.h>

Bootstrap bootStrap("SSID", "SSIDPASSWORD", "BOARDNAME", "SERVERIP");

void setup()
{
  Serial.setDebugOutput(true);
  bootStrap.setup();
  delay(500);
  bootStrap.startMainProcess();
}

void loop()
{
  delay(1500);
  Serial.print("Loop running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("");
  delay(15000);
}
```

### Usage with portal for configuration
``` cpp
#include <Bootstrap.h>
#include <ESP.h>

Bootstrap bootStrap("BOARDNAME", "SERVERIP");

void setup()
{
  Serial.setDebugOutput(true);
  bootStrap.setup();
  delay(500);
  bootStrap.startMainProcess();
}

void loop()
{
  delay(1500);
  Serial.print("Loop running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("");
  delay(15000);
}
```

### Command handling

Create local command handler inherits from `CommandHandler`.

``` cpp
/*
  LocalCommandHandler.h
*/
#ifndef LocalCommandHandler_h
#define LocalCommandHandler_h

#include "Arduino.h"
#include <CommandHandler.h>

class LocalCommandHandler : public CommandHandler
{
public:
    LocalCommandHandler(char* deviceName);
    virtual void handleCommand(char* topic, byte* message, unsigned int length);
private:
    void switchLight(String bind, int value);
    void switchFlashLight(int value);
};

#endif
```

Pass device name to instantiate new command handler. Set handler reference to bootstrap.
``` cpp
localCommandHandler = new LocalCommandHandler(bootStrap.getDeviceNameAsChar());
bootStrap.setHandler(localCommandHandler);
```

Full sample.

``` cpp
#include <Bootstrap.h>
#include "LocalCommandHandler.h"
#include <ESP.h>

Bootstrap bootStrap("Home", "PCSNLVOI", "ESP32-CAM", "192.168.1.131");
CommandHandler* localCommandHandler;

void setup()
{
  Serial.setDebugOutput(true);
  bootStrap.setup();
  localCommandHandler = new LocalCommandHandler(bootStrap.getDeviceNameAsChar());
  bootStrap.setHandler(localCommandHandler);
  delay(500);
  bootStrap.startMainProcess();
}

void loop()
{
  delay(1500);
  Serial.print("Loop running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("");
  delay(15000);
}
```

``` cpp
#include <ArduinoJson.h>

#include "Arduino.h"
#include "LocalCommandHandler.h"
#include "CameraHandler.h"

#define FLASH_LIGHT 4

CameraHandler* cameraHandler;
int FRONT_LED = 2;
int BACK_LED = 14;

LocalCommandHandler::LocalCommandHandler(char* deviceName){
  cameraHandler = new CameraHandler("192.168.1.131", deviceName);
  cameraHandler->setup();

  pinMode (FRONT_LED, OUTPUT);
  pinMode (BACK_LED, OUTPUT);
  pinMode (FLASH_LIGHT, OUTPUT);

}

void LocalCommandHandler::handleCommand(char* topic, byte* message, unsigned int length){
  Serial.println("Message receive from handler.");
  char json[1024];
  DynamicJsonDocument jsonDoc(1024);

  for (int i=0;i<length;i++) {
    json[i] = ((char)message[i]);
  }
  
  Serial.println("");
  deserializeJson(jsonDoc, json);
  const char* commandName = jsonDoc["command"];
  String command = String(commandName);
  Serial.println(commandName);
  if(command.indexOf("CaptureCamera") > -1){
    cameraHandler->captureAndUpload();
  }else if(command.indexOf("flash") > -1){
    int commandValue = jsonDoc["value"];
    this->switchFlashLight(commandValue);
  }else if(command.indexOf("light") > -1){
    const char* commandBind = jsonDoc["bind"];
    String bind = String(commandBind);
    int commandValue = jsonDoc["value"];
    this->switchLight(bind, commandValue);
  }
}

void LocalCommandHandler::switchLight(String bind, int value){
  if(bind.indexOf("back") > -1){
    digitalWrite(BACK_LED, value);
  }else{
    digitalWrite(FRONT_LED, value);
  }
}

void LocalCommandHandler::switchFlashLight(int value){
    digitalWrite(FLASH_LIGHT, value);
}
```

You can process command as you want by implement your own `void LocalCommandHandler::handleCommand(char* topic, byte* message, unsigned int length)`, this method 
receive message from MQTT Server.


## ESP32 Power Consumption
https://www.of-things.de/battery-life-calculator.php

| Device | Reference [mA] | Light-Sleep [mA]  |  Deep-Sleep [mA]  |  Hibernation [mA]  |
|--------|----------------|-------------------|-------------------|--------------------|
| ESP32  |      51        |        10         |        9          |         9          |