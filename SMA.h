#ifndef SMA_H
#define SMA_H

#include <AsyncUDP.h>

class SMA {

  private:
    unsigned int portMulti = 9522; 
    char const lookup[17] = "0123456789ABCDEF";
    bool initialized = false;
    char hex[1202]; //Hex von buf
    char wlb[9]; //Wirkleistung Bezug
    char wll[9]; //Wirkleistung Lieferung
    void eval(AsyncUDPPacket* packet);
    bool hasNewPckt = false; //0.9.9.99
    long lastUdpRead = -1;
    long lastUdpReset = -1;
      
  public:
    void init();
    void reset(); 
    bool hasNewPacket();
    void resetNewPacket();
};

#endif
