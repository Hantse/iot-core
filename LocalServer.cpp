#include "Arduino.h"
#include "LocalServer.h"
#include <WiFi.h>
#include <WebServer.h>

static char ssid[60];
static char password[60];
static WebServer server(80);

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
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

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