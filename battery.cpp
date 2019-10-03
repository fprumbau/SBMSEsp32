#include "battery.h"

#include "global.h"

void Battery::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Batterie: "));
  Serial.println(isOn()); 
}

void Battery::checkCellVoltages() {

  //c) 0.9.9.37: Egal, ob stop aktiv oder nicht, Teste Zellspannungen und aktiviere ggfls. S2(175W) für 5Min
  if(!charger.isChargerOn(2)) {
    
    for (int k = 0; k < 8; k++) {

      if(debugBattery) {
        String d = String((char*)0);
        d.reserve(64);
        d+="Cell: ";
        d+=k;
        d+="; Voltage: ";
        d+=cv[k];
        d+="; Limit: ";
        d+=LOW_MINIMAL_CV_MILLIS;
        wc.sendClients(d.c_str());
      }      
      if (cv[k] < LOW_MINIMAL_CV_MILLIS) {    
        cvErrChg[k]++;                        
      } else {
        cvErrChg[k]=0;
      }
      if(cvErrChg[k]>3) {
          String m = F("Aktiviere Solarcharger 2 wegen Zellunterspannung Zelle ");
          m+=k;
          m+=F("; gemessene Spannung (in mv): ");
          m+=cv[k];
          Serial.println(m);
          wc.sendClients(m.c_str());
          charger.toggleCharger(S2,true,true,true);
          s2ActForLowCV = true;
          break;            
      }
    }
  } else {
      if(s2ActForLowCV && charger.getRunningMillis(2) > 600000) { //nachdem S2 10 Minuten gelaufen ist, abschalten (nächste Messung kann Charger wieder aktivieren)
          String m = F("Deaktiviere Solarcharger 2 nach 10 Minuten Ladezeit jetzt...");
          Serial.println(m);
          wc.sendClients(m.c_str());
          charger.toggleCharger(S2,false,true, true);
          s2ActForLowCV = false;
      }
  }  
}

bool Battery::isOn() {
    return digitalRead(RELAY_PIN);
}

//v. 0.9.9.40 laeuft weder die Batterie noch ein Charger, schalte die Luefter ueber Relais 4 ab
void Battery::controlFans() {
  bool fansRunning = !digitalRead(RELAY_4);
  if(debugBattery) {
    String m((char *)0);
    m.reserve(128);
    m += F("Lueft.: ");
    m += fansRunning;
    m += F("; Batt: ");
    m += isOn();
    m += F("; Chrg: ");
    m += charger.isOn();
    m += F("; RS1: ");
    m +=digitalRead(RELAY_S1);
    m += F("; RS2: ");
    m +=digitalRead(RELAY_S2);
    m += F("; R3: ");
    m +=digitalRead(RELAY_3);
    m += F("; isOnS1: ");
    m += charger.isChargerOn(1);
    m += F("; isOnS2: ");
    m += charger.isChargerOn(2);
    m += F("\nBez/Lief: ");
    m += netto;
    wc.sendClients(m.c_str());
  }
  String msg;
  if(isOn()) { //Batteriebetrieb, Wechselrichter braucht Kuehlung
    if(!fansRunning) { //Versuche die Luefter nur anzuschalten, wenn sie nicht schon laufen
      msg = F("Schalte Luefter an, da der Batteriebetrieb aktiv ist");
      Serial.println(msg);
      wc.sendClients(msg.c_str());
      digitalWrite(RELAY_4, LOW);
    }
 } else if(charger.isOn()) { //Ladebetrieb, Lader brauchen Kuehlung
    if(!fansRunning) { //Versuche die Luefter nur anzuschalten, wenn sie nicht schon laufen, aber nur wenn der Ladestand<99% UND die Temperatur>35°C ist, beim Balancing werden die Luefter nicht benutzt
      if(temp>TEMP_THRESHOLD_HIGH) {
        msg = F("Schalte Luefter an, da gerade geladen wird; Temperatur: ");
        msg+=temp;
        msg+="°C";
        Serial.println(msg);
        wc.sendClients(msg.c_str());
        digitalWrite(RELAY_4, LOW);
      }
    } else {
      if(soc>=99 && temp<TEMP_THRESHOLD_LOW) {
        msg = F("Schalte Luefter ab, da fertig geladen wurde");
        Serial.println(msg);
        wc.sendClients(msg.c_str());
        digitalWrite(RELAY_4, HIGH);
      }      
    }
 } else {
    if(fansRunning) { //Versuche, die Luefter auszuschalten nur dann, wenn sie schon laufen
      if(temp<TEMP_THRESHOLD_LOW) {
        if(fansRunning) {
          msg = F("Schalte Luefter ab, da weder Batterie noch Charger laufen");
          Serial.println(msg);
          wc.sendClients(msg.c_str());
          digitalWrite(RELAY_4, HIGH);
        }
      } else {
        Serial.println(F("Luefter bleibt aktiv, da die Temperatur zu hoch ist (Grad Celsius): "));
        Serial.println(temp);
      }
    }
  }
}
