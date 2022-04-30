//LCD16x2_4bit.h
#ifndef LCD16X2_4BIT_H_
#define LCD16X2_4BIT_H_
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define LCD_DPRT PORTB
#define LCD_DDDR DDRB
#define LCD_RS 0
#define LCD_EN 1

void lcdcommand(unsigned char cmnd); 
void lcddata(unsigned char data); //lcd data receive function
void lcdinit(); //lcd initiallize function
void lcd_gotoxy(unsigned char x, unsigned char y); //cursor shift function
void lcd_print(char *str); //lcd display
void lcd_clear(); /lcd clear display

#endif 

// LCD16x2_4bit.c
#include "LCD16x2_4bit.h"
void lcdcommand(unsigned char cmnd)
{
   LCD_DPRT = (LCD_DPRT & 0x0f)|(cmnd & 0xf0);      //SEND COMMAND TO DATA PORT
   LCD_DPRT &= ~ (1<<LCD_RS);                  // RS = 0 FOR COMMAND 
   LCD_DPRT |= (1<<LCD_EN);                  // EN = 1 FOR H TO L PULSE 
   _delay_us(1);                           // WAIT FOR MAKE ENABLE WIDE 
   LCD_DPRT &= ~(1<<LCD_EN);                  // EN = 0 FOR H TO L PULSE
   _delay_us(100);                           //WAIT FOR MAKE ENABLE WIDE 
   LCD_DPRT = (LCD_DPRT & 0x0f)|(cmnd << 4);      // SEND COMMAND TO DATA PORT 
   LCD_DPRT |= (1<<LCD_EN);                  // EN = 1 FOR H TO L PULSE 
   _delay_us(1);                           // WAIT FOR MAKE ENABLE WIDE 
   LCD_DPRT &= ~(1<<LCD_EN);                  // EN = 0 FOR H TO L PULSE
   _delay_ms(2);                           // WAIT FOR MAKE ENABLE WIDE 
}


void lcddata(unsigned char data)
{
   LCD_DPRT = (LCD_DPRT & 0x0f)|(data & 0xf0);      // SEND DATA TO DATA PORT 
   LCD_DPRT |= (1<<LCD_RS);                  // MAKE RS = 1 FOR DATA 
   LCD_DPRT |= (1<<LCD_EN);                  // EN=0 FOR H TO L PULSE 
   _delay_us(1);                           // WAIT FOR MAKE ENABLE WIDE 
   LCD_DPRT &= ~(1<<LCD_EN);                  // EN = 0 FOR H TO L PULSE 
   _delay_us(100);                           // WAIT FOR MAKE ENABLE WIDE 
   LCD_DPRT = (LCD_DPRT & 0x0f)|(data << 4);      
   LCD_DPRT |= (1<<LCD_EN);                  // EN=0 FOR H TO L PULSE
   _delay_us(1);                           // WAIT FOR MAKE ENABLE WIDE
   LCD_DPRT &= ~(1<<LCD_EN);                  //EN = 0 FOR H TO L PULSE
   _delay_ms(2);                           // WAIT FOR MAKE ENABLE WIDE
}


void lcdinit()
{
   LCD_DDDR = 0xFF;
   _delay_ms(200);                           // WAIT FOR SOME TIME 
   lcdcommand(0x33);
   lcdcommand(0x32);                        // SEND $32 FOR INIT OT 0X02 
   lcdcommand(0x28);                        // INIT. LCD 2 LINE, 5 X 7 MATRIX 
   lcdcommand(0x0C);                        // DISPLAY ON CURSOR ON 
   lcdcommand(0x01);                        // LCD CLEAR 
   _delay_ms(2);
   lcdcommand(0x82);                        // SHIFT CURSOR TO WRITE 
}

void lcd_gotoxy(unsigned char x, unsigned char y) //cursor shift function
{
   unsigned char firstcharadd[]={0x80, 0xC0};
   lcdcommand(firstcharadd[y] + x);
}

void lcd_print(char *str) //data print function
{
   unsigned char i=0;
   while (str[i] |= 0)
   {
      lcddata(str[i]);
      i++;
   }
}

void lcd_clear()//clear function
{
   lcdcommand(0x01);
   _delay_ms(2);
}

