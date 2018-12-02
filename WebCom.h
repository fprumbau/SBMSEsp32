#ifndef WEBCOM_H
#define WEBCOM_H

#include "global.h"
#include <WebSocketsServer.h>

class WebCom {
  private:
    unsigned long _wsServerLastSend = -1; //Letztes Senden einer Nachricht an die Clients
    WebSocketsServer _wsServer = WebSocketsServer(81); //Websocketserver, um Nachrichten an Clients zu senden

  public:

    typedef void (*WebSocketServerEvent)(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    typedef bool (*WebSocketServerHttpHeaderValFunc)(String headerName, String headerValue);

    WebCom();

    //client connected to send?
    bool ready = false;

    uint8_t clientCount = 0;
    uint8_t clients[256] = { -1};
    bool notifiedNoClient = false;
    void loop();
    bool sendTXT(uint8_t num, String& payload);
    IPAddress remoteIP(uint8_t num);
    void begin(WebSocketServerEvent cbEvent);
    void sendClients(String msg, bool data);
};

#endif
