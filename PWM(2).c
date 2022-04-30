#include <avr/io.h>
#include	 <avr/interrupt.h>
#define	F_CPU	16000000UL //클럭 주파수 설정
#define	PRESCALE			256L //256분주 설정
#define	PULSE_PER_OVERFLOW	510L  //PWM 오버플로우 주기 펄스값
#define	MS_OVERFLOW_CYCLE	((double)(PULSE_PER_OVERFLOW * PRESCALE) \
				/(double)((double)F_CPU/1000.0))
#define	OC0		PB4 //co0=PB4
#define	UP		PB7 // PB7을 UP 스위치로
#define	DOWN	PB6 //PB6를 DOWN 스위치로
#define	NUM_REQ		2  //req[]배열 크기 설정
#define	REQ_UP		0 //UP스위치 응답 지연 배열 위치
#define	REQ_DOWN	1 //DOWN 스위치 응답 지연 배열 위치
#define	DEBOUNCE_CYCLE	50 //지연값 설정 = ms_interval. 50이면 약 0.05초 지연
volatile	unsigned	long	timer0;		// 오버플로마다 1씩 증가될 변수
volatile	unsigned	int		number; //디스플레이 될 숫자의 변수
unsigned char led[] = {0x48, 0x7D, 0xC4, 0x64, 0x71, 0x62, 0x43, 0x7C, 0x40, 0x70}; //디스플레이 할 값의 배열
volatile	unsigned	long	req[NUM_REQ] = {0, 0}; //req[2]배열 초기값 00
double		ms_ov_cycle; //1회 오버플로에  소요되는 시간 저장 변수
// 타이머/카운터0 인터럽트 서비스 루틴
SIGNAL(SIG_OVERFLOW0) 
{	int	i;

	timer0++;		// 오버플로마다 1씩 증가
	
	// 오버플로 횟수가 짝수일 때 10자리, 홀수일 때 1자리 디스플레이
	PORTC = (timer0 % 2 == 0) ? led[(number % 100) / 10] : led[number%10];
	PORTD = (PORTD |0xC0) & ~(1<<((timer0 % 2 == 0) ? PD7 : PD6));
	
	for(i=0; i<NUM_REQ; i++)
		if( req[i] > 0)		// REQ 요청이 있을 때만 
			req[i]--;		// 시간 지연 경과 응답
}
// ms_interval초 시간 지연을 위한 오버플로 횟수 계산 함수
unsigned long	ms_req_timer0(unsigned long ms_interval)
{	return ( ms_interval <= 0) ? 0 : \
			+ (unsigned long)(ms_interval / ms_ov_cycle); 
}

int	main(void)
{	
	DDRC = 0xFF; //C포트를 출력으로 지정
	DDRD |= 1<<PD7 | 1<<PD6;	// 두 자리 7-세그먼트 LED를 켜기위한 출력
	DDRB |= 1<<OC0;		// OC0 = PB4 출력
	DDRB &= ~(1<<UP | 1<<DOWN);	// UP, DOWN 스위치 위치를 입력 방향으로
	PORTB |= 1<<UP | 1<<DOWN;	// UP, DOWN 풀업저항 

	TCCR0 = 1<<WGM00;              // 위상정정 PWM 파형 발생모드
	TCCR0 |= 1<<CS02 | 1<<CS01;   // 프리스케일러 CS02:00 = (1,1,0) 256 분주
	TCCR0 |= 1<<COM01;		// 상승 중 OCR0와 일치 0, 하강 중 일치 1
	
	TIMSK |= 1<<TOIE0;		// 타이머/카운터0 인터럽트 활성화
	
	timer0 = 0; //초기화
	sei(); //SERG의 Global bit set 하여 모든 인터럽트를 활성화

	ms_ov_cycle	= MS_OVERFLOW_CYCLE;

	OCR0 = 0;
	number = OCR0*100/256;	// OCR0 값과 number 값을 일치
	
	while(1){
	  if( (req[REQ_UP]==0) && !(PINB & (1<<UP))){ //req[UP]응답, UP 스위치 눌림 검사
		 req[REQ_UP] = ms_req_timer0(DEBOUNCE_CYCLE);
		 OCR0 = (OCR0 == 255) ? 255 : OCR0 + 1;
	  }
			
	  if( (req[REQ_DOWN]==0) && !(PINB & (1<<DOWN))){ //req[DOWN]응답,DOWN 스위치 검사
		 req[REQ_DOWN] = ms_req_timer0(DEBOUNCE_CYCLE);
	     OCR0 = (OCR0 == 0) ? 0 : OCR0 - 1;
	  }

	  number = OCR0 * 100/256;
	}
	
	return 0;
}