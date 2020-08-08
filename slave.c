#define _XTAL_FREQ 8000000
#include <xc.h>
#include <pic16f877a.h>
#include "USRT.h"

#define output1 RB7
#define output2 RD7


// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG
void servoRotate0() //0 Degree
{
  unsigned int i;
  for(i=0;i<50;i++)
  {
    RB7 = 1;
    __delay_us(800);
    RB7 = 0;
    __delay_us(19200);
  }
}

void servoRotate90() //90 Degree
{
  unsigned int i;
  for(i=0;i<50;i++)
  {
    RB7 = 1;
    __delay_us(1500);
    RB7 = 0;
    __delay_us(18500);
  }
}

void servoRotate180() //180 Degree
{
  unsigned int i;
  for(i=0;i<50;i++)
  {
    RB7 = 1;
    __delay_us(2200);
    RB7 = 0;
    __delay_us(17800);
  }
}


void main()
{
    
  TRISB = 0x00; 
  TRISD = 0x00;
  
  PORTB=0x00;
  PORTD=0x00;
  
  UART_Init(9600);
   
  int data,x=1,y=1;
  int array[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
  PORTB=array[0];
  PORTD=array[0];
  
   do{
      output1=0;
      output2=0;
      
      if(UART_Data_Ready()){
          
          data=UART_Read();
          
          if(data==0x00){
             if(x>=0&&x<=9){ 
                PORTB=array[x++];
                output1=1;
                 __delay_ms(1000);
             }
             else{
                 x=0;
                 PORTD=array[0];
                 __delay_ms(1000);
             }
           
          }
         if(data==0x01){
              if(y>=0&&y<=9){
                PORTD=array[y++];
                output2=1;
                 __delay_ms(1000);
               
              }
              else{
                  y=0;
                  PORTD=array[0];
                  __delay_ms(1000);
              }
                 
          }
      }
      }while(1);    

}
