#ifndef SMA_H
#define SMA_H

#include <WiFiUdp.h>

#include "WebCom.h"
#include "MyWifi.h"
#include "Vars.h"

class SMA {

  private:
    Vars _vars;
    byte buf[601]; //buffer to hold incoming packet
    char hex[1202]; //Hex von buf
    char wlb[9]; //Wirkleistung Bezug
    char wll[9]; //Wirkleistung Lieferung
    WiFiUDP _udp;
    WebCom _wc;
    
  public:
    SMA(Vars& vars, WebCom& wc);
    void read();
    void init(MyWifi& myWifi);
  
};

#endif
