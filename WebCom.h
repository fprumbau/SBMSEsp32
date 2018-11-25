#ifndef WEBCOM_H
#define WEBCOM_H

#include <WebSocketsServer.h>

class WebCom {
  private:
    
  public:
    WebSocketsServer wsServer = WebSocketsServer(81);
    void loop();
};

#endif
