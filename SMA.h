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
    int s1_switched = -1;
    int s2_switched = -1;
    //manuelle Schaltung der Solarcharger ermöglichen
    bool s1override = false;
    bool s2override = false;
    bool isChargerOn(byte nr);
    void enableCharger(byte nr, bool active);
    void disableCharger(byte nr, bool active);
    
  public:
    SMA(Vars& vars, WebCom& wc);
    void read();
    void init(MyWifi& myWifi);
    void toggleCharger(byte nr, bool onOff, bool override);
  
};

#endif
