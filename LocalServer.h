/*
  LocalServer.h
*/
#ifndef LocalServer_h
#define LocalServer_h

#include "Arduino.h"
#include "StoreService.h"

class LocalServer
{
public:
  LocalServer();
  LocalServer(char *ssidInput, char *passwordInput);
  void injectStoreService(StoreService *storeService);
  void startServer();
  void handleClient();

private:
  static void handleOnConnect();
  static void handleOnScan();
  static void handleConfigure();
};

#endif
