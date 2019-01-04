#ifndef CHARG_H
#define CHARG_H

#include <Arduino.h>

class Charger {

  private:
    int s1_switched = -1;
    int s2_switched = -1;    
    //Counter, deren Grenzwerte (3...10) ueberschreiten muss, damit die Charger abgeschaltet werden
    int s1_countBeforeOff = -1; 
    int s2_countBeforeOff = -1;
    //manuelle Schaltung der Solarcharger ermöglichen
    bool s1override = false;
    bool s2override = false;
    void enableCharger(uint8_t nr, bool active);
    void disableCharger(uint8_t nr, bool active);
    
  public:
    void toggleCharger(uint8_t nr, bool onOff, bool override);
    bool isChargerOn(uint8_t nr);
    void checkOnIncome(float netto); //pruefe, ob Solarstrom konsumiert werden kann
    int getRunningMillis(uint8_t nr); //liefere die Millisekunden, die der Charger schon laeuft

};

#endif
