//#include <Lab 4 ECE 422.h>

#include <Aux_Keypad_LED.h>

//Already Declared in .h file
//#include <24FV16KM202.h>

#FUSES FRC_PLL

#use delay(clock = 32MHZ, internal = 8MHZ)
#use fast_io(B)

//Serial Port communication
#USE RS232(UART2, BAUD = 115200, PARITY = N, BITS = 8, STOP = 1, TIMEOUT = 500)

//DEFINE LCD PINS
#define LCD_ENABLE_PIN  PIN_A7
#define LCD_RS_PIN      PIN_B8
#define LCD_RW_PIN      PIN_B9
#define LCD_DATA4       PIN_B12
#define LCD_DATA5       PIN_B13
#define LCD_DATA6       PIN_B14
#define LCD_DATA7       PIN_B15
#define LED_PIN         PIN_A2

#define LCD_EXTENDED_NEWLINE //Include for enable 4 line LED

#include <LCD.C>
#include "kbd_mod_123A_int.c"

//Flags for Both interrupts
int1 serial_flag = 0;
int1 keypad_flag = 0;
char key;
char UARTKey;
int32 frequency = 0;

//UART Interrupt
#INT_RDA2
void isr_uart()
{
   UARTKey = getc();
   serial_flag = 1;
}

#INT_EXT0
void ist_ext()
{
   key = kbd_getc();
   keypad_flag = 1;
}

void lcd_display(char c)
{
   disable_interrupts(INT_EXT0);
   set_pullup(false);
   lcd_putc(c);
   kbd_init();
   clear_interrupt(INT_EXT0);
   enable_interrupts(INT_EXT0);
 
}

void lcd_clear()
{
   printf(lcd_putc, "\f"); //Clear Screen
}


int state = 1; 
int selected = 1;

CONST int BOTTOM = 4;
CONST int TOP = 1;
/*
STATE GUIDE:

   0 - MENU
*/

void main()
{
//----------------------DECLARATIONS AND INITIATIONS---------------------------//
   systemInit();

   printf("\n\rHello...\n\r");
   
   serial_flag = 0;
   keypad_flag = 0;
   
   unsigned int8 xPosition = 1;
   
   //Pressed indicates if a number has been pressed 
   boolean pressed = 0;
   //printf(lcd_putc , "Hi\nHi\nHi\n");
//----------------------INFINITE LOOP-----------------------------------//
   while(1)
   {
   
     if(keypad_flag)
     {
         disableKeypad();
         //lcd_putc(key);
         switch(state){
            case 1: //Main Menu State
               if(key == 'A') //UP KEY
               {
                  if(selected != TOP)
                     selected--;
                  menuRefresh(selected);
                  
               }
               else if(key == 'B') //Down KEY
               {
                  if(selected != BOTTOM)
                     selected++;
                  menuRefresh(selected);
               }
               else if(key == '#') //Enter Key
               {
                  state = state + selected;
                  enterState(state);
                  selected = 1;
               }
            break;
            case 2: //Single Sample State
              
               if(key == 'D') //Exit key pressed
               {
                  state = 1;
                  enterState(state);
                  frequency = 0;
                  break;
               }
               else if((key >= '0') && (key <= '9'))
               {
                  frequency = frequency * 10 + key - 48;
                  if(frequency > 200000)
                  {
                     frequency = frequency / 10;
                  }
               }
               else if(key == '*')
               {
                  frequency = frequency / 10;
               }
               else if(key == '#')
               {
                  
                  if(frequency > 0)
                  {
                     printf("F"); //Send Frequency
                     printf("%i", frequency); //Send to GUI
                     printf("\n"); //Print newline
                     

                     state = 1;
                     enterState(state);
                     frequency = 0;
                     break;
                  }
                  
               }
               
               readFrequency();
               
            break;
            case 3: //Sweep State
               
               if(key == 'A') //UP KEY
               {
                  if(selected != TOP)
                     selected--;
                  sweepRefresh(selected);
                  
               }
               else if(key == 'B') //Down KEY
               {
                  if(selected != 7) //Use 7 for max 
                     selected++;
                  sweepRefresh(selected);
               }
               else if(key == '#') //Enter Key We send the sweep here
               {
                  printf("S"); //Print two for sweep
                  printf("%i", selected); //Send type of sweep 1- 7
                  waitForResponse();
                  //Return to menu
                  state = 1;
                  enterState(state);
                  selected = 1;
                  
               }
               else if(key == 'D') //Exit key pressed
               {
                  state = 1;
                  selected = 1;
                  enterState(state);
                  break;
               }
               
            break;
            case 4: //Clear Screen
                  screenClearedDisplay();
                  state = 1;
                  selected = 1;
                  enterState(state);
                  break;
            break;
            case 5: //Full Cycle
               
            break;
         
         }
         
         enableKeypad();
         keypad_flag = 0;
     }
     else if(serial_flag) //If Serial Data is Received
     {
         serial_flag = 0;
     }

   }
}
//Clear Line
void clearLine(void)
{
   disable_interrupts(INT_EXT0);
   set_pullup(false);
   //Precondition: The cursor is set to the line that you want to clear
   //Postcondition: The line is clear but cursor is at x = 17 of whatever line was cleared
   //MUST MOVE CURSOR
   for(int i = 0; i <16;i++)
   {
      lcd_putc(' ');
   }
   
   //lcd_gotoxy(1,1);
   kbd_init();
   clear_interrupt(INT_EXT0);
   enable_interrupts(INT_EXT0);
}

