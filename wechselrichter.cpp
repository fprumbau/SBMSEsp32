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

void Inverter::setBlue() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void Inverter::setGreen() {
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void Inverter::setRed() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

/**
 * Pruefe die Werte fuer SOC (StateOfCharge) und
 * CellVoltages (cv)
 */
void Inverter::check()  {
  if (soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  if (( millis() - lastCheckedMillis ) < 3000) { //Pruefung hoechstens alle 3 Sekunden
    return;
  }
  
  Serial.print("Check...  ; failureCount: ");
  Serial.println(failureCount);
  lastCheckedMillis = millis();

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }

  boolean stop = false;
  String message = "";
  if (soc < SOC_LIMIT) {
    message = "State of charge below ";
    message += SOC_LIMIT;
    message += "%";
    stop = true;
  }
  if (!stop) {
    for (int k = 0; k < 7; k++) {
      if (cv[k] < LOW_VOLTAGE_MILLIS) {
        message = "Undervoltage cell: ";
        message += k;
        stop = true;
      }
    }
  }
  if (stop) {
    failureCount++;
    if (failureCount < errLimit) { //einen 'Fehlversuch' ignorieren.
      if (debug) {
        Serial.print("Error found, waiting until failureCount reaches ");
        Serial.print(errLimit);
        Serial.print("; now: ");
        Serial.println(failureCount);
      }
    } else {
      if (!stopBattery) {
        if (debug) {
          Serial.println("Error limit reached, stopping inverter...");
        }
      }
      stopBattery = true; //
      starteNetzvorrang("Interrupt(NZV); " + message);
      setRed();
    }
  } else {
    if (failureCount >= errLimit) {
      failureCount = 0;
    }
    //Hier sollte nicht die Batterie gestartet, sondern nur freigeschaltet werden!!!
    stopBattery = false;
    setGreen();
  }
}

/**
 * Toggeln des Inverter-/Batteriebetriebs
 */
void Inverter::handleButtonPressed() {

    Serial.println("Button pressed");

    bool relayStatus = digitalRead(RELAY_PIN);
    if (relayStatus == HIGH) {
      // starte Netzvorrang
      starteNetzvorrang("Buttonaction");
    } else {
      if (!stopBattery) {
        starteBatterie("Buttonaction");
      } else {
        if (debug) {
          Serial.println("ON, kann Netzvorrang nicht abschalten (Stop wegen SOC oder Low Voltage)");
        }
      }
    }  
}
