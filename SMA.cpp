#include "global.h"

void SMA::init() {

  IPAddress ip = myWifi.localIP();

  // Multicast
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  Serial.println("\nUDP init fertig!\n");

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::read() {

  if(!initialized) {
    return;
  }
  
  int packetSize = udp.parsePacket();
  if(packetSize) {

    lastUdpRead = millis(); //Dient als WiFi isAlive
 
    // read the packet into buf and get the senders IP addr and port number
    udp.read(buf,packetSize);
   
    static char const lookup[] = "0123456789ABCDEF";

    for (std::size_t i = 0; i != packetSize; ++i) {
      hex[2*i  ] = lookup[ buf[i] >> 4  ];
      hex[2*i+1] = lookup[ buf[i] & 0xF ];
    }

    //Summe Wirkleistung Bezug, 64...71
    for(int v=0; v<8; v++) {
          wlb[v]=hex[v+64];
    }
    //Summe Wirkleistung Lieferung, 104...111
    for(int v=0; v<8; v++) {
          wll[v]=hex[v+104];
    }

    float bezug = strtol(wlb, NULL, 16)/10.0; //in Watt
    float lieferung = strtol(wll, NULL, 16)/10.0; //in Watt

    if(debug2) {
      String msg;
      msg = "\nWirkleistung (Bezug/Lieferung): ";
      msg += bezug;
      msg += " / ";
      msg += lieferung;
      wc.sendClients(msg, false);
      Serial.println(msg);
    }

    /**
     * Ist der Charger2 aus UND ist der letzte Schaltvorgang
     * mehr als 60s her UND gibt es einen Energieüberschuss von 
     * mindestens 200W, dann aktiviere S2.  
     * 
     * Wird nichts eingespeist, dann stoppe den Charger.
     */
    if(( millis() - s2_switched ) > 60000) {
      if(!isChargerOn(2)) {
        if (lieferung > 200) {
          Serial.println("Aktiviere Solarcharger 2");
          toggleCharger(2,true,false);
          s2_countBeforeOff = -1;
        } 
      } else if(lieferung <= 0 && bezug > 100 && !s2override) {
          if(s2_countBeforeOff < smaMeasurementsBeforSwitchoff) {
            s2_countBeforeOff++; 
          } else {        
            Serial.println("Deaktiviere Solarcharger 2");
            toggleCharger(2,false,false);
          }
      }
    }

    /**
     * Ist der Charger1 aus UND ist der letzte Schaltvorgang
     * mehr als 60s her UND gibt es einen Energieüberschuss von 
     * mindestens 600W, dann aktiviere S2.  
     * 
     * Wird nichts eingespeist, dann stoppe den Charger.
     */
    if(( millis() - s1_switched ) > 60000) {
      if(!isChargerOn(1)) {
        if(lieferung > 600){
          Serial.println("Aktiviere Solarcharger 1");
          toggleCharger(1,true,false);
          s1_countBeforeOff = -1;
        }
      } else if(lieferung <= 0 && bezug > 300 && !s1override) {
          if(s1_countBeforeOff < smaMeasurementsBeforSwitchoff) {
            s1_countBeforeOff++;
          } else {
            Serial.println("Deaktiviere Solarcharger 1");
            toggleCharger(1,false,false);
          }
      }
    }
   
  } else {
      //Wird 10s kein udp Paket des Energymeters gelesen, dann initialisiere WiFi-Reconnect
      if((millis() - lastUdpRead) > 15000) {
          Serial.println("Restarting WiFi, last successfull sma udp packet more than 15s ago...");
          wc.sendClients("Restarting WiFi, last successfull sma udp packet more than 15s ago...", false);
          delay(500);
          lastUdpRead = millis();
          myWifi.reconnect();
      }
  }
}

/**
 * An-/Ausschalten der Solarcharger S1 und S2.
 * Erfolgt die Schaltung manuell, dann wird beim
 * Anschalten ein Sperrflag gesetzt, welcher ein
 * automatisches Ausschalten verhindert ( Der 
 * Charger MUSS manuell wieder ausgeschaltet werden)
 */
void SMA::toggleCharger(byte nr, bool onOff, bool override) {
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

bool SMA::isChargerOn(byte nr) {
  if(nr == 1) {
    return !digitalRead(RELAY_S1);
  } else {
    return !digitalRead(RELAY_S2);
  }
}

void SMA::enableCharger(byte nr, bool override) {
  if(nr == 1) {
      if(override) {
        s1override = true;
      }
      digitalWrite(RELAY_S1, LOW);
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

void SMA::disableCharger(byte nr, bool override) {
  if(nr == 1) {
      if(override) {
        s1override = false;
      }
      if(override || !s1override) {
        digitalWrite(RELAY_S1, HIGH);
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
