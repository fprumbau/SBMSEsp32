#ifndef SMA_H
#define SMA_H

#include <WiFiUdp.h>

class SMA {

  private:
    byte buf[601]; //buffer to hold incoming packet
    char hex[1202]; //Hex von buf
    char wlb[9]; //Wirkleistung Bezug
    char wll[9]; //Wirkleistung Lieferung
    int s1_switched = -1;
    int s2_switched = -1;
    //manuelle Schaltung der Solarcharger ermöglichen
    bool s1override = false;
    bool s2override = false;
    void enableCharger(byte nr, bool active);
    void disableCharger(byte nr, bool active);
    
  public:
    void init();
    void read();
    void toggleCharger(byte nr, bool onOff, bool override);
    bool isChargerOn(byte nr);
};

#endif
