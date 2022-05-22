#include "charger.h"
#include "global.h"

#define ONE_HOUR_MILLIS 3600000

/**
   An-/Ausschalten der Solarcharger S1 und S2.
*/
void Charger::toggleCharger(uint8_t nr, bool onOff, bool notify) {
  bool isOn = isChargerOn(nr);
  if (isOn != onOff) { //nur, wenn es etwas zu schalten gibt
    if (isOn) {
      disableCharger(nr, notify);
    } else {
        //3.0.8 Charger S1+S2 arbeiten zusammen, S1 (600W konstant) kann jetzt deaktiviert werden (s1Activated)
        if(nr == S1 && s1Activated) {
            enableCharger(nr, notify);
        } else {    
            enableCharger(nr, notify);
        }
    }
    if (nr == S1) {
      s1_switched = millis();
    } else {
      s2_switched = millis();
    }
  }
}

bool Charger::isChargerOn(uint8_t nr) {
  if (nr == 1) {
    //0.9.9.48 Remote ON/OFF berücksichtigen; ( R3 ist das einzige auf NC laufende Relais, darum muss nicht negierend gefragt werden )
    //HLG600 ist AN, wenn NC-CO geschlossen sind, das ist der Default, also HIGH zustand. Wird auf LOW gestellt, öffnet sich NC-CO und der Charger ist ON
    //Ergo ist das Ergebnis von digitalRead(RELAY_3) im Anfangsstadium HIGH -> Aus; Der Charger ist dann an, wenn RELAY_S1 LOW UND RELAY_3 LOW sind.
    //(alle Relais, also S1,S3,3 und 4 sind mit HIGH initialisiert, schalten also zum Start nicht);
    //Anders gesagt: HLG_600 verhaelt sich an RELAY_3 andersherum, ist aber auch anders angeschlossen, das hebt sich wieder auf!!!
    return !digitalRead(RELAY_S1) && !digitalRead(RELAY_3);
  } else {
    return !digitalRead(RELAY_S2);
  }
}

bool Charger::isOn() {
  return !digitalRead(RELAY_3) || !digitalRead(RELAY_S2);
}

//stop 
void Charger::stop() {
  disableCharger(0, 0);
  disableCharger(1, 0);
}

void Charger::enableCharger(byte nr, bool notify) {
  if (nr == 1) {
    digitalWrite(RELAY_3, LOW); //schaltet Charger ueber Remote an (oeffnet R3)
    digitalWrite(LED_S1, HIGH);
  } else {
    digitalWrite(RELAY_S2, LOW);
    digitalWrite(LED_S2, HIGH);
  }
  if (notify) {
    wc.updateUi(NULL, true);
  }
}

void Charger::disableCharger(uint8_t nr, bool notify) {
  if (nr == S1) {
      digitalWrite(RELAY_3, HIGH); //schaltet R3 nur noch über Remote aus (schliesst CO-NC von R3)
      digitalWrite(LED_S1, LOW);
  } else {
      digitalWrite(RELAY_S2, HIGH);
      digitalWrite(LED_S2, LOW);
  }
  if (notify) {
    wc.updateUi(NULL, true);
  }
}

/**
   Dem zugehoerigen Wert der Nettolieferung sollte
   ein Wert von 0...1023 zugeordnet werden.

   0    : 31 W
   100  : 84 W
   200  : 136 W
   300  : 187 W
   400  : 237 W
   500  : 287 W
   600  : 337 W
   700  : 386 W
   800  : 436 W
   900  : 484 W
   1000 : 533 W
   1023 : 546 W
*/
int Charger::calculateDc(float netto) {
  //0.9.9.63 Netto neu berechnen: Eine DC-Berechnung sollte auch den aktuellen Wert des Ladestroms mit inkludieren
  netto += (dutyCycle * 0.5);

  if (netto < 50) return 0;
  if (netto < 100) return 120;
  if (netto < 150) return 220;
  if (netto < 200) return 320;
  if (netto < 250) return 420;
  if (netto < 300) return 520;
  if (netto < 350) return 620;
  if (netto < 400) return 720;
  if (netto < 450) return 820;
  if (netto < 500) return 920;
  if (netto < 550) return 980;
  return 1023;
}