//Cool Function
//Shifts the top line over
void lcdShift(void)
{
   disable_interrupts(INT_EXT0);
   set_pullup(false);
   //char temp1;
   char temp2;
   
   //int cursor = 1;
   
   lcd_gotoxy(1,1);
   
   for(int i = 1; i < 16; i++)
   {
      temp2 = lcd_getc(i+1,1);
      
      lcd_gotoxy(i,1);
      
      lcd_putc(temp2);
      //lcd_gotoxy(i,1);
      
   }
   
   lcd_gotoxy(16,1);
   lcd_putc(' ');
   lcd_gotoxy(16,1);
   
   kbd_init();
   clear_interrupt(INT_EXT0);
   enable_interrupts(INT_EXT0);
   
}

void disableKeypad()
{
   disable_interrupts(INT_EXT0);
   set_pullup(false);
}

void enableKeypad()
{
   kbd_init();
   clear_interrupt(INT_EXT0);
   enable_interrupts(INT_EXT0);
}

void enableInterrupts()
{

}
void systemInit(void)
{
   lcd_init();
   
   //lcd_cursor_on(TRUE);
   
   printf(lcd_putc, "Powering ON");
   delay_ms(500);
   printf(lcd_putc, "\f");
   
   menuRefresh(selected);
   
   kbd_init();
   ext_int_edge(L_TO_H);
   clear_interrupt(INT_EXT0);
   
   enable_interrupts(INT_EXT0);
   enable_interrupts(INT_RDA2);
   enable_interrupts(INTR_GLOBAL);
}

void menuRefresh(int selected)
{
   printf(lcd_putc, "\f"); //Clear Screen
   
   if(selected == 1)
   {
      printf(lcd_putc, "> ");
      printf(lcd_putc, "Single Sample\n");
      printf(lcd_putc, "Sweep\n");
      printf(lcd_putc, "Full Sweep\n");
      printf(lcd_putc, "Clear\n");
   }
   else if(selected == 2)
   {
      printf(lcd_putc, "Single Sample\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "Sweep\n");
      printf(lcd_putc, "Full Sweep\n");
      printf(lcd_putc, "Clear\n");
   }
   else if(selected == 3)
   {
      printf(lcd_putc, "Single Sample\n");
      printf(lcd_putc, "Sweep\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "Full Sweep\n");
      printf(lcd_putc, "Clear\n");
   }
   else if(selected == 4)
   {
      printf(lcd_putc, "Single Sample\n");
      printf(lcd_putc, "Sweep\n");
      printf(lcd_putc, "Full Sweep\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "Clear\n");
   }

}

void readFrequency()
{
   printf(lcd_putc, "\f"); //Clear Screen
   
   printf(lcd_putc, "Frequency:\n"); //Clear Screen
   
   if(frequency != 0)
   {
      printf(lcd_putc, "%i",frequency);
   }
}


void enterState(int state)
{
   switch(state)
   {
      case 1: //Mani Menu
         menuRefresh(selected);
         lcd_cursor_on(FALSE);
      break;
      case 2: //Single Sample
         readFrequency();
         lcd_cursor_on(TRUE);
      break;
      case 3: //Sweep
         selected = 1;
         sweepRefresh(selected);
      break;
      
   }
}

void sweepRefresh(int selected)
{
      printf(lcd_putc, "\f"); //Clear Screen
   
   if(selected == 1)
   {
      printf(lcd_putc, "> ");
      printf(lcd_putc, "1 - 200k Hz\n");
      printf(lcd_putc, "1 - 10 Hz\n");
      printf(lcd_putc, "10 - 100 Hz\n");
      printf(lcd_putc, "100 - 1k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, ">");
   }
   else if(selected == 2)
   {
      printf(lcd_putc, "1 - 200k Hz\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "1 - 10 Hz\n");
      printf(lcd_putc, "10 - 100 Hz\n");
      printf(lcd_putc, "100 - 1k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, ">");
   }
   else if(selected == 3)
   {
      printf(lcd_putc, "1 - 200k Hz\n");
      printf(lcd_putc, "1 - 10 Hz\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "10 - 100 Hz\n");
      printf(lcd_putc, "100 - 1k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, ">");
   }
   else if(selected == 4)
   {
      printf(lcd_putc, "1 - 200k Hz\n");
      printf(lcd_putc, "1 - 10 Hz\n");
      printf(lcd_putc, "10 - 100 Hz\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "100 - 1k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, ">");
   } //BACK HALF BELOW
   else if(selected == 5)
   {
      printf(lcd_putc, "> ");
      printf(lcd_putc, "1k - 10k Hz\n");
      printf(lcd_putc, "10k - 100k Hz\n");
      printf(lcd_putc, "100k - 200k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, "<");
   }
   else if(selected == 6)
   {
      printf(lcd_putc, "1k - 10k Hz\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "10k - 100k Hz\n");
      printf(lcd_putc, "100k - 200k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, "<");
   }
   else if(selected == 7)
   {
      printf(lcd_putc, "1k - 10k Hz\n");
      printf(lcd_putc, "10k - 100k Hz\n");
      printf(lcd_putc, "> ");
      printf(lcd_putc, "100k - 200k Hz\n");
      lcd_gotoxy(20, 4);
      printf(lcd_putc, "<");
   }
   
}

void waitForResponse()
{
   disableKeypad();
   
   while(serial_flag != 0)//While end is not done
   {
      
      if(UARTKey == '!')
      {
         lcd_clear();
         printf(lcd_putc, "System is Busy");
         serial_flag = 1;
      }
      else if(UARTKey == '#')
      {
         serial_flag = 1; //Data is Done
      }
      else
      {
         printf(lcd_putc, UARTKey);
      }
   }
   
   enableKeypad();
}

void screenClearedDisplay(void)
{
   disableKeypad();
   
   lcd_clear();
   printf(lcd_putc , "Terminal Clear");
   
   enableKeypad();
}
