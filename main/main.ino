#include "Bootstrap.h"

Bootstrap bootStrap("Home", "PCSNLVOI", "ESP32", "192.168.1.131");

void setup()
{
  bootStrap.setup();
  bootStrap.startMainProcess();
}

void loop()
{
  delay(1500);
  Serial.print("Loop running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("");
  bootStrap.publishData("data/test-temp", "Fake event");
  delay(10000);
}