/**
   Achtung: millis() beginnt bei 0 mit Start des Arduino!

   Seit der letzten Boardaenderung werden ueber RELAY_S1 BEIDE
   Charger aktiviert und dann jeweils ueber RELAY_3 und RELAY_S2 remote
   aktiviert und deaktiviert.

   CHARGER1: HLG600A
   CHARGER2: HLG600B, laesst sich ueber GPIO05 mit einem PWM-Signal steuern

   Ziel: Charger2 nicht mehr ueber RELAY_S2 abzuschalten, sondern ueber GPIO05
         variabel zu steuern.

*/
void Charger::checkOnIncome() {

  bool s1on = isChargerOn(1);
  bool s2on = isChargerOn(2);

  unsigned long now = millis();

  if (debugCharger) {
    Serial.print(F("charger.checkOnIncome(): "));
    Serial.println(now);
  }

  //Sind ALLE Lademoeglichkeiten ausgeschoepft, dann versuche, den Tesla zu laden; TODO: tatsaechlichen Status des Wagens abfragen
  unsigned long nowOff = now + TESLA_LOAD_SWITCH_MIN_MILLIS; //da millis() bei ESP-Start von 0 anfaengt, muss er diesen Offset erhalten
  if ((nowOff - lastTeslaLoadSwitch) > TESLA_LOAD_SWITCH_MIN_MILLIS) {
    //Wenn mehr als 500W bleibt, dann versuchen, den Tesla zu laden
    if (s1on && s2on && dutyCycle == 1023 && netto > teslaChargeStartThreshold && !teslaCharging) {
      if (highIncomeCount >= 10 && teslaCtrlActive) {
        perry.wakeup();
        perry.startCharge();
        lastTeslaLoadSwitch = nowOff;
        teslaCharging = true;
        wc.sendClients("Es wurde versucht, den Ladevorgang des TESLA zu starten");
        highIncomeCount = 0;
      } else {
        highIncomeCount++;
      }
    } else if (netto < teslaChargeStopThreshold && teslaCharging && teslaCtrlActive) { //wenn mehr als |-1000W| aus dem Netz gezogen wird, dann versuchen, den Tesla auszuschalten
      if (lowIncomeCount >= 10) {
        perry.wakeup();
        perry.stopCharge();
        lastTeslaLoadSwitch = nowOff;
        teslaCharging = false;
        wc.sendClients("Es wurde versucht, den Ladevorgang des TESLA zu stoppen");
        lowIncomeCount = 0;
      } else {
        lowIncomeCount++;
      }
    }
  }
  yield();

  unsigned long s1Last = now - s1_switched;
  unsigned long s2Last = now - s2_switched;

  if(automatedCharging) {
       /**
         Ist Charger2 aus UND ist der letzte Schaltvorgang
         mehr als 60s (s1MinRestMilis) her UND gibt es einen Energieüberschuss von mindestens 600W,
         und ( v. 1.0.7 ) gibt es diesen Überschuss mindestens schon 1Minute (wait_excess_power_millis > 60000),
         dann aktiviere S2. Der neue Timout wait_excess_power_millis reduziert weiter den Gebrauch von relay_1
         unter Last.
    
         Wird nichts eingespeist, dann stoppe den Charger.
    
         ledcWrite(GPIO05, dutyCycle);  ;dutyCycle 0-1023
    
         0.9.9.71 Zuerst S1 bewerten
         s2MinRestMillis==60s
    
      */
      if(!s1on) {
        if (netto > 600) {
           s1_lowNettoMillis = -1; //Reset des LowNetto-Timers
        }             
      } else {
        if(netto > 0) {
          s1_lowNettoMillis = -1; //Reset des LowNetto-Timers
        }
      }
      if (s1_switched == -1 || s1Last > s1MinRestMillis) {
        if (!s1on) {
          //Läuft S2 noch nicht, dann wird S1 gestartet, wenn netto > 600, also der Max-Wert von S1
          if (!s2on) {
            if (netto > 600) {
              if (wait_excess_power_start_millis < 0) { //Ausreichend Energie vorhanden, Wartezeit starten
                    wait_excess_power_start_millis = now;
               }
              if ((now - wait_excess_power_start_millis) > 60000) {
                netto -= 600;
                if (debugRelais) {
                  wc.sendClients("Aktiviere Solarcharger 1 weil netto > 600 und s2 off", true);
                }
                s1on = true;
                toggleCharger(S1, true, true);
              }
            } else {
              wait_excess_power_start_millis = -1; //Zuwenig Energie, Wartezeit zuruecksetzen
            }
          } else {
            //S1 sollte gestartet werden, wenn der aktuelle Power von S2 + netto > 600W betragen.
            if ((netto + getS2Power()) > 600) {
              if (wait_excess_power_start_millis < 1) { //Ausreichend Energie vorhanden, Wartezeit starten
                wait_excess_power_start_millis = now;
              }
              if ((now - wait_excess_power_start_millis) > 60000) {
                s1on = true;
                netto -= 600;
                toggleCharger(S1, true, true);
                if (debugRelais) {
                   wc.sendClients("Aktiviere Solarcharger 1 weil netto+power(S2)>600; S2 wird jetzt mit netto-600W neu bewertet...", true);
                }
                return; //direkt nach Schalten von S1 nichts weiteres mehr machen
              }         
            } else {
              wait_excess_power_start_millis = -1; //Zuwenig Energie, Wartezeit zuruecksetzen
            }
          }
        } else if (netto < -300) {
            //1.0.12 analog S2
            if (s1_lowNettoMillis < 1) {
              s1_lowNettoMillis = now; //Start des LowNetto-Timers
            }
            if ( (now - s1_lowNettoMillis) > 60000) { //erst nach 30s negativen Energiebetrags Charger 1 abschalten
              if (debugRelais) {
                wc.sendClients("Deaktiviere Solarcharger 1, weil >30s ein negativer Ertrag ist", true);
              }
              netto += 600;
              s1on = false;
              wait_excess_power_start_millis = -1;
              toggleCharger(S1, false, true);
            } 
        }
      }
    
      /*
         Der Charger S2 ist selbstregelnd und kann von 1-21A geregelt werden.
         Er sollte erst per Relais abgeschaltet werden (0A), wenn länger als
         5 Minuten kein Stromüberschuss vorhanden ist, um Schaltzyklen zu verhindern.
      */
      if (netto > 0) {
        s2_lowNettoMillis = -1; //Reset des LowNetto-Timers
      }
      unsigned long s2Last = now - s2_switched;
      if (s2_switched == -1 || s2Last > s2MinRestMillis) {  
          if (!s2on) {
            if (netto > 30) {
              /*0.9.9.69 ERST pruefen, ob Inverter gerade auf Batterie laeuft
                // (und der Solarertrag groesser als 100W ist, da sonst abends bei z.B. netto = -400 eingeschaltet wird und dann +500 erzeugt wird, also netto>30 wird )
                // Die 100 entprechen dem, was Mama&Papa verbrauchen
                if(battery.isOn() && netto > 100) {
                  inverter.starteNetzvorrang(F("Deaktiviere Batteriemodus, weil Nettoertrag positiv (S2 Bewertung)"));
                  return;
                }*/
              if (debugRelais) {
                wc.sendClients("Aktiviere Solarcharger 2");
              }
              s2on = true;
              toggleCharger(S2, true, true);
              return; //Direkt nach Akivierung von Charger S2 nichts weiteres tun
            }
          } else {
            
            if (netto < -100) {
      
              //0.9.9.88, der Status netto sollte laenger als 5 Min kleiner 0 sein, sonst NICHT ausschalten
              if (s2_lowNettoMillis < 1) {
                s2_lowNettoMillis = now; //Start des LowNetto-Timers
              }
        
              if ( (now - s2_lowNettoMillis) > 300000) { //erst nach 5 Minuten negativen Energiebetrags Charger 2 abschalten
                if (debugRelais) {
                  wc.sendClients("Deaktiviere Solarcharger 2");
                }
                s2on = false;
                toggleCharger(S2, false, true);
              }
            }
        }
      }
      yield();
  }

  //0.9.9.1 laufen Charger S1 UND S2 seit 1h, dann schalte Batteriebetrieb ab ( 1.0.9 => revert )
  //if (s1on && s2on && getRunningMillis(S1) > ONE_HOUR_MILLIS && getRunningMillis(S2) > ONE_HOUR_MILLIS && battery.isOn()) {
  //  inverter.starteNetzvorrang(F("Deaktiviere Batteriemodus, weil S1 UND S2 schon mehr als eine Stunde laufen"));
  //}

  //Nun Ladelevel ueber GPIO5 einstellen.
  if (s2on) {
    dutyCycle = calculateDc(netto);
    ledcWrite(GPIO05, dutyCycle);
    //Grobe Naeherung: dutycycle von 500 entspricht 250W (0.5)
    if (debugCharger) {
      String ms = F("Regulating Dutycycle: ( netto / dutyCycle ) | ");
      ms += netto;
      ms += F(" / ");
      ms += dutyCycle;
      wc.sendClients(ms.c_str());
    }
  }

  if (debugCharger) {
    String m1((char *)0);
    m1.reserve(128);
    m1 += F("; Netto: ");
    m1 += netto;
    m1 += F("; now: ");
    m1 += now;
    m1 += F("; s1Last: ");
    m1 += s1Last;
    m1 += F("; s1_switched: ");
    m1 += s1_switched;
    m1 += F("; automatedCharging: ");
    m1 += automatedCharging;
    m1 += F("; s2Last: ");
    m1 += s2Last;
    m1 += F("; s2_switched: ");
    m1 += s2_switched;
    wc.sendClients(m1.c_str());
  }

  //0.9.9.99 diese Methode wird aus der Mainklasse nur angeseteuert, wenn ein neues UDP-Paket vom Energymanager vorliegt; hier wird
  //         das schon verarbeitete Paket ausgeflagged
  sma.resetNewPacket();
}

