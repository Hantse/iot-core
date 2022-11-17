#include "Arduino.h"
#include "StoreService.h"
#include "EEPROM.h"

#define EEPROM_SIZE 1024
#define IS_CONFIGURE_ADDRESS 0
#define WIFI_SSID_ADDRESS 100
#define WIFI_PASSWORD_ADDRESS 200

StoreService::StoreService()
{
}

bool StoreService::isConfigure()
{
    bool isConfigure = EEPROM.readBool(IS_CONFIGURE_ADDRESS);
    if (!isConfigure)
    {
        EEPROM.writeBool(IS_CONFIGURE_ADDRESS, false);
        EEPROM.commit();
    }
    String ssidAsString = EEPROM.readString(WIFI_SSID_ADDRESS);

    return isConfigure && ssidAsString.length() > 0;
}

void StoreService::storeConfiguration(String wifiSsid, String wifiPassword)
{
    EEPROM.writeBool(IS_CONFIGURE_ADDRESS, true);
    EEPROM.writeString(WIFI_SSID_ADDRESS, wifiSsid);
    EEPROM.writeString(WIFI_PASSWORD_ADDRESS, wifiPassword);
    EEPROM.commit();
}

String StoreService::getWifiSsid()
{
    String ssidAsString = EEPROM.readString(WIFI_SSID_ADDRESS);
    return ssidAsString;
}

String StoreService::getWifiPassword()
{
    String passwordAsString = EEPROM.readString(WIFI_PASSWORD_ADDRESS);
    return passwordAsString;
}

void StoreService::setup()
{
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
    }
}

void StoreService::clearStorage()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}