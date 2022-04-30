#include		<avr/io.h>
#include		<avr/interrupt.h> //인터럽트 관련 라이브러리
#define	DISP_INTERVAL	2 //disperation interval(분산간격) 2로 정의
#define	OC0		PB4 //PB4를 OC0로 정의 – PWM신호 폭 결정
volatile	unsigned	long	timer0;		// 오버플로마다 1씩 증가될 변수
volatile	unsigned	int	number;  //디스플레이 할 숫자를 저장하는 변수
unsigned 	char led[] = {0x48, 0x7D, 0xC4, 0x64, 0x71, 0x62, 0x43, 0x7C, 0x40, 0x70};//출력할 디스플레이 값 배열 
// 타이머/카운터0 인터럽트 서비스 루틴
ISR(TIMER0_OVF_vect) // 인터럽트 서비스 루틴
{
	timer0++; // 오버플로마다 1씩 증가
	// 두 자리7-세그먼트 LED 디스플레이
	if( timer0 % DISP_INTERVAL == 0){
		PORTC = (timer0 % (DISP_INTERVAL<<1) == 0) ? led[(number % 100) / 10] : led[number%10];
		PORTD = (PORTD |0xC0) & ~(1<<((timer0 % (DISP_INTERVAL<<1) == 0) ? PD7 : PD6));
	}
}
int	main(void)
{	
	DDRC = 0xFF; //C포트를 출력으로 지정
	DDRD |= 1<<PD7 | 1<<PD6;	// 두 자리 7-세그먼트 LED를 켜기위한 출력
	DDRB |= 1<<OC0;		// OC0 = PB4 출력

	TCCR0 = 1<<WGM01 | 1<<WGM00;	// 고속 PWM 파형 발생모드
	TCCR0 |= 1<<CS02 | 1<<CS01;	// 프리스케일러 CS02:00 = (1,1,0) 256 분주
	TCCR0 |= 1<<COM01;		// TCNT0 0xFF에서 1, 상승 도중 OCR0와 일치하면 0
	
	TIMSK |= 1<<TOIE0;		// 타이머/카운터0 인터럽트 활성화
	timer0 = 0; //변수 초기화
	sei(); //SERG의 Global bit set 하여ㅏ 모든 인터럽트 활성화

	OCR0 = 100; //OCR값 설정
	number = OCR0*100/256;	// OCR0 값과 number 값을 일치
	
	while(1);			// 무한 loop
	return 0;}