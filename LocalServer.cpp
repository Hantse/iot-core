#include "Arduino.h"
#include "LocalServer.h"
#include <WiFi.h>
#include <WebServer.h>

IPAddress local_ip(192,168,1,4);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

static char ssid[60];
static char password[60];

LocalServer::LocalServer()
{
}

LocalServer::LocalServer(char *ssidInput, char *passwordInput)
{
    strcpy(ssid, ssidInput);
    strcpy(password, passwordInput);
}

void LocalServer::startServer()
{
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    server.on("/", handleOnConnect);
    server.begin();
    Serial.println("HTTP server started");
}

void LocalServer::handleClient()
{
    server.handleClient();
}

void LocalServer::handleOnConnect()
{
    Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
    server.send(200, "text/html", "");
}