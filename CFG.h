#ifndef CFG_H
#define CFG_H

class CFG {
  private:
        //SPIFFS
        char* _webUser;
        char* _webPass;
        void init();
  public:
        void load();
        bool save();
        void print();
        void set(const String& keyVal);
        void set(const char* key, const char* val);
        String getValue(String data, char separator, int index);
        char* webUser();
        char* webPass();
};

#endif
