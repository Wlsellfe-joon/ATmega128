#define	F_CPU	16000000UL //딜레이를 위한 매크로 정의
#include	<avr/io.h>
#include	<util/delay.h>
#define	PRESSED		1
#define	RELEASED		0
#define	DEBOUNCE_MS	1000 //시간지연을 위한 정의//원래 코드는 100ms였지만 실험을 위해 1000ms 즉, 1초로 변경하였다.
unsigned char digit[] = {0x88, 0xBE, 0xC4, 0xA4, 0xB2, 0xA1, 0x83, 0xBC, 0x80, 0xB0};
void	display_7segled(unsigned char led[], unsigned int number)
{	PORTC = led[number];	}
int	main(void)
{
	int	number;
	int	before;
	DDRC = 0xFF;
	DDRD = DDRD & ~(1<<PD0);	  // PD0를 입력핀으로 설정
	PORTD = PORTD | 1<<PD0;	  // 입력핀 PD0를 내부 풀업저항으로 연결
	number = 0;
	before = RELEASED;
	while(1){
		display_7segled(digit, number % 10);
		if( before==RELEASED && !(PIND & 1<<PD0) ){ 
			number++;
			_delay_ms(DEBOUNCE_MS); //시간지연 
			before = PRESSED;
		}else if( before==PRESSED && (PIND & 1<<PD0) ){
			_delay_ms(DEBOUNCE_MS); //시간지연
			before = RELEASED;
		}
	}
	return 0;
}