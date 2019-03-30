 #include "charger.h"
#include "global.h"

/**
 * An-/Ausschalten der Solarcharger S1 und S2.
 * Erfolgt die Schaltung manuell, dann wird beim
 * Anschalten ein Sperrflag gesetzt, welcher ein
 * automatisches Ausschalten verhindert ( Der 
 * Charger MUSS manuell wieder ausgeschaltet werden)
 */
void Charger::toggleCharger(uint8_t nr, bool onOff, bool override) {
  bool isOn = isChargerOn(nr);
  if(isOn != onOff) { //nur, wenn es etwas zu schalten gibt
    if(isOn) {
      disableCharger(nr, override);
    } else {     
      enableCharger(nr, override);
    }
    if(nr == 1) {
      s1_switched = millis();
    } else {
      s2_switched = millis();
    }
  }
  Serial.printf("SMA::toggleCharger(%d, %d, %d); s1override: %d, s2override: %d, isOn: %s\n", nr, onOff, override, s1override, s2override, isOn?"true":"false");
}

bool Charger::isChargerOn(uint8_t nr) {
  if(nr == 1) {
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
  return (!digitalRead(RELAY_S1) && !digitalRead(RELAY_3)) || !digitalRead(RELAY_S2);
}

void Charger::enableCharger(byte nr, bool override) {
  if(nr == 1) {
      if(override) {
        s1override = true;
      }
      digitalWrite(RELAY_S1, LOW); //schaltet nur Power an, Charger bleibt aus
      delay(1000);
      digitalWrite(RELAY_3, LOW); //schaltet Charger ueber Remote an (oeffnet R3)
      digitalWrite(LED_S1, HIGH);
  } else {
      if(override) {
        s2override = true;
      }    
      digitalWrite(RELAY_S2, LOW);
      digitalWrite(LED_S2, HIGH);
  }
  wc.updateUi(NULL, true);
}

void Charger::disableCharger(uint8_t nr, bool override) {
  if(nr == 1) {
      if(override) {
        s1override = false;
      }
      if(override || !s1override) {
        digitalWrite(RELAY_3, HIGH); //schaltet R3 nur noch über Remote aus (schliesst CO-NC von R3)
        //digitalWrite(RELAY_S1, HIGH);        
        digitalWrite(LED_S1, LOW);
      }  
  } else {
      if(override) {
        s2override = false;
      }    
      if(override || !s2override) {
        digitalWrite(RELAY_S2, HIGH);
        digitalWrite(LED_S2, LOW);
      } 
  }
  wc.updateUi(NULL, true);
}

/**
 * Achtung: millis() beginnt bei 0 mit Start des Arduino!          
 */
void Charger::checkOnIncome(float netto) {

    /**
     * Ist der Charger2 aus UND ist der letzte Schaltvorgang
     * mehr als 30s her UND gibt es einen Energieüberschuss von 
     * mindestens 200W, dann aktiviere S2.  
     * 
     * Wird nichts eingespeist, dann stoppe den Charger.
     */
    unsigned long now = millis();
    unsigned long s2Last = now - s2_switched;
    if(s2_switched == -1 || s2Last > s2MinRestMillis) {
      if(!isChargerOn(2)) {
        if (netto > 300) {
          Serial.println("Aktiviere Solarcharger 2");
          toggleCharger(2,true,false);
          s2_countBeforeOff = -1;
          netto-=600;
        } 
      } else if(netto < -400 && !s2override) {
          if(s2_countBeforeOff < smaMeasurementsBeforSwitchoff) {
            s2_countBeforeOff++; 
          } else {        
            Serial.println("Deaktiviere Solarcharger 2");
            toggleCharger(2,false,false);
            netto+=600; //0.9.9.55 frei werdende Energie für Berechnung S1 anpassen
          }
      }
    }

    /**
     * Ist der Charger1 aus UND ist der letzte Schaltvorgang
     * mehr als 60s her UND gibt es einen Energieüberschuss von 
     * mindestens 400W, dann aktiviere S2.  
     * 
     * Wird nichts eingespeist, dann stoppe den Charger.
     */     
    unsigned long s1Last = now - s1_switched;
    if(s1_switched == -1 || s1Last > s1MinRestMillis) {     
      if(!isChargerOn(1)) {
        if(netto > 400){         
          //Serial.println("Aktiviere Solarcharger 1");
          toggleCharger(1,true,false);
          s1_countBeforeOff = -1;      
        }
      } else if(netto < -300 && !s1override) {        
          if(s1_countBeforeOff < smaMeasurementsBeforSwitchoff) {
            s1_countBeforeOff++;
          } else {
            //Serial.println("Deaktiviere Solarcharger 1");
            toggleCharger(1,false,false);
          }
      }
    }  

    /*
    TODO:
    - GPIO05/GND mit Blau/weiss HLG600B verbinden
    - ledcWrite(GPIO05, dutyCycle); //dutyCycle=0..255 ??
    - S2 Relais kann staendig an sein
    - s2MinRestMillis wird nicht mehr benötigt.
    - S2 könnte zeitgesteuert morgens an- und abends ausgeschaltet werden
    Angefangen wird immer mit S2/GPIO05, hier muss eine Tabelle
    von Dutycycle zu Watt her (gibt es evtl. schon)

    - Wenn S2 voll ausgesteuert und netto>0, dann S2 auf 0 und
      S1 einschalten. s1MinRestMillis pruefen.
    */

   if(debug2) {
     String m1 = "; Netto: ";
     m1+=netto;
     m1+="; now: ";
     m1+=now;
     m1+="; s1Last: ";
     m1+=s1Last;
     m1+="; s1_switched: ";
     m1+=s1_switched;
     m1+="; s1_countBeforeOff: ";
     m1+=s1_countBeforeOff;
     m1+="; s1override: ";
     m1+=s1override;       
     m1+="; s2Last: ";
     m1+=s2Last;
     m1+="; s2_switched: ";
     m1+=s2_switched;
     m1+="; s2_countBeforeOff: ";
     m1+=s2_countBeforeOff;
     m1+="; s2override: ";
     m1+=s2override;
     wc.sendClients(m1);
   }
}

//liefere die Millisekunden, die der Charger schon laeuft
int Charger::getRunningMillis(uint8_t nr) {
   if(!isChargerOn(nr)) {
      return 0;
   }
   if(nr == 1) {
      return millis() - s1_switched; 
   } else {
      return millis() - s2_switched;  
   }
}
