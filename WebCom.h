#ifndef WEBCOM_H
#define WEBCOM_H

#include "Vars.h"
#include <WebSocketsServer.h>

class WebCom {
  private:
    Vars _vars;
  public:
    WebCom(Vars& vars);  
    uint8_t clientCount = 0;
    uint8_t clients[256] = { -1};
    unsigned long wsServerLastSend = -1;
    WebSocketsServer wsServer = WebSocketsServer(81);
    void loop();
    void begin();
    void sendClients(String msg, bool data);
};

#endif
