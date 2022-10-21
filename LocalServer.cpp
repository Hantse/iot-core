#include "Arduino.h"
#include "LocalServer.h"
#include <WiFi.h>

static char ssid[60];
static char password[60];
static WiFiServer server(80);

LocalServer::LocalServer(char* ssidInput, char* passwordInput)
{
  strcpy(ssid, ssidInput);
  strcpy(password, passwordInput);
}

void LocalServer::startServer(){
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    server.begin();
}