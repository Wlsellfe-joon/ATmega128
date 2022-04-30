1. #include<avr/io.h>
 
#define S0PB5
#define S1PB6
#define S2PB7
 
#define D0PB0
#define D1PB1
#define D2PB2
#define D3PB3
#define D4PB4  // 레이블 정의
 
intmain(void)  // 메인 함수
{
DDRB = 1<<D0 | 1<<D1 | 1<<D2 | 1<<D3 | 1<<D4;  //PB0,PB1,PB2,PB3,PB4를 출력으로 지정
 
while(1){                      //계속 반복
if( !(PINB &(1<<S0)) )         // PINB5 bit 가 0이면 스위치 So ON
PORTB |= 1<<D0 | 1<<D1;   // So ON 이면, PB0, PB1 출력 = LED 0,1 켬
else
PORTB &= ~(1<<D0 | 1<<D1); // LED 0,1 OFF
if( !(PINB &(1<<S1)) )          // PINB6 bit가 0이면 스위치 S1 ON
PORTB |= 1<<D2;             // S1 ON 이면 LED 2 ON
else
PORTB &= ~(1<<D2);          //그렇지 않으면 LED 2 OFF
 
if( !(PINB &(1<<S2)) )         // PINB7 bit 가 0이면 스위치 S2 ON
PORTB |= 1<<D3 | 1<<D4;   // S2 ON 이면 LED 3,4 ON
else
PORTB &= ~(1<<D3 | 1<<D4);  // 그렇지 않으면 LED OFF
}
return 0;
}