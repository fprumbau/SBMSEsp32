#include "Luefter.h"
#include "global.h"

//v. 0.9.9.40 laeuft weder die Batterie noch ein Charger, schalte die Luefter ueber Relais 4 ab
void Luefter::check() {
  bool fansRunning = !digitalRead(RELAY_4);
  bool isBatOn = battery.isOn();
  if(debugBattery) {
    String m((char *)0);
    m.reserve(128);
    m += F("Lueft.: ");
    m += fansRunning;
    m += F("; Batt: ");
    m += isBatOn;
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
  if(isBatOn) { //Batteriebetrieb, Wechselrichter braucht Kuehlung
    if(!fansRunning) { //Versuche die Luefter nur anzuschalten, wenn sie nicht schon laufen
      msg = F("Schalte Luefter an, da der Batteriebetrieb aktiv ist");
      Serial.println(msg);
      wc.sendClients(msg.c_str());
      digitalWrite(RELAY_4, LOW);
    }
 } else if(charger.isOn()) { //Ladebetrieb, Lader brauchen Kuehlung
    if(!fansRunning) { //Versuche die Luefter nur anzuschalten, wenn sie nicht schon laufen, und die Temperatur>35°C ist, beim Balancing werden die Luefter nicht benutzt
      if(temp>TEMP_THRESHOLD_HIGH &&  temp<80) { //1.0.4 falls Temperatur ueber 80 Grad, handelt es sich mit einiger Sicherheit um eine Fehlmessung, die wir hier ignorieren
        msg = F("Schalte Luefter an, da gerade geladen wird; Temperatur: ");
        msg+=temp;
        msg+=F("°C");
        Serial.println(msg);
        wc.sendClients(msg.c_str());
        digitalWrite(RELAY_4, LOW);
      }
    } else {
      if(battery.soc>=99 || temp<TEMP_THRESHOLD_LOW) {
        msg = F("Schalte Luefter aus: Balanciere oder Temp < TEMP_THRESHOLD_LOW; Temp: ");
        msg+=temp;
        msg+=F("°C");
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
