#include "SBMS.h"

#include "global.h"
#include <Arduino.h>

const long pwrs[] = { 1, 91, 8281, 753571 }; // 91^0, 91^1, 91^2, 91^3

const unsigned char dec[91] = {
  '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<',
  '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
  'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c',
  'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}'
};

/**
 * Dekodierung der vom SBMS120 kommenden Werte
 */
long SBMS::dcmp(int loc, int size,  const char* sbms, int sbmsLen) {
  if(loc > sbmsLen) {
    return 0;
  }
  long rv = 0;
  for (int i=0; i < size; i++){
    int ind = loc + size - 1 - i;
    char c = sbms[ind];
    int offc = SBMS::char_off(c);
    rv += ( offc * pwrs[i] );
  }
  return rv;
}

unsigned int SBMS::char_off(char c) {
  for(int i = 0; i < 91; i++) {
      if(dec[i] == c) {
        return i;
      }
    
  }
  return 0;
}

bool SBMS::read() {

   sbmsAnalyzer=0;

  /**
     Solange etwas empfangen wird (data gefuellt) sollte ausgewertet werden.
     Wenn aber der Timeout zuschlaegt, dann fuehrt das Lesen der nicht empfangenen
     Werte, dazu, soc und battery.cv[] zurueckzusetzen, woraufhin der naechste Lauf der
     Interruptmethode isrHandler(..) dazu, dass die Status-LED auf rot schaltet.
     Gleichzeitig ist es nicht mehr möglich, auf Batterie zu wechseln.

     Ist die Batterie gerade aktiv, wird das Relais wieder zurückgeschaltet (normal connected)
  */
  long now = millis();
  if (( now - lastReceivedMillis ) > 4000 && ( now -lastChecked ) > 2000) { //Verarbeitung hoechstens alle 4 Sekunden, Versuch nur alle 2 Sekunden ( SBMS aktualisiert nun alle 5s )

      sbmsAnalyzer=1;
      
      lastChecked = now;
    
      int len;
      if (testFixed) {
        //data = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
        //data = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
        //data = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
        //data = "#$7%XS$*GOGRGTGPGOGRGOGP*]##-##9##E#####################%N(";
        //data = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";
        data = testData;   
        len = data.length(); 

        sbmsAnalyzer=2;
        
      } else {

        sbmsAnalyzer=3;

        int index;
        const int STRING_BUFFER_SIZE = 70; //Max. Laenge der Pakete oben ist 61
        char stringBuffer[STRING_BUFFER_SIZE];
        if(serialSBMS.available()>61) {
          sbmsAnalyzer=4;
          while(serialSBMS.available()>0) {
            //bis zum nächsten String-Start ( \0 ) vorlesen
            char c = serialSBMS.read();
            if(c == '\0') break;
          }                 
        }
        sbmsAnalyzer=5; 
        while (serialSBMS.available() > 0) {            
            char c = serialSBMS.read();
            if(c >= 32 && index < STRING_BUFFER_SIZE - 1) {
              stringBuffer[index++] = c;
            } else {
              if(debugSbms) {
                Serial.print(F("Empfangenes String-Endezeichen / aktueller Index: "));
                Serial.print((int)c);
                Serial.print(F(" / "));
                Serial.println(index);
              }
              if(index > 0) {
                stringBuffer[index] = '\0';
                index=0;
              }
              break;
            }       
        }
        sbmsAnalyzer=6;
        data = String(stringBuffer);
        len = data.length(); 
        /*
        int ct = serialSBMS.available();
        if(ct > 80) {
          ct+=61; //max. obere Grenze, um Lockup zu verhindern
          while(serialSBMS.available() && ct>0) {
            serialSBMS.read();
            ct--;
          }
          return false;
        }
        if (serialSBMS.available()) {
          data = serialSBMS.readString();      
        }
        data.trim();
        len = data.length();
        
        if(debugSbms) {
          Serial.print(F("Empfangener SBMS-String: "));
          Serial.println(data);
          Serial.print(F("Laenge; "));
          Serial.println(len);
        }
        */
        
      }
      sbmsAnalyzer=7;
      if(len == 0) {
        return false;
      }

      yield();
    
      if (debugSbms && len > 0) {
        Serial.print(">");
        Serial.print(data);
        Serial.println("<");
        Serial.print("Length 'data': ");
        Serial.println(len);
      };

      if(len<50) {
        if(debugSbms) {
          String errData = String((char*)0);
          errData.reserve(100);
          errData+="Fehler, SBMS-Daten scheinen zu kurz zu sein: (len: ";
          errData+=len;
          errData+="); Inhalt: ";
          errData+=data;        
          wc.sendClients(errData.c_str());
        }
        sbmsAnalyzer=8;
        return false;
      }

      //Wert soc zurücksetzen (Wichtig, wenn mehrere Male nichts gelesen wird, also data.length=0,dann muss erst der failureCount hochgehen und nachher und schliesslich der Fehlermodus aktiviert werden (Batteriesperre)
      soc = 0;

      //Werte nun ans Webfrontend schicken
      wc.updateUi(); //ab 0.9.9.22 wird data per JSon uebermittelt

      const char* txt = data.c_str();

      sbmsAnalyzer=10;

      if(debugSbms) {
          String outString = String((char*)0);
          outString.reserve(100);
          outString += "\nSOC: ";
          if (len >= 8) {
            soc = sbms.dcmp(6, 2, txt, len);
            outString += soc;
            outString += "(Limit:";
            outString += socLimit;
            outString += "); ";
          }
          if (len >= 24) {
            for (int k = 0; k < 8; k++) {
              int loc = k * 2 + 8;
              battery.cv[k] = sbms.dcmp(loc, 2, txt, len);
              outString += "cv";
              outString += ( k + 1 );
              outString += "[";
              outString += battery.cv[k];
              outString += "], ";
            }
          }
          if (len >=26) {
            //((dcmp(24,2,data)/10)-45)
            temp = (sbms.dcmp(24, 2, txt, len)/10)-45;
          }
          Serial.print(outString);
          Serial.print(F("; StopBattery: "));
          Serial.println(inverter.stopBattery);   
      } else {
          sbmsAnalyzer=11;
          if (len >= 8) {
            soc = sbms.dcmp(6, 2, txt, len);
          }
          sbmsAnalyzer=12;
          if (len >= 24) {
            for (int k = 0; k < 8; k++) {
              int loc = k * 2 + 8;
              battery.cv[k] = sbms.dcmp(loc, 2, txt, len);
            }
          }
          sbmsAnalyzer=13;
          if (len >=26) {
            temp = (sbms.dcmp(24, 2, txt, len)/10)-45;
          }               
      }
      sbmsAnalyzer=14;
      yield();

      //Timeoutcounter nur zuruecksetzen, wenn etwas empfangen wurde
      lastReceivedMillis = millis();

      sbmsAnalyzer=15;
      return true;
  
  }
   sbmsAnalyzer=16;
  return false;
}

void SBMS::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("SBMS.lastReceivedMillis (in Sekunden): "));
  Serial.println(utils.secondsSince(lastReceivedMillis));  
  Serial.print(F("SBMS.testFixed: "));
  Serial.println(testFixed);  
}
