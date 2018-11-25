#ifndef RELAY_H
#define RELAY_H

class Vars {
  public:

    int RELAY_S1 = 32;
    int RELAY_S2 = 33;
    
    int RELAY_PIN = 4;
    volatile int relayStatus = LOW;
    
    int LED_S1 = 25;
    int LED_S2 = 26;
};

#endif
