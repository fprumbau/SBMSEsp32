#include "SBMS.h"

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
