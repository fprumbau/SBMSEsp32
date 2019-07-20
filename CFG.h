#ifndef CFG_H
#define CFG_H

class CFG {
  private:
        bool _configRead=false;
        StaticJsonDocument<1024> doc;
  public:
        bool load();
        bool save();
};

#endif
