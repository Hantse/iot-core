# iot-core

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