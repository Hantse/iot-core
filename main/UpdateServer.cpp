#include "Arduino.h"
#include "UpdateServer.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <Update.h>
#include <WebServer.h>
#include <ESPmDNS.h>

static WebServer _server(80);
static char host[60];

UpdateServer::UpdateServer(char* hostInput){
  strcpy(host, hostInput);
}

void UpdateServer::setup(){
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  _server.on("/update", HTTP_POST, []() {
    _server.sendHeader("Connection", "close");
    _server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = _server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  _server.begin();
}

void UpdateServer::handleServerUpdate(){
  _server.handleClient();
}
