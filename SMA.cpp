#include "SMA.h"

#include "Vars.h"

SMA::SMA(Vars& vars, WebCom& wc) {
  _vars = vars;
  _wc = wc;
}

void SMA::init(MyWifi& myWifi) {

  IPAddress ip = myWifi.localIP();

  // Multicast
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  _udp.beginMulticast(ipMulti, portMulti);
  Serial.println("\nUDP init fertig!\n");

  wlb[8]='\0';
  wll[8]='\0';
}

void SMA::read() {
  int packetSize = _udp.parsePacket();
  if(packetSize) {
 
    // read the packet into buf and get the senders IP addr and port number
    _udp.read(buf,packetSize);
   
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

    if (lieferung < 200) {
      _wc.sendClients("Aktiviere Solarlader 1, deaktiviere 2", false);
      
      digitalWrite(_vars.RELAY_S1, LOW);
      digitalWrite(_vars.RELAY_S2, HIGH);

      digitalWrite(_vars.LED_S1, HIGH);
      digitalWrite(_vars.LED_S2, LOW);
    } else {
      _wc.sendClients("Aktiviere Solarlader 2, deaktiviere 1", false);
      
      digitalWrite(_vars.RELAY_S1, HIGH);
      digitalWrite(_vars.RELAY_S2, LOW);

      digitalWrite(_vars.LED_S1, LOW);
      digitalWrite(_vars.LED_S2, HIGH);
    }

    Serial.print("\nWirkleistung (Bezug/Lieferung): ");
    Serial.print(bezug);
    Serial.print(" / ");
    Serial.println(lieferung);
   
  }
}
