#include "global.h"

void SMA::init() {

  // Multicast
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  Serial.println(F("\nUDP init fertig!\n"));

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::reset() {
  udp.stop();
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  wc.sendClients(F("UDP reinitialized"));
}

void SMA::ping() {
   Serial.println(F("Ping udp!"));
   udp.beginPacket(udp.remoteIP(), udp.remotePort());
   uint8_t buffer[8] = "pingupd";
   udp.write(buffer,7);
   udp.endPacket();
}

void SMA::read() {

  if(!initialized) {
    return;
  }

  //da der UDP-Empfang alle 5-10Minuten einmal zusammenbricht, jetzt ein Ping jede Minute
  long now = millis();
  if(now-lastPingUdp > 600000) {
    ping();
    lastPingUdp = now;
  }
  
  int packetSize = udp.parsePacket();
  yield();
  if(packetSize) {

    lastUdpRead = millis(); //Dient als WiFi isAlive
 
    // read the packet into buf and get the senders IP addr and port number
    udp.read(buf,packetSize);
    yield();
   
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

    bezug = strtol(wlb, NULL, 16)/10.0; //in Watt
    lieferung = strtol(wll, NULL, 16)/10.0; //in Watt

    float netto = lieferung - bezug;

    //v. 0.9.9.38 charger-Klasse regelt Aktivierung
    yield();
    charger.checkOnIncome(netto);
    yield();
  } else {
      //Wird 2 Minuten kein udp Paket des Energymeters gelesen, dann initialisiere WiFi-Reconnect
      long now = millis();
      long lastUdp = now - lastUdpRead;
      //wurde lastUdp millis kein Paket mehr empfangen UND liegt der letzte Resetversuch mind. 10s zurück
      if(lastUdp > 120000 && (now - lastUdpReset) > 10000) {
          yield();
          reset();
          lastUdpReset = now;
          yield();
          String msg((char *)0);
          msg.reserve(80);
          msg += F("Last WiFi UPD-Packet read ");
          msg += lastUdp;
          msg += F("ms ago");
          Serial.println(msg);
          wc.sendClients(msg);
      }
  }
}
