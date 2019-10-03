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
      
  public:
    void init();
    void reset(); 
};

#endif
