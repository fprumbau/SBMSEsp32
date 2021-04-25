#ifndef DISPLAY_H
#define DISPLAY_H

#define SHOW_MSG_START_TICKS 10

class Display {
    private:
      char screen[82]="                                                                                \0";
      int offsets[3];
      int onStart[3];
      unsigned long lastVoltUpdate;
      void line2buff(const char* txt, int offset);
      void prepareLine(int line, int offset);
      void draw();
      
    public:
      void init();
      void update();
      void print();
};      
#endif
