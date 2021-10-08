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
