#ifndef CHARG_H
#define CHARG_H

#include <Arduino.h>

#define S1 1
#define S2 2

/**
 * Ein Charger wird aktiviert ueber
 * a) battery.cpp: falls eine Zelle Unterspannung hat, wird S2 (der kleine Charger) eingeschaltet
 * b) WebCom.cpp: falls aus der GUI die Schaltung aktiviert wird 
 * c) charger.cpp: In der Methode checkOnIncome, aufgrund des gelieferten Solarstroms
 */

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
    void enableCharger(uint8_t nr, bool active, bool notify);
    void disableCharger(uint8_t nr, bool active, bool notify);
    int calculateDc(float netto);
    
  public:
    bool isOn();
    void toggleCharger(uint8_t nr, bool onOff, bool override, bool notify);
    bool isChargerOn(uint8_t nr);
    void checkOnIncome(float netto); //pruefe, ob Solarstrom konsumiert werden kann
    int getRunningMillis(uint8_t nr); //liefere die Millisekunden, die der Charger schon laeuft

};

#endif
