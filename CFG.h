#ifndef CFG_H
#define CFG_H

class CFG {
  private:
        //SPIFFS
        char* _webUser;
        char* _webPass;
        StaticJsonDocument<1024> doc;
  public:
        void load();
        void init();
        bool save();
        void print();
        void set(const String& keyVal);
        String getValue(String data, char separator, int index);
        char* webUser();
        char* webPass();
};

#endif
