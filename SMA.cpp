#include "global.h"

#define PACKETSIZE 600

void SMA::init() {
   
  if(udp.listenMulticast(IPAddress(239,12,255,254), portMulti)) {
      Serial.println("UDP connected");
      udp.onPacket([&](AsyncUDPPacket packet) {
          eval(&packet);
      });
  }  
  
  Serial.println(F("\nUDP init fertig!\n"));

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::reset() {

    if(debugSma) {
        Serial.print(F("udp.close() and listenMulticast(239,12,255,254)"));
    } 

    initialized = false;
    udp.close();
    
    if(udp.listenMulticast(IPAddress(239,12,255,254), portMulti)) {
        Serial.println(F("UDP connected"));
        udp.onPacket([&](AsyncUDPPacket packet) {
            eval(&packet);
        });
    }    
    initialized = true;
}

void SMA::eval(AsyncUDPPacket* packet) {
  
  int packetSize = packet->length();
  if(packetSize == PACKETSIZE) {

      long now = millis();
      if(debugSma) {
        Serial.print(F("Received 600Byte UDP packet successfully after "));
        Serial.print(now - lastUdpRead);
        Serial.println(F("ms"));
      }    
      
      lastUdpRead = now;
   
      uint8_t* buf = packet->data();
      
      for (std::size_t i = 0; i != packetSize; ++i) {
        hex[2*i  ] = lookup[ buf[i] >> 4  ];
        hex[2*i+1] = lookup[ buf[i] & 0xF ];
      }
  
      for(int v=0; v<8; v++) {
          //Summe Wirkleistung Bezug, 64...71
          wlb[v]=hex[v+64];
          //Summe Wirkleistung Lieferung, 104...111          
          wll[v]=hex[v+104];
      }
  
      float bezug = strtol(wlb, NULL, 16)/10.0; //in Watt
      float lieferung = strtol(wll, NULL, 16)/10.0; //in Watt
      netto = lieferung - bezug;

      if(debugSma) {
        Serial.print(F("Neuer Wert fuer Nettoertrag (sma.udp): "));
        Serial.println(netto);
      }

      //0.9.9.99 alle verfuegbaren Werte DIREKT an den/die Clients senden
      if(fastResponse) {
        StaticJsonDocument<32> doc; 
        doc["n"]=netto;
        char jsonChar[32];
        serializeJson(doc, jsonChar);
        String str(jsonChar);
        wc.sendClients(str.c_str());
      }

      //0.9.9.99 die Auswertung sollte nicht mehr Timergesteuert laufen, sondern immer dann, wenn ein neues UDP-Paket vorliegt
      hasNewPckt = true;
  
  } else {

     if(debugSma) {
        Serial.print(F("Received packet of size (sma.udp): "));
        Serial.println(packetSize);
      }

      long now = millis();
      long lastUdp = now - lastUdpRead;
      
      //0.9.9.91 Manchmal bricht der Wifistack zusammen, dann kann nur ein Reconnect desselben helfen.
      if(lastUdp > 600000) {
        Serial.println(F("Das letzte UDP-Paket wurde vor mehr als 10Min empfangen, restarte Wifi jetzt..."));
        myWifi.reconnect();
        return;
      }

      if(lastUdp > 120000 && (now - lastUdpReset) > 10000) {         
          yield();
          reset();
          lastUdpReset = now;
          yield();
          String msg((char *)0);
          msg.reserve(42);
          msg += F("UDP reinitialized: Last packet read ");
          msg += lastUdp;
          msg += F("ms ago; ReconnectCount: ");
          msg += udpResets;
          Serial.println(msg);
          wc.sendClients(msg.c_str());
          udpResets++;
      }
  }
}

bool SMA::hasNewPacket() {
  return hasNewPckt;
}

void SMA::resetNewPacket() {
  hasNewPckt = false;
}
