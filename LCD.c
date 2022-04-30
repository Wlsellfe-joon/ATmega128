//함수의 원형 정의(lcd.h)#ifndef	__LCD_H__
#define	__LCD_H_

#define	MAX_LCD_STRING	0x40 //문자열의 최대 길이 정의

extern void	gen_E_strobe(void); //E strobe 신호 함수
extern void	wait_BusyFlag(void);//busy flag 함수
extern void	LCD_command(unsigned char data);//LCD명령어 함수
extern void	LCD_data_write(unsigned char data);//LCD데이터 함수
extern void	LCD_init(void);//LCD 초기화 함수
extern void	set_cursor(unsigned int row, unsigned int col) ; //커서 설정 함수
extern void	LCD_str_write(unsigned int row, unsigned int col, char *str);//커서 설정 함수
#endif
#include	<stdio.h> //sprintf출력을 위한 헤더파일
#include	<avr/io.h>
#define	F_CPU	8000000
#include	<util/delay.h>
#include	"lcd.h"
// 전역변수 선언
long 	a = 0, b = 0;
double 	x = 0., y = 0.;
//////////////////////////////////////////////////////////////
int	main(void)
{	// 지역 변수 선언
	char lcd_string[2][MAX_LCD_STRING];

	LCD_init();

	while(1){
		a = a + 1;
		b = b + 2;

		x += 0.1;
		y += 0.2;

		sprintf(lcd_string[0], "A=%-5ld B=%-5ld", a, b);
		LCD_str_write(0, 0, lcd_string[0]);

		sprintf(lcd_string[1], "X=%-5.1f Y=%-5.1f   ", x, y);
		LCD_str_write(1, 0, lcd_string[1]);
		_delay_ms(1000);
	}
	return 0;
}
#include	<avr/io.h>
#include	"lcd.h"
#define	RS	PD5	// LCD 문자디스플레이에 연결된 포트D 의 핀번호 
#define	RW	PD6
#define	E	PD7
void	gen_E_strobe(void)
{	volatile	int	i;
	PORTD |= 1<<E;		// E 신호를 High로
	for(i=0; i<10; i++);		// E 스트로브 신호를 일정기간 High로 유지
	PORTD &= ~(1<<E);		// E 신호를 Low로
}	
void	wait_BusyFlag(void)
{	volatile int 		i;
	unsigned char		bf;
	DDRC = 0x0;			// 포트C를 입력핀으로 설정
	PORTD = (PORTD & ~(1<<RS)) | 1<<RW; // RS <- Low, RW <- High
	do{
		PORTD |= 1<<E;		// E 신호를 High로
		for(i=0; i<10; i++);	// E 스트로브 신호를 일정기간 High로 유지
		bf = PINC & 1<<PC7;	// busy flag 읽어 냄
		PORTD &= ~(1<<E);	// E 신호를 Low로
	}while( bf );			// bf 값이 0이 아니면 busy, 0 일 때까지 반복
}
void	LCD_command(unsigned char data)
{	wait_BusyFlag();		// busy flag가 0될 때까지 대기
	DDRC = 0xFF;			// 포트C를 출력핀으로 설정
	PORTC = data;			// data 출력
	PORTD &= ~(1<<RS | 1<<RW);	// RS <- 0, RW <-0
	gen_E_strobe();		// E 스트로브 신호 만들기
}
void	LCD_data_write(unsigned char data)
{	wait_BusyFlag();
	DDRC = 0xFF;
	PORTC = data;
	PORTD = (PORTD | 1<<RS) & ~(1<<RW); // RS <- 1, RW <-0
	gen_E_strobe();
}

void	LCD_init(void)
{
	DDRD |= 1<<RS | 1<<RW | 1<<E;	// RS, RW, E 핀을 출력핀으로 설정
	
	PORTD &= ~(1<<RS | 1<<E | 1<<RW); 	// 초기에 RS, E, RW <- 0

	LCD_command(0x3C);
	LCD_command(0x02);
	LCD_command(0x01);
	LCD_command(0x06);
	LCD_command(0x0F);
}

void	set_cursor(unsigned int row, unsigned int col)
{
	LCD_command(0x80 + (row % 2) * 0x40 + (col % 0x40));
}

// 함수 정의 : row, col 위치에서 문자열 str 을 LCD에 출력시킨다.
void	LCD_str_write(unsigned int row, unsigned int col, char *str)
{
	int	i;
	
	set_cursor(row, col);
	for(i=0; (i+col < MAX_LCD_STRING) && (str[i] != '\0'); i++)
		LCD_data_write(str[i]);
}