#ifndef WEBCOM_H
#define WEBCOM_H

#include <ESPAsyncWebServer.h>

#include "global.h"

class WebCom {
  
  private:

  public:
  
    void sendClients(String msg, bool data);
    void updateUi(AsyncWebSocketClient *client, bool all);
   
};

#endif
