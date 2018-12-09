#include "global.h"

bool Inverter::isBatteryOn() {
    return digitalRead(RELAY_PIN);
}

/**
   Netzvorrang starten
*/
void Inverter::starteNetzvorrang(String reason) {
  String msg = "";
  if (digitalRead(RELAY_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    wc.sendClients("Toggle battery LOW", false);
    msg += "Starte Netzvorrang :: ";
    msg += reason;
    msg += '\n';
  } else {
    if (debug)  msg = "Kann Netzvorrang nicht starten, da schon aktiv\n";
  }
  if (msg.length() > 0) {
    if (debug) {
      Serial.println(msg);
    }
    wc.sendClients(msg, false);
  }
}

/**
   Batteriebetrieb starten
*/
void Inverter::starteBatterie(String reason) {
  String msg = "";
  if (!stopBattery) {
    if (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
      wc.sendClients("Toggle battery HIGH", false);
      msg += "Starte Netzvorrang :: ";
      msg += reason;
      msg += '\n';
    } else {
      return;
    }
  } else {
    msg = "Kann Netzvorrang nicht stoppen, da Stopflag aktiv\n";
  }
  if (msg.length() > 0) {
    if (debug) {
      Serial.println(msg);
    }
    wc.sendClients(msg, false);
  }
}
