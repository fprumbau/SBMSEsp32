#include "global.h"

#define BUTTONACTION "Buttonaction"
/**
   Netzvorrang starten
*/
void Inverter::starteNetzvorrang(String reason) {
  String msg((char *)0);
  msg.reserve(60);
  if (digitalRead(RELAY_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW); //ON, d.h. Netzvorrang aktiv
    wc.sendClients("Netz -> An, Batterie -> Aus");
    msg += F("Starte Netzvorrang :: ");
    msg += reason;
    msg += '\n';
  } else {
    if (debugInverter)  msg = F("Kann Netzvorrang nicht starten, da schon aktiv\n");
  }
  if (msg.length() > 0) {
    if (debugInverter) {
      Serial.println(msg);
    }
    wc.sendClients(msg.c_str());
  }
}

/**
   Batteriebetrieb starten;
   gibt true zurück, wenn start erfolgreich ODER
   wenn Batteriemodus schon aktiv.
*/
bool Inverter::starteBatterie(String reason) {
  String msg((char *)0);
  msg.reserve(60);
  if (!stopBattery) {
    if (digitalRead(RELAY_PIN) == LOW) {
      //0.9.9.6 Batteriemodus erst 'erlauben', wenn ESP32>1Min laeuft (um SW-Updates zu erlauben)
      if(!controller.isUpForSeconds(60)) {
        msg = F("Bevor der Batteriemodus aktiviert werden kann, muss der Controller mindestends 60s laufen");
        Serial.println(msg);
        if(debugInverter) {
          wc.sendClients(msg.c_str());
        }
        return false;
      }
      digitalWrite(RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
      wc.sendClients("Batterie -> An, Netz -> Aus");
      msg += F("Starte Batterie :: ");
      msg += reason;
      msg += '\n';
    }
    return true; //lief schon oder wurde aktiviert
  } else {
    msg = F("Kann Batterie nicht starten, da Stopflag aktiv\n");
  }
  if (msg.length() > 0) {
    if (debugInverter) {      
       wc.sendClients(msg.c_str());
    }
    Serial.println(msg);
  }
  return false;
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

  if (( millis() - lastCheckedMillis ) < checkMillis) { //Pruefung hoechstens alle 10 Sekunden
    return;
  }
  lastCheckedMillis = millis();
    
  //v. 0.9.9.38 faellt die CV zu sehr, sollte S1 (Charger 1) aktiviert werden
  battery.checkCellVoltages();
  yield();

  //v. 0.9.9.40 ggfls. Luefer abschalten (es laeuft weder ein Charger noch der Inverter)
  battery.controlFans();
  yield();
  
  if(debugInverter) {
    Serial.print(F("Check...  ; failureCount: "));
    Serial.println(failureCount);
  }

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }
  
  boolean isBatOn = battery.isOn();
  /*int limit;
  if(isBatOn) {
    limit = socLimit;
  } else {
    //v.0.9.9.36 ist die Batterie aus, dann muss um SOC_HYST (z.B. 5%) hoehere Ladung zur Verfuegung stehen
    limit = socLimit + SOC_HYST;
  }*/
  //a) Teste State-Of-Charge
  boolean stop = false;
  String message((char *)0);
  message.reserve(128);
  /*if (soc < limit) {
    message = F("State of charge below ");
    message += limit;
    message += "%";
    stop = true;
  }*/
  //b) Ist jetzt noch kein Stopflag aktiv, teste die einzelnen Zellspannungen
  if (!stop) {
    int limit = LOW_VOLTAGE_MILLIS;   
    if(!isBatOn) {
      limit += CV_HYST; //Hysterese beachten: Bei Netzbetrieb (Batterie im Leerlauf, also mit hoeherer Zellspannung) ist Grenze um CV_HYST mV hoeher
    }
    for (int k = 0; k < 8; k++) {
      if (cv[k] > 0 && cv[k] < limit) {
        message = F("Undervoltage cell: ");
        message += k;
        stop = true;
        break;
      }    
    }
  }
  if (stop) {
    failureCount++;
    if (failureCount < errLimit) { //einen 'Fehlversuch' ignorieren.
      if (debugInverter) {
        Serial.print(F("Error found, waiting until failureCount reaches "));
        Serial.print(errLimit);
        Serial.print("; now: ");
        Serial.println(failureCount);
      }
    } else {
      if (!stopBattery) {
        if (debugInverter) {
          Serial.println(F("Error limit reached, stopping inverter..."));
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
  //ab v.0.9.9.28 NTPClient mit Zeit; 0.9.9.97 nicht EWIG warten (10 Versuche)
  int ct = 0;
  bool timeUpdate = false;
  while(!(timeUpdate = timeClient.update())) {
    yield();
    if(ct++>10) { 
      lastStatusMsg = F("Loese timeUpdate-Loop (break)");
      break;
    }
    timeClient.forceUpdate();
  }
  
  int day = timeClient.getDay();
  int hours = timeClient.getHours();
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  datetime = timeClient.getFormattedDate();
  if (debugInverter && timeUpdate) {
      Serial.print(hours);
      Serial.print(":");
      Serial.print(mins);
      Serial.print(":");
      Serial.println(secs);    
      wc.sendClients(datetime.c_str());
  }
  //ab v.0.9.9.29 zwischen 19Uhr und 9Uhr morgens Batterie schalten; Vorraussetzung (0.9.9.31!!!): stop (statt nur socLimit) beruecksichtigen)
  if(!stop) {
    if(hours>=18 || hours < 9) {
      if(!nacht) {
        if(!isBatOn) { 
            if(timeUpdate) wc.sendClients(datetime.c_str());
            nacht = starteBatterie(F("Batteriezeit"));    
        }  
      } 
    } else {
      if(nacht) {
        nacht = false;
        if(isBatOn) {
            starteNetzvorrang(F("Schalte wieder auf Netz zurück"));    
        } else { 
            if(timeUpdate) wc.sendClients(datetime.c_str());
        }
      }
    }
  }
  /*
  //v. 0.9.9.58 jeden Morgen um 6Uhr neu starten (nur volle Minute)
  if(hours == 6 && mins == 0 && dayOfMonthLastRestart != day)  {
     dayOfMonthLastRestart = day; //nur EINEN Restart am Tag 
     wc.sendClients(F("Restarte ESP um 6:00 Uhr").c_str());
     delay(200); //Warte, bis Nachricht verschickt ist
     ESP.restart();
  }*/
  //0.9.9.90 Steigt die Erzeugung (PV2) über 10A, dann ist der Tag sicher nicht fern :-); siehe SBMS.cpp
}

/**
 * Toggeln des Inverter-/Batteriebetriebs
 */
void Inverter::handleButtonPressed() {

    Serial.println(F("Button pressed"));

    bool relayStatus = digitalRead(RELAY_PIN);
    if (relayStatus == HIGH) {
      // starte Netzvorrang
      starteNetzvorrang(BUTTONACTION);
    } else {
      if (!stopBattery) {
        starteBatterie(BUTTONACTION);
      } else {
        if (debugInverter) {
          Serial.println(F("ON, kann Netzvorrang nicht abschalten (Stop wegen SOC oder Low Voltage)"));
        }
      }
    }  
}
