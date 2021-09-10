#include "battery.h"

#include "global.h"

void Battery::print() {
  Log.warningln(F("--------------------------------"));
  Log.warningln(F("Batterie isOn: %T"), isOn()); 
  Log.warningln(F("Ladezustand: %d"), soc); 
  Log.warningln(F("SOC-Limit: %d"), socLimit);    
  Log.warningln(F("battery.enabled: %T"), enabled);     
  Log.warningln(F("SOC_HYST: %d"), SOC_HYST);       

  //Ueber die 8 Zellen iterieren
  for (int k = 0; k < 8; k++) {
          Log.warningln(F("Cellerrors charger %s : %s"), k, cvErrChg[k]);       
  }
}

/**
 * Aufruf aus Inverter
 */
boolean Battery::checkCellVoltages() {

  boolean stopBattery = false;
  int cellNumber;

  //Ueber die 8 Zellen iterieren
  for (int k = 0; k < 8; k++) {

      int lowMilliVolts = LOW_MINIMAL_CV_MILLIS;
      if(inverter.nacht && voltageSensor.hasNetzspannung()) {
          lowMilliVolts = LOW_NIGHTLY_CV_MILLIS;
      }

      if(debugBattery) {
          status = String((char*)0);
          status.reserve(128);
          status+="Cell: ";
          status+=k;
          status+="; Volts: ";
          status+=cv[k];
          status+="; Limit: ";
          status+=lowMilliVolts;
          status+="; SOC: ";
          status+=soc;
          wc.sendClients(status.c_str());
      }      
      if (cv[k] < lowMilliVolts) {    
          if(cv[k] > 0) {
            cvErrChg[k]++; 
          }
          Log.warningln("%s: %s", cvErrChg[k], status);                                     
      } else {
          cvErrChg[k]=0;
      }
      //Errorcount der Zelle groesser als 10 Fehler -> Ende der Auswertung
      if(cvErrChg[k]>10) {
          cellNumber=k;
          stopBattery = true;
          break;          
      }
      
  }

  //Massnahmen treffen
  if(stopBattery) {

      //Zur Zellunterstuetzung den Charger2 aktivieren
      if(!charger.isChargerOn(2)) {
        
          String m = F("Aktiviere Solarcharger 2 wegen Zellunterspannung Zelle ");
          m+=cellNumber;
          m+=F("; gemessene Spannung (in mv): ");
          m+=cv[cellNumber];
          Log.warningln(m.c_str());
          wc.sendClients(m.c_str());
          charger.toggleCharger(S2,true,true);
          s2ActForLowCV = true;
          
      } else {
        
          //nachdem S2 10 Minuten gelaufen ist, abschalten (nächste Messung kann Charger wieder aktivieren)
          if(s2ActForLowCV && charger.getRunningMillis(2) > 600000) { 
              String m = F("Deaktiviere Solarcharger 2 nach 10 Minuten Ladezeit jetzt...");
              Log.warningln(m.c_str());
              wc.sendClients(m.c_str());
              charger.toggleCharger(S2,false,true);
              s2ActForLowCV = false;
          }
          
      }   
  }
  
  return stopBattery;
}

bool Battery::isReady2Activate() {
  int limit = socLimit + SOC_HYST;
  if(enabled && soc > limit && soc <= 100) {
    if(debugBattery) {
      Log.warningln(F("Battery::isReady2Activate enabled==true\nBattery::isReady2Activate soc: %d\nBattery::isReady2Activate socLimit: %d\nBattery::isReady2Activate limit: %d"), soc, socLimit, limit);
    }
    return true;
  }
  if(enabled && debugBattery) {
    Log.warningln(F("Battery::isReady2Activate enabled==true (Batterie wird NICHT aktiviert)\nBattery::isReady2Activate soc: %d\nBattery::isReady2Activate socLimit: %d\nBattery::isReady2Activate limit: %d"), soc, socLimit, limit);
  }
  return false;
}


bool Battery::isOn() {
    return digitalRead(RELAY_PIN);
}
