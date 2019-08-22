#include "global.h"

#define PACKETSIZE 600

void SMA::init() {

  IPAddress ipMulti(239,12,255,254);     
  udp.beginMulticast(ipMulti, portMulti);
  
  Serial.println(F("\nUDP init fertig!\n"));

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::reset() {

    initialized = false;
    udp.stop();
    
    IPAddress ipMulti(239,12,255,254);   
    udp.beginMulticast(ipMulti, portMulti);
    wc.sendClients("UDP reinitialized");
    initialized = true;
}

bool SMA::read() {

  if(!initialized) {
    return false;
  }

  unsigned long now = millis();
  if ((now - lastUdpRead) < 3000) { 
    return false;
  }
  
  yield();
  int packetSize = udp.parsePacket();
  yield();
  if(packetSize == PACKETSIZE) {
    
      if(debugSma) {
        Serial.print(F("Received 600Byte UDP packet successfully after "));
        Serial.print(now - lastUdpRead);
        Serial.println(F("ms"));
      }    
      
      lastUdpRead = millis();
   
      // read the packet into buf and get the senders IP addr and port number
      udp.read(buf,PACKETSIZE);
      yield();
  
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
  
      return true;
      
  } else {

      long now = millis();
      long lastUdp = now - lastUdpRead;
      
      //0.9.9.91 Manchmal bricht der Wifistack zusammen, dann kann nur ein Restart desselben helfen.
      if(lastUdp > 3600000) {
        Serial.println(F("Das letzte UDP-Paket wurde vor mehr als einer Stunde empfangen, restarte Wifi jetzt..."));
        myWifi.reconnect();
        return false;
      }

      if(lastUdp > 120000 && (now - lastUdpReset) > 10000) {         
          yield();
          reset();
          lastUdpReset = now;
          yield();
          String msg((char *)0);
          msg.reserve(42);
          msg += F("Last WiFi UPD-Packet read ");
          msg += lastUdp;
          msg += F("ms ago");
          Serial.println(msg);
          wc.sendClients(msg.c_str());
      }
  }
  return false;
}