#include <avr/io.h> //기본 입출력 및 avr 레이블 정의 헤더파일
#include <stdlib.h> //문자열 데이터 처리를 위한 헤더파일
#include <stdio.h> //기본 입출력 함수 이용을 위한 헤더파일
#include "LCD16x2_4bit.h" //LCD 디스플레이를 위한 사용자정의 헤더파일
#define DHT11_PIN 6 //DHT11 핀을 6으로 정의
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum; //온습도센서의 데이터를 받기위한 변수
int tem=0; //조건에 사용할 변수 초기화

void Request()            //DHT11신호를 받기위한 송신 함수
{
   DDRD |= (1<<DHT11_PIN); //출력으로 지정
   PORTD &= ~(1<<DHT11_PIN);  //LOW신호 송신
   _delay_ms(20);        //20ms 대기
   PORTD |= (1<<DHT11_PIN);   //H신호 송신
}

void Response()            //DHT11 신호를 받기위한 수신 함수
{
   DDRD &= ~(1<<DHT11_PIN); //입력으로 지정
   while(PIND & (1<<DHT11_PIN)); //신호를 H받기 까지
   while((PIND & (1<<DHT11_PIN))==0);//신호를 L 받기 까지
   while(PIND & (1<<DHT11_PIN)); //신호를 H 받기 까지
}

uint8_t Receive_data()         //DHT11 신호를 데이터로 받아옴
{   
   for (int q=0; q<8; q++) //DHT11신호 8bit로 호출
   {
      while((PIND & (1<<DHT11_PIN)) == 0); //송신된 비트가 H,L 판단
      _delay_us(30); //30 ms 대기
      if(PIND & (1<<DHT11_PIN))//D의 신호가 H이면, 배열값에 1추가, 아니면 0 추가
      c = (c<<1)|(0x01);   
      else         
      c = (c<<1);
      while(PIND & (1<<DHT11_PIN)); //D핀의 신호가 H가 될 때까지,
   }
   return c;
}
int main(void) //메인 함수
{   char data[5];
   lcdinit();         //LCD 초기화
   lcd_clear();         // Clear LCD 
   lcd_gotoxy(0,0);      //커서 원위치
   lcd_print("Humidity =");// 습도 값 출력
   lcd_gotoxy(0,1);//2행으로 이동
   lcd_print("Temp = ");//온도값 출력
   DDRE |= (1<< 0);//신호 출력을 위한 E 출력 지정
    PORTE &= ~(1<<0);   //초기 L신호 송신
   _delay_ms(100); //100ms 대기
    while(1) 
   {  Request();      //데이터 스타트 비트 전송
      Response();      //데이터 값 수신
      I_RH=Receive_data();   //I_RH 8bit 저장
      D_RH=Receive_data();   //D_RH 8bit 저장
      I_Temp=Receive_data();   //I_TEMP 8bit 저장
      D_Temp=Receive_data();   //D_TEMP 8bit 저장
      CheckSum=Receive_data();//Check sum 8bit 저장
      if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum) //오류 체크값 확인
      {  lcd_gotoxy(0,0);
         lcd_print("Error"); } //오류 출력
      else //온도, 습도 출력
      {  itoa(I_RH,data,10); //습도 출력
         lcd_gotoxy(11,0);
         lcd_print(data);
         lcd_print(".");
         itoa(D_RH,data,10);
         lcd_print(data);
         lcd_print("%");
         itoa(I_Temp,data,10); //온도출력
         lcd_gotoxy(6,1);
         lcd_print(data);
         lcd_print(".");
             tem=atoi(data); //온도값을 정수형으로 저장

         itoa(D_Temp,data,10); 
         lcd_print(data);
         lcddata(0xDF);
         lcd_print("C ");
         

         itoa(CheckSum,data,10);
         lcd_print(data);
         lcd_print(" ");

         if (tem >= 30) //온도가 30도 이상이면,
         {DDRE |= (1<< 0); //모터 on
             PORTE |= (1<< 0);}   /* set to high pin */ 
         
          else //30 미만이면,
         {DDRE |= (1<< 0); //모터 off
             PORTE &= ~(1<<0);}   /* set to high pin */ 
     }
   _delay_ms(500); //500 ms 대기
   }   
}