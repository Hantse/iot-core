/*
  OtaService.h
*/
#ifndef OtaService_h
#define OtaService_h

#include "Arduino.h"

class OtaService
{
  public: 
    OtaService();
    void update(String url, int port);
  private:
    String getHeaderValue(String header, String headerName);
    String getBinName(String url);
    String getHostName(String url);
};

#endif
