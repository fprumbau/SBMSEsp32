#ifndef WEBCOM_H
#define WEBCOM_H

#include <ESPAsyncWebServer.h>

#include "global.h" 

class WebCom {
  
  private:
    void buildMessage(String* msg, const char* name, const char* value);
  public:
    void sendJson(const char* key, const char* value);
    void sendClients(const char* msg);
    void sendClients(const char* msg, bool save);
    void updateUi(AsyncWebSocketClient *client, bool all);
    void updateUi();
    void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
    void print();
   
};

#endif
