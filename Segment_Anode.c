#include		<avr/io.h>
#define	PRESSED		1 //전상태를 판단하기 위한 PRESSED정의 – 전상태가 눌려져있던 상태
#define	RELEASED		0 //전상태를 판단하기 위한 RELEASED정의 – 전상태가 떨어져 있던 상태
unsigned char digit[] = {0x88, 0xBE, 0xC4, 0xA4, 0xB2, 0xA1, 0x83, 0xBC, 0x80, 0xB0};//애노드 타입 LED 출력 배열
void	display_7segled(unsigned char led[], unsigned int number) //7segment ON 함수
{	PORTC = led[number];	} //port C를 모두 출력으로 사용
int	main(void)
{
	int	number //스위치가 들어오는 횟수를 카운트하기 위한 변수 선언
	int	before;//전 상태를 판단하기위한 before 변수 선언
	DDRC = 0xFF; //C포트를 출력으로 지정
	DDRD = DDRD & ~(1<<PD0);	  // PD0를 입력핀으로 설정
	PORTD = PORTD | 1<<PD0;	  // 입력핀 PD0를 내부 풀업저항으로 연결
	number = 0; //초기 스위치 누른 횟수를 0으로 초기화
	before = RELEASED; //초기 상태를 떨어져 있던 상태로 초기화
	while(1){
		display_7segled(digit, number % 10); //스위치를 누른횟수에 따라 LED ON
		
		if( before==RELEASED && !(PIND & 1<<PD0) ){ //전상태 open, 처음으로 눌린 상태
			number++; //스위치가 눌리면 스위치 카운트 변수 1증가
			before = PRESSED;//전상태를 눌린 상태로 지정
		}else if( before==PRESSED && (PIND & 1<<PD0) ){//전에 눌린 상태에서 처음으로 오픈	
			before = RELEASED;
		}
	}
	return 0;}//함수 값 반환