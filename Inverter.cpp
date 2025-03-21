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
    msg = F("Kann Netzvorrang nicht starten, da schon aktiv :: ");
    msg += reason;
    msg += '\n';
  }
  Serial.println(msg);
  if (msg.length() > 0) {    
     logs.append(msg);
     wc.sendClients(msg.c_str(), true);
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
  if(!battery.enabled) {
    msg = F("Kann Batterie nicht starten, da battery.enabled==false");
    Serial.println(msg);
    logs.append(msg);
    wc.sendClients(msg.c_str());
    return false;
  }
  if (!stopBattery) {
    if (digitalRead(RELAY_PIN) == LOW) {
      //0.9.9.6 Batteriemodus erst 'erlauben', wenn ESP32>1Min laeuft (um SW-Updates zu erlauben)
      if(!controller.isUpForSeconds(60)) {
        msg = F("Bevor der Batteriemodus aktiviert werden kann, muss der Controller mindestends 60s laufen");
        Serial.println(msg);
        logs.append(msg);
        wc.sendClients(msg.c_str());        
        return false;
      }
      digitalWrite(RELAY_PIN, HIGH); //OFF, d.h. Batterie aktiv
      wc.sendClients("Batterie -> An, Netz -> Aus", true);
      msg += F("Starte Batterie :: ");
      msg += reason;
      msg += '\n';
    }
    Serial.println(msg);
    if (msg.length() > 0) {  
       logs.append(msg);
       wc.sendClients(msg.c_str(), true);
    }    
    return true; //lief schon oder wurde aktiviert
  } else {
    msg = F("Kann Batterie nicht starten, da Stopflag aktiv :: ");
    msg += reason;
    msg += '\n';
  }
  Serial.println(msg);
  if (msg.length() > 0) {  
     logs.append(msg);
     wc.sendClients(msg.c_str(), true);
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

void Inverter::toggleNacht() {
  nacht = !nacht;
}

/**
 * Pruefe die Werte fuer SOC (StateOfCharge) und
 * CellVoltages (cv)
 */
void Inverter::check()  {

  if (debugInverter) {
    Serial.println(F("Inverter::check()"));
  }

  //Zur weiteren Pruefung sollte die soc vorliegen
  if (battery.soc < 0) return; //die Main-Loop sollte erstmal Werte lesen

  long now = millis();
  if (( now - lastCheckedMillis ) < tenSeconds) { //Pruefung hoechstens alle 10 Sekunden
    return;
  }
  lastCheckedMillis = now;
    
  //v. 0.9.9.40 ggfls. Luefer abschalten (es laeuft weder ein Charger noch der Inverter)
  luefter.check();
  yield();

  if (testFixed) {
    return; //keine Auswertung, wenn Testwerte
  }

  boolean badBattery = battery.checkCellVoltages();
  yield();
  
  if (badBattery) {

      if (!stopBattery) {
        if (debugInverter) {
          Serial.println(F("Stopflag was set, stopping inverter..."));
        }
      }
      stopBattery = true; 
      starteNetzvorrang("Interrupt(NZV); (CellVoltages)");
      setRed();
    
  } else {

    //Hier sollte nicht die Batterie gestartet, sondern nur freigeschaltet werden!!!
    stopBattery = false;
    setGreen();

  }
  //1.0.1 Timeclient muss nicht wieder aktualisiert werden (kann HTTPClient stoeren)  
  int day = timeClient.getDay();
  int hours = timeClient.getHours();
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  datetime = timeClient.getFormattedTime();
  if (debugInverter) {
      Serial.print(F("Time: "));
      Serial.print(hours);
      Serial.print(":");
      Serial.print(mins);
      Serial.print(":");
      Serial.println(secs);    
      wc.sendClients(datetime.c_str());
      Serial.print(F("Flag 'badBattery': "));
      Serial.println(badBattery);
      Serial.print(F("Flag 'nacht': "));
      Serial.println(nacht);
      Serial.print(F("Flag 'dauerbetrieb': "));
      Serial.println(dauerbetrieb);      
  }

  if(!badBattery) { 

      //ab v.0.9.9.29 zwischen 19Uhr und 9Uhr morgens Batterie schalten; Vorraussetzung (0.9.9.31!!!): stop (statt nur socLimit) beruecksichtigen)
      //ab 3.0.14 t1 Batteriestartzeit, t2 Batterieendezeit
      if(hours>=t1 || hours < t2) { 
          if(!nacht && !battery.isOn() && battery.isReady2Activate()) { //das 'nacht'-Flag verhindert, dass mehrfach versucht wird, auf Batterie umzuschalten; war die Umschaltung erfolgreich, ist nacht==true
              //3.0.14 Umschaltung auf Batterie  nur zwischen 18...24Uhr
              if(hours >= t1) {
                String msg = F("Batteriezeit ab ");
                msg+=t1;
                msg+= F("Uhr: ");
                msg+=hours;
                msg+=":";
                msg+mins;
                nacht = starteBatterie(msg);                         
                //Laeuft der Charger noch, sollte er nun gestoppt werden
                if(charger.isOn()) {
                  charger.stop();
                }
              }
          } 
      } else {
          //3.0.14 Umschaltung auf Netz nur nach t2 (z.B. 9 Uhr), aber VOR t1 (z.B. 19 Uhr)
          if(!dauerbetrieb && (hours >= t2 && hours < t1)) { //3.0.9
            nacht = false;
            if(battery.isOn()) {              
                  String msg = F("Schalte ab ");
                  msg+=t2;
                  msg+= F(" Uhr auf Netzversorgung");
                  msg+=battery.soc;
                  starteNetzvorrang(msg);                    
            }
          }
      }

  }
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

void Inverter::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Inverter.nacht: "));
  Serial.println(nacht);
  Serial.print(F("Batteriestartzeit: "));
  Serial.println(t1);  
  Serial.print(F("Batterieendezeit: "));
  Serial.println(t2);    
}
