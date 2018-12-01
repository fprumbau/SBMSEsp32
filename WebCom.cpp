#include "WebCom.h"

WebCom::WebCom() { }

WebCom::WebCom(Vars& vars) {
  _vars = vars;
}

IPAddress WebCom::remoteIP(uint8_t num) {
  return _wsServer.remoteIP(num);
}

bool WebCom::sendTXT(uint8_t num, String& payload) {
  return _wsServer.sendTXT(num, payload);
}

/**
   Sende Daten zu allen über Websockets verbundenen
   Clients. Alles, was NICHT SBMS-Daten sind, also
   Fehler- bzw. Statusmeldungen MUSS mit einem '@'
   eingeleitet werden, sonst wird es von der Webseite
   falsch interpretiert und führt zu wilden Werten
   z.B. beim Batteriestatus.
*/
void WebCom::sendClients(String msg, bool data) {
  if (clientCount <= 0) {
    return;
  }
  _wsServerLastSend = millis();
  for (int m = 0; m < clientCount; m++) {
    uint8_t client = clients[m];
    if (_vars.debug) {
      Serial.printf("Sending client %u ( %u ) from %u clients\n", (m + 1), client, clientCount);
    }
    if (!data) {
      msg = "@ " + msg;
    }
    _wsServer.sendTXT(client, msg);
  }
}

void WebCom::loop() {
  _wsServer.loop();
}

void WebCom::begin(WebSocketServerEvent cbEvent) {
  _wsServer.onEvent(cbEvent);
  _wsServer.begin();
}
