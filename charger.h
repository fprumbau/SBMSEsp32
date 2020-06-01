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
    long s1MinRestMillis = 60000; //0.9.9.53
    long s2MinRestMillis = 30000; //0.9.9.53
    long s1_lowNettoMillis = -1;
    //0.9.9.88 Da der Charger S2 selbstregelnd ist (bis runter auf 1A) sollte er erst wirlich abgeschaltet werden, 
    //         wenn 5Min, also 300000 Millis zu wenig Ertrag reinkommt

    long s2_lowNettoMillis = -1;
    //manuelle Schaltung der Solarcharger ermöglichen
    void enableCharger(uint8_t nr, bool notify);
    void disableCharger(uint8_t nr, bool notify);
    int calculateDc(float netto);
    int getS2Power(); //S2 ist variabel
    long wait_excess_power_start_millis = 0; //Solange muss ein Ueberangebot an Solarstrom herrschen, bevor S1 angeschaltet wird, v.1.0.7
    int dutyCycle = 0; //0.9.9.60, 0...1023
    
  public:
    bool isOn();
    void stop();
    void toggleCharger(uint8_t nr, bool onOff, bool notify);
    bool isChargerOn(uint8_t nr);
    void checkOnIncome(); //pruefe, ob Solarstrom konsumiert werden kann
    unsigned long getRunningMillis(uint8_t nr); //liefere die Millisekunden, die der Charger schon laeuft
    void print();
    bool automatedCharging = true;
};

#endif
