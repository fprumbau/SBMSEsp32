#ifndef SMA_H
#define SMA_H

#include <WiFiUdp.h>

class SMA {

  private:
    bool initialized = false;
    byte buf[601]; //buffer to hold incoming packet
    char hex[1202]; //Hex von buf
    char wlb[9]; //Wirkleistung Bezug
    char wll[9]; //Wirkleistung Lieferung
    void reset();
    void ping();
    
  public:
    void init();
    void read();
};

#endif
