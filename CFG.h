#ifndef CFG_H
#define CFG_H

class CFG {
  private:
        bool _configRead=false;
        StaticJsonDocument<1024> doc;
        String getValue(String data, char separator, int index);
  public:
        bool load();
        void init();
        bool save();
        void set(const String& keyVal);
};

#endif
