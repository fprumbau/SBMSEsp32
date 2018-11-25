#ifndef RELAY_H
#define RELAY_H


class Vars {
  public:
    bool debug = false;
    bool debug2 = false;

    int RELAY_S1 = 32;
    int RELAY_S2 = 33;
    
    int RELAY_PIN = 4;
    int relayStatus = 0;
    
    int LED_S1 = 25;
    int LED_S2 = 26;
};

#endif
