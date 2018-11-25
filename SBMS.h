#ifndef SBMS_H
#define SBMS_H

class SBMS {

  private:
  
  unsigned int char_off(char c);

  public:
  
  long dcmp(int loc, int size,  const char* sbms, int sbmsLen);
  
};

#endif
