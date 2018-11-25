#ifndef WEBCOM_H
#define WEBCOM_H

#include "Vars.h"
#include <WebSocketsServer.h>

class WebCom {
  private:
    Vars _vars;

  public:

    typedef void (*WebSocketServerEvent)(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    typedef bool (*WebSocketServerHttpHeaderValFunc)(String headerName, String headerValue);

    WebCom();
    WebCom(Vars& vars);  

    //client connected to send?
    bool ready = false;

    uint8_t clientCount = 0;
    uint8_t clients[256] = { -1};
    bool notifiedNoClient = false;
    unsigned long wsServerLastSend = -1;
    WebSocketsServer wsServer = WebSocketsServer(81);
    void loop();
    bool sendTXT(uint8_t num, String& payload);
    IPAddress remoteIP(uint8_t num);
    void begin(WebSocketServerEvent cbEvent);
    void sendClients(String msg, bool data);
};

#endif
