#include "global.h"

void SMA::init() {

  // Multicast
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  Serial.println("\nUDP init fertig!\n");

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::reset() {
  udp.stop();
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  wc.sendClients("UDP reinitialized");
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

    bezug = strtol(wlb, NULL, 16)/10.0; //in Watt
    lieferung = strtol(wll, NULL, 16)/10.0; //in Watt

    float netto = lieferung - bezug;

    //v. 0.9.9.38 charger-Klasse regelt Aktivierung
    charger.checkOnIncome(netto);
   
  } else {
      //Wird 60s kein udp Paket des Energymeters gelesen, dann initialisiere WiFi-Reconnect
      long now = millis();
      long lastUdp = now - lastUdpRead;
      if(lastUdp > 60000 && (now - lastUdpNotification) > 10000) {
          lastUdpNotification = now;
          String msg = "Last WiFi UPD-Packet read ";
          msg += lastUdp;
          msg += "ms ago";
          Serial.println(msg);
          wc.sendClients(msg);
          /*Serial.println("Restarting WiFi, last successfull sma udp packet more than 60s ago...");
          wc.sendClients("Restarting WiFi, last successfull sma udp packet more than 60s ago...");
          delay(500);
          lastUdpRead = millis();
          myWifi.reconnect();
          */
          reset();
      }
  }
}
