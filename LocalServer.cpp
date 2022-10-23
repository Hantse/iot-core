#include "Arduino.h"
#include "LocalServer.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "StoreService.h"

static WebServer server(80);
static char ssid[60];
static char password[60];
static char hostname[60];
static DynamicJsonDocument doc(1024);
static StoreService *storeService;

LocalServer::LocalServer()
{
}

LocalServer::LocalServer(char *ssidInput, char *passwordInput)
{
    strcpy(ssid, ssidInput);
    strcpy(password, passwordInput);
}

LocalServer::LocalServer(char *hostNameInput, char *ssidInput, char *passwordInput)
{
    strcpy(ssid, ssidInput);
    strcpy(password, passwordInput);
    strcpy(hostname, hostNameInput);
}

void LocalServer::injectStoreService(StoreService *storeServiceInjected)
{
    storeService = storeServiceInjected;
}

void LocalServer::startServer()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i)
    {
        doc["availableNetworks"][i]["ssid"] = WiFi.SSID(i);
        doc["availableNetworks"][i]["rssi"] = WiFi.RSSI(i);
    }

    delay(100);
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

    server.on("/scan", handleOnScan);
    server.on("/", handleOnConnect);
    server.on("/configure", handleConfigure);
    server.begin();

    Serial.println("HTTP server started");
    Serial.println(ipStr);
}

void LocalServer::handleClient()
{
    server.handleClient();
}

void LocalServer::handleOnScan()
{
    String jsonScan;
    serializeJson(doc, jsonScan);
    server.send(200, "application/json", jsonScan);
}

void LocalServer::handleConfigure()
{
    String qsid = server.arg("ssid");
    String qpass = server.arg("password");
    String content = "";
    int statusCode = 200;

    if (qsid.length() > 0 && qpass.length() > 0)
    {
        storeService->storeConfiguration(qsid, qpass);
        content = "{\"success\":\"true\"}";
        statusCode = 200;
    }
    else
    {
        content = "{\"error\":\"Missing SSID or Password\"}";
        statusCode = 404;
        Serial.println("Sending 404");
    }

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(statusCode, "application/json", content);

    delay(2500);
    ESP.restart();
}

void LocalServer::handleOnConnect()
{
    DynamicJsonDocument doc(1024);
    doc["hostname"] = hostname;

    String configAsString;
    serializeJson(doc, configAsString);

    server.send(200, "text/json", configAsString);
}