#ifndef WEBCOM_H
#define WEBCOM_H

#include <ESPAsyncWebServer.h>

#include "global.h"

class WebCom {
  
  private:

  public:
    /**
     * Toggle:
     * - @d1-true
     * - @d1-false
     * - @d2-true
     * - @d2-false
    */
    void toggleDebug(unsigned char* payload);
    void sendClients(String msg, bool data);
    void updateUi(AsyncWebSocketClient *client, bool all);
    void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
   
};

#endif