//liefere die Millisekunden, die der Charger schon laeuft
unsigned long Charger::getRunningMillis(uint8_t nr) {
  if (!isChargerOn(nr)) {
    return 0;
  }
  if (nr == 1) {
    return millis() - s1_switched;
  } else {
    return millis() - s2_switched;
  }
}

int Charger::getS2Power() {
  if (dutyCycle < 50) {
    return 0;
  } else if (dutyCycle < 100) {
    return 100;
  } else if (dutyCycle < 200) {
    return 150;
  } else if (dutyCycle < 300) {
    return 200;
  } else if (dutyCycle < 400) {
    return 250;
  } else if (dutyCycle < 500) {
    return 300;
  } else if (dutyCycle < 600) {
    return 350;
  } else if (dutyCycle < 700) {
    return 400;
  } else if (dutyCycle < 800) {
    return 450;
  } else if (dutyCycle < 900) {
    return 500;
  } else if (dutyCycle < 950) {
    return 550;
  } else {
    return 600;
  }
}

void Charger::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Charger.wait_excess_power_start_millis: "));
  long wait;
  if(wait_excess_power_start_millis > 0) {
    wait = millis() - wait_excess_power_start_millis;
  } else wait = wait_excess_power_start_millis;
  Serial.println(wait);
  Serial.print(F("Charger.isChargerOn(1): "));
  Serial.println(isChargerOn(1));
  Serial.print(F("Charger.isChargerOn(1): "));
  Serial.println(isChargerOn(2));
  Serial.print(F("Charger.s1_switched: "));
  Serial.println(s1_switched);
  Serial.print(F("Charger.s2_switched: "));
  Serial.println(s2_switched);
  Serial.print(F("Charger.s1_lowNettoMillis: "));
  if(s1_lowNettoMillis > 0) {
    wait = millis() - s1_lowNettoMillis;
  } else wait = s1_lowNettoMillis;
  Serial.println(wait);
  Serial.print(F("Charger.s2_lowNettoMillis: "));
  if(s2_lowNettoMillis > 0) {
    wait = millis() - s2_lowNettoMillis;
  } else wait = s2_lowNettoMillis;
  Serial.println(wait);
  Serial.print(F("Charger.automatedCharging: "));
  Serial.println(automatedCharging);
  Serial.print(F("Charger.s1Activated: "));
  Serial.println(s1Activated);
  Serial.print(F("Charger.dutyCycle: "));
  Serial.println(dutyCycle);
  Serial.print(F("Charger.getS2Power: "));
  Serial.println(getS2Power());
  Serial.print(F("Netto: "));
  Serial.println(netto);
}
