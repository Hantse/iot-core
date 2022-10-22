#include "Arduino.h"
#include "StoreService.h"
#include "EEPROM.h"

#define IS_CONFIGURE_ADDRESS 0
#define WIFI_SSID_ADDRESS 48
#define WIFI_PASSWORD_ADDRESS 96

StoreService::StoreService()
{
}

bool StoreService::isConfigure()
{
    bool isConfigure = EEPROM.readBool(IS_CONFIGURE_ADDRESS);
    Serial.println(isConfigure);
    if (!isConfigure)
    {
        EEPROM.writeBool(IS_CONFIGURE_ADDRESS, false);
        EEPROM.commit();
    }

    return isConfigure;
}

void StoreService::storeConfiguration(String wifiSsid, String wifiPassword)
{
    EEPROM.writeBool(IS_CONFIGURE_ADDRESS, true);
    EEPROM.writeString(WIFI_SSID_ADDRESS, wifiSsid);
    EEPROM.writeString(WIFI_PASSWORD_ADDRESS, wifiPassword);
    EEPROM.commit();
}

char *StoreService::getWifiSsid()
{
    String ssidAsString = EEPROM.readString(WIFI_SSID_ADDRESS);
    char ssid[48];
    ssidAsString.toCharArray(ssid, ssidAsString.length() + 1);
    return ssid;
}

char *StoreService::getWifiPassword()
{
    String passwordAsString = EEPROM.readString(WIFI_PASSWORD_ADDRESS);
    char password[48];
    passwordAsString.toCharArray(password, passwordAsString.length() + 1);
    return password;
}

void StoreService::setup()
{
    if (!EEPROM.begin(1000))
    {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
    }
}