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

  /**
     Solange etwas empfangen wird (sread gefuellt) sollte ausgewertet werden.
     Wenn aber der Timeout zuschlaegt, dann fuehrt das Lesen der nicht empfangenen
     Werte, dazu, soc und cv[] zurueckzusetzen, woraufhin der naechste Lauf der
     Interruptmethode isrHandler(..) dazu, dass die Status-LED auf rot schaltet.
     Gleichzeitig ist es nicht mehr möglich, auf Batterie zu wechseln.

     Ist die Batterie gerade aktiv, wird das Relais wieder zurückgeschaltet (normal connected)
  */
  long now = millis();
  if (( now - lastReceivedMillis ) > 3000) { //Verarbeitung hoechstens alle 3 Sekunden
    
      String sread;
      if (testFixed) {
        //sread = "5+'/,D$+HNGpGtGuGkH9H5HD+J##-#$'#####&##################$|(";
        //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
        //sread = "5+'0GT$,I+GvG|H#GnH[HUHs+T##-##|##%##(##################%{*";
        //sread = "#$7%XS$*GOGRGTGPGOGRGOGP*]##-##9##E#####################%N(";
        //sread = "#$87%K$*GDGGGPGDG2GLGLGL*m##-##:##@#####################%N(";
        sread = testData;    
      } else {

        /*
        static int index;
        const int STRING_BUFFER_SIZE = 70; //Max. Laenge der Pakete oben ist 61
        char stringBuffer[STRING_BUFFER_SIZE];
        while (Serial1.available() > 0) {            
            char c = Serial1.read();
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
        sread = String(stringBuffer);
        */
        while (Serial1.available()) {
          sread = Serial1.readString();      
        }
        
      }
    
      yield();
      sread.trim();
      int len = sread.length();

    
      if (debugSbms && len > 0) {
        Serial.print(".____");
        Serial.print(sread);
        Serial.println("____.");
        Serial.print("Length 'sread': ");
        Serial.println(len);
      };

      if(len<50) {
        if(debugSbms) {
          String errData = String((char*)0);
          errData.reserve(100);
          errData+="Fehler, SBMS-Daten scheinen zu kurz zu sein: (len: ";
          errData+=len;
          errData+="); Inhalt: ";
          errData+=sread;        
          wc.sendClients(errData.c_str());
          return false;
        }
      }

      //Wert soc zurücksetzen (Wichtig, wenn mehrere Male nichts gelesen wird, also sread.length=0,dann muss erst der failureCount
      // hochgehen und nachher und schliesslich der Fehlermodus aktiviert werden (Batteriesperre)
      soc = 0;

      //Werte nun lesen
      if (len > 0) {

        sbmsData = sread;
        wc.updateUi(); //ab 0.9.9.22 wird data per JSon uebermittelt

        const char* txt = sread.c_str();

        String outString = "\nSOC: ";
        if (len >= 8) {
          soc = sbms.dcmp(6, 2, txt, len);
          outString += soc;
          outString += " ( Limit: ";
          outString += socLimit;
          outString += " ) \n";
        }
        if (len >= 24) {
          for (int k = 0; k < 8; k++) {
            int loc = k * 2 + 8;
            cv[k] = sbms.dcmp(loc, 2, txt, len);

            outString += "\ncv";
            outString += ( k + 1 );
            outString += ": ";
            outString += cv[k];
          }
        }
        if (len >=26) {
          //((dcmp(24,2,data)/10)-45)
          temp = (sbms.dcmp(24, 2, txt, len)/10)-45;
        }


/*
 * TODO PV2 in A ermitteln
 * 
  var SOC = dcmp(6,2,data);
  soc = sbms.dcmp(6, 2, txt, len);

  var eA="##lh###v1---$v2---empty-%v1&2-#+#y#$1u#y##";
  x1 1..8
  var enA=dcmp(x1*6,6,eA);
*/

        
        yield();

        if (debugSbms) {
          Serial.println(outString);
          Serial.print(F("StopBattery: "));
          Serial.println(inverter.stopBattery);
          Serial.println(F("_______________________________________"));
        }

        //Timeoutcounter nur zuruecksetzen, wenn etwas empfangen wurde
        lastReceivedMillis = millis();
        return true;
      }
  }
  return false;
}

void SBMS::print() {
  Serial.print(F("SBMS.lastReceivedMillis: "));
  Serial.println(lastReceivedMillis);  
  Serial.print(F("SBMS.testFixed: "));
  Serial.println(testFixed);  
}
