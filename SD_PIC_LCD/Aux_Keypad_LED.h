#include <24FV16KM202.h>
#device ICSP=3
#use delay(internal=32MHz)

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOBROWNOUT               //No brownout reset
#FUSES BORV_LOW                 //Brown-out Reset set to lowest voltage

void disableKeypad(void);
void enableKeypad(void);

void clearLine(void);
void lcdShift();
void systemInit(void);
void menuRefresh(int);

void readFrequency(void);
void enterState(int state);
void sweepRefresh(int selected);
void waitForResponse(void);

void screenClearedDisplay(void);
