#ifndef CFG_H
#define CFG_H

class CFG {
  private:
        bool _configRead=false;
        StaticJsonDocument<1024> doc;
  public:
        bool load();
        void init();
        bool save();
        void set(const String& keyVal);
        String getValue(String data, char separator, int index);
};

#endif
