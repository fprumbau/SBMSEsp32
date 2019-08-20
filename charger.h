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
    unsigned long s1_switched=0;
    unsigned long s2_switched=0;    
    //Counter, deren Grenzwerte (3...10) ueberschreiten muss, damit die Charger abgeschaltet werden
    int s1_countBeforeOff = -1; 
    //0.9.9.88 Da der Charger S2 selbstregelnd ist (bis runter auf 1A) sollte er erst wirlich abgeschaltet werden, 
    //         wenn 5Min, also 300000 Millis zu wenig Ertrag reinkommt
    int s2_millisBeforeOff = -1;
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
    void checkOnIncome(); //pruefe, ob Solarstrom konsumiert werden kann
    unsigned long getRunningMillis(uint8_t nr); //liefere die Millisekunden, die der Charger schon laeuft

};

#endif
