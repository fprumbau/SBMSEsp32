#include "battery.h"

#include "global.h"

void Battery::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Batterie: "));
  Serial.println(isOn()); 
  Serial.print(F("Ladezustand: "));
  Serial.println(soc); 
  Serial.print(F("SOC-Limit: "));
  Serial.println(socLimit);    
  Serial.print(F("battery.enabled: "));
  Serial.println(enabled);     
  Serial.print(F("SOC_HYST: "));
  Serial.println(SOC_HYST);       
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
      if(inverter.nacht) {
          lowMilliVolts = LOW_NIGHTLY_CV_MILLIS;
      }

      if(debugBattery) {
          String d = String((char*)0);
          d.reserve(64);
          d+="Cell: ";
          d+=k;
          d+="; Voltage: ";
          d+=cv[k];
          d+="; Limit: ";
          d+=lowMilliVolts;
          wc.sendClients(d.c_str());
      }      
      if (cv[k] < lowMilliVolts) {    
          if(cv[k] > 0) {
            cvErrChg[k]++; 
          }
          Serial.print(cvErrChg[k]);       
          Serial.print(F(": Batteriezelle "));         
          Serial.print(k); 
          Serial.print(F(" meldet Unterspannung: "));
          Serial.println(cv[k]);                    
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
          Serial.println(m);
          wc.sendClients(m.c_str());
          charger.toggleCharger(S2,true,true,true);
          s2ActForLowCV = true;
          
      } else {
        
          //nachdem S2 10 Minuten gelaufen ist, abschalten (nächste Messung kann Charger wieder aktivieren)
          if(s2ActForLowCV && charger.getRunningMillis(2) > 600000) { 
              String m = F("Deaktiviere Solarcharger 2 nach 10 Minuten Ladezeit jetzt...");
              Serial.println(m);
              wc.sendClients(m.c_str());
              charger.toggleCharger(S2,false,true, true);
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
      Serial.println(F("Battery::isReady2Activate enabled==true"));
      Serial.print(F("Battery::isReady2Activate soc: "));
      Serial.println(soc);
      Serial.print(F("Battery::isReady2Activate socLimit: "));
      Serial.println(socLimit);
      Serial.print(F("Battery::isReady2Activate limit: "));
      Serial.println(limit);
    }
    return true;
  }
  if(enabled && debugBattery) {
      Serial.println(F("Battery::isReady2Activate enabled==true (Batterie wird NICHT aktiviert)"));
      Serial.print(F("Battery::isReady2Activate soc: "));
      Serial.println(soc);
      Serial.print(F("Battery::isReady2Activate socLimit: "));
      Serial.println(socLimit);
      Serial.print(F("Battery::isReady2Activate limit: "));
      Serial.println(limit);
  }
  return false;
}


bool Battery::isOn() {
    return digitalRead(RELAY_PIN);
}
