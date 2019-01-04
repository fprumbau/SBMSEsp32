#include "battery.h"

#include "global.h"

void Battery::checkCellVoltages() {

  //c) 0.9.9.37: Egal, ob stop aktiv oder nicht, Teste Zellspannungen und aktiviere ggfls. S2(175W) für 5Min
  if(!charger.isChargerOn(2)) {
    
    for (int k = 0; k < 8; k++) {

      if(debug2) {
        String d = "Cell: ";
        d+=k;
        d+="; Voltage: ";
        d+=cv[k];
        d+="; Limit: ";
        d+=LOW_MINIMAL_CV_MILLIS;
        wc.sendClients(d);
      }      
      if (cv[k] < LOW_MINIMAL_CV_MILLIS) {
            String m = "Aktiviere Solarcharger 2 wegen Zellunterspannung Zelle ";
            m+=k;
            Serial.println(m);
            wc.sendClients(m);
            charger.toggleCharger(2,true,true);
            break;            
      }
    }
  } else {
      if(charger.getRunningMillis(2) > 300000) { //nachdem S2 5 Minuten gelaufen ist, abschalten (nächste Messung kann Charger wieder aktivieren)
          String m = "Deaktiviere Solarcharger 2 nach 5 Minuten Laufzeit jetzt...";
          Serial.println(m);
          wc.sendClients(m);
          charger.toggleCharger(2,false,true);
      }
  }  
}
