#ifndef VARS_H
#define VARS_H

#include "global.h"

class Vars {
  public:
      
    bool debug = false;
    bool debug2 = false;

    int RELAY_S1 = 33;
    int RELAY_S2 = 32;
    
    int RELAY_PIN = 21;
    
    int LED_S1 = 25;
    int LED_S2 = 26;

    //manuelle Inverterumschaltung
    int TASTER = 19;

};

#endif
