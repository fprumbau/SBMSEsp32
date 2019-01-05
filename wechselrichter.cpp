#include "global.h"

/**
   Netzvorrang starten
*/
void Inverter::starteNetzvorrang(String reason) {
  String msg = "";
  if (digitalRead(RELAY_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    wc.sendClients("Toggle battery LOW");
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
    wc.sendClients(msg);
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
      wc.sendClients("Toggle battery HIGH");
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
    wc.sendClients(msg);
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

  //Zur weiteren Pruefung sollte die soc vorliegen
  if (soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  if (( millis() - lastCheckedMillis ) < checkMillis) { //Pruefung hoechstens alle 3 Sekunden
    return;
  }
  lastCheckedMillis = millis();
    
  //v. 0.9.9.38 faellt die CV zu sehr, sollte S1 (Charger 1) aktiviert werden
  battery.checkCellVoltages();

  //v. 0.9.9.40 ggfls. Luefer abschalten (es laeuft weder ein Charger noch der Inverter)
  battery.controlFans();
  
  Serial.print("Check...  ; failureCount: ");
  Serial.println(failureCount);

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }
  
  boolean isBatOn = battery.isOn();
  int limit;
  if(isBatOn) {
    limit = SOC_LIMIT;
  } else {
    //v.0.9.9.36 ist die Batterie aus, dann muss um SOC_HYST (z.B. 5%) hoehere Ladung zur Verfuegung stehen
    limit = SOC_LIMIT + SOC_HYST;
  }
  //a) Teste State-Of-Charge
  boolean stop = false;
  String message = "";
  if (soc < limit) {
    message = "State of charge below ";
    message += limit;
    message += "%";
    stop = true;
  }
  //b) Ist jetzt noch kein Stopflag aktiv, teste die einzelnen Zellspannungen
  if (!stop) {
    int limit = LOW_VOLTAGE_MILLIS;   
    if(!isBatOn) {
      limit += 100; //Hysterese beachten: Bei Netzbetrieb (Batterie im Leerlauf, also mit hoeherer Zellspannung) ist Grenze um 100mV hoeher
    }
    for (int k = 0; k < 8; k++) {
      if (cv[k] < limit) {
        message = "Undervoltage cell: ";
        message += k;
        stop = true;
        break;
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
  //ab v.0.9.9.28 NTPClient mit Zeit
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  int hours = timeClient.getHours();
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  Serial.print(hours);
  Serial.print(":");
  Serial.print(mins);
  Serial.print(":");
  Serial.println(secs);
  datetime = timeClient.getFormattedDate();
  if (debug) {
      wc.sendClients(datetime);
  }
  //ab v.0.9.9.29 zwischen 20Uhr und 5Uhr morgens Batterie schalten; Vorraussetzung (0.9.9.31!!!): stop (statt nur SOC_LIMIT) beruecksichtigen)
  if(!stop) {
    if(hours>=20 || hours < 5) {
      if(!nacht) {
        if(!isBatOn) { 
            wc.sendClients(datetime);
            starteBatterie("Batteriezeit");    
        }
        nacht = true;    
      } 
  
    } else {
      if(nacht) {
        nacht = false;
        if(isBatOn) {
            starteNetzvorrang("Schalte wieder auf Netz zurück");    
        } else {
            wc.sendClients(datetime);
        }
      }
    }
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
