#define _XTAL_FREQ 8000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include "lcd.h";
#include <pic16f877a.h>
#include "USRT.h"
#include <stdio.h>
#include <stdlib.h> 
#define delay for(i=0;i<=1000;i++)
//#define rs RC0
//#define rw RC1
//#define e RC2

#define irsensor1 RB0
#define irsensor2 RB1

int count1,count2,total;
char s[55];

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

unsigned char sec,min,hour,date,month,year;
unsigned int adc();
void rtc_int();
void rtc_start();
void rtc_stop();
void rtc_ack();
void rtc_nak();
void rtc_res();
void rtc_send(unsigned char a);
void rtc_send_byte(unsigned char addr,unsigned char data);
unsigned char rtc_read();
unsigned char rtc_read_byte(unsigned char addr);
void waitmssp();
unsigned char convup(unsigned char bcd);
unsigned char convd(unsigned char bcd);


void count(){
    
      if(irsensor1==1){      
            count1++;
            __delay_ms(1000);
            }    
      if(irsensor2==1){
           count2++; 
           __delay_ms(1000);
            }
      
      total = count1-count2;
              
      }

void display(){
        
        
       if(total >= 0 && total <=55){
        
            Lcd_Clear();
            sprintf(s, "IN = %d", count1);
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(s);
            sprintf(s, "OUT = %d", count2);
            Lcd_Set_Cursor(1,10);
            Lcd_Write_String(s);
            sprintf(s, "Total = %d", total);
            Lcd_Set_Cursor(2,5);
            Lcd_Write_String(s);
            __delay_ms(1000);
            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Time:  ");
            Lcd_Write_Char(convup(hour));
            Lcd_Write_Char(convd(hour));
            Lcd_Write_Char(':');
            Lcd_Write_Char(convup(min));
            Lcd_Write_Char(convd(min));
            Lcd_Write_Char(':');
            Lcd_Write_Char(convup(sec));
            Lcd_Write_Char(convd(sec));
            
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String("Date:  ");
            Lcd_Write_Char(convup(date));
            Lcd_Write_Char(convd(date));
            Lcd_Write_Char(':');
            Lcd_Write_Char(convup(month));
            Lcd_Write_Char(convd(month));
            Lcd_Write_Char(':');
            Lcd_Write_Char(convup(year));
            Lcd_Write_Char(convd(year));
            __delay_ms(1000);
            
       }
    
}

void main()
{
  TRISB = 0xFF;
  TRISD = 0x00;
  
  PORTB=0x00;
  PORTC=0x00;
  PORTD=0x00;
    
  nRBPU = 0;
  UART_Init(9600);
  
  Lcd_Init();
  rtc_int();
  
  Lcd_Set_Cursor(1,1);
  Lcd_Write_String("Embedded System");
  __delay_ms(1000);
  Lcd_Set_Cursor(2,1);
  Lcd_Write_String("Vehical Counter");
  __delay_ms(1000);
 
  Lcd_Clear();
  for(int n;n<15;n++){
           Lcd_Set_Cursor(1,3);
            Lcd_Write_String("Update Count"); 
            Lcd_Set_Cursor(2,n);
            Lcd_Write_String(".");
              __delay_ms(55);    
       }
  
  while(1){
      
           sec  =rtc_read_byte(0x00);
           min  =rtc_read_byte(0x01);
           hour =rtc_read_byte(0x02);
           date =rtc_read_byte(0x04);
           month=rtc_read_byte(0x05);
           year =rtc_read_byte(0x06);
      
            
    
            if(total >= 0 && total <=55 && irsensor1==1){
                          count();
                          UART_Write(0x00);
                          display();            
              }
            if(total > 0 && total <=55 && irsensor2==1){
                          count();
                          UART_Write(0x01);
                          display();         
              }
            else{
                display();  
            }
            
     }
  
}

void rtc_int()
{
    TRISC3=TRISC4=1;
    SSPCON=0x28;
    SSPADD= (((11059200/4)/100)-1);
}
 
void waitmssp()
{
    while(!SSPIF); // SSPIF is zero while TXion is progress    
    SSPIF=0;
}
 
void rtc_send(unsigned char a)
{
    SSPBUF=a;
    waitmssp();
    while(ACKSTAT);
}   
 
void rtc_send_byte(unsigned char addr,unsigned char data)
{
    rtc_start();
    rtc_send(0xD0);
    rtc_send(addr);
    rtc_send(data);
    rtc_stop();
}
 
unsigned char rtc_read()
{
    RCEN=1;
    waitmssp();
    return SSPBUF;
}   
    
unsigned char rtc_read_byte(unsigned char addr)
{
    unsigned char rec;
L:  rtc_res();
    SSPBUF=0xD0;
    waitmssp();
    while(ACKSTAT){goto L;}
    rtc_send(addr);
    rtc_res();
    rtc_send(0xD1);
    rec=rtc_read();
    rtc_nak();
    rtc_stop();
    return rec;
}
    
    
void rtc_start()
{
    SEN=1;
    waitmssp();
}
 
void rtc_stop()
{
    PEN=1;
    waitmssp();
}
 
void rtc_res()
{
    RSEN=1;
    waitmssp();
}
 
void rtc_ack()
{
    ACKDT=0;
    ACKEN=1;
    waitmssp();
}
 
void rtc_nak()
{
    ACKDT=1;
    ACKEN=1;
    waitmssp();
}
 
unsigned char convup(unsigned char bcd)
{ 
    return ((bcd>>4)+48);
}
 
unsigned char convd(unsigned char bcd)
{ 
    return ((bcd&0x0F)+48);
}


unsigned int adc()
{
    unsigned int adcval;
    
    ADCON1=0xc0;                    //right justified
    ADCON0=0x85;                    //adc on, fosc/64
    while(GO_nDONE);                //wait until conversion is finished
    adcval=((ADRESH<<8)|(ADRESL));    //store the result
    adcval=(adcval/3)-1;
        
    return adcval;  
}
