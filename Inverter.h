#ifndef INVERTER_H
#define INVERTER_H

class Inverter {
  private: 
    unsigned long lastCheckedMillis = -1;    
  public:
    bool nacht = false;
    bool dauerbetrieb = false;
    void setBlue();
    void setGreen();
    void setRed();   
    void check(); //Pruefe Werte fuer SOC und cell voltages
    bool stopBattery; //Stopflag, verhindert ein manuelles Anschalten der Batterie
    void starteNetzvorrang(String reason); //Netzbetrieb aktivieren
    bool starteBatterie(String reason); //Batteriebetrieb aktivieren   
    void handleButtonPressed(); //Tastersteuerung, um Inverterstatus zu toggeln 
    void print();
};

#endif 
