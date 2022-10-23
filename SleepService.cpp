#include "Arduino.h"
#include "SleepService.h"
#include "EEPROM.h"

static int usFactor = 1000000;
static bool canStartSleep = true;
static int sleepTime = 15;

SleepService::SleepService(int timeToSleep)
{
    sleepTime = timeToSleep;
}

void SleepService::setCanSleep(bool sleepStatus)
{
    canStartSleep = sleepStatus;
}

void SleepService::startSleep()
{
    if (canStartSleep)
    {
        Serial.println("Setup ESP32 to sleep for every " + String(sleepTime) + " Seconds");
        esp_sleep_enable_timer_wakeup(sleepTime * usFactor);
        esp_deep_sleep_start();
    }
}

void SleepService::printWakeupReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}