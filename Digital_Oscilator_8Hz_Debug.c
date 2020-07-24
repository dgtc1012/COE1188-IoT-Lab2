#include <stdint.h>
#include "msp432p401r.h"

//#define NUM_CYCLES_2_WAIT = 0x0005B8D8; // 2) 3,000,000 Hz/ 8Hz = 375,000 cycles/tick = 5b8d8 in hex

void SysTick_Init( )
{
	SYSTICK_STCSR = 0; // 1) disable SysTick during setup
	SYSTICK_STRVR = 0x00ffffff; // 2) delay by correct number of clock cycles
	SYSTICK_STCVR = 0; // 3) any write to current clears it
	SYSTICK_STCSR = 0x00000005; // 4) enable SysTick with core clock
}

void Delay_8Hz(){
	SYSTICK_STRVR = 0x0002dc6c-1;
	SYSTICK_STCVR = 0;
	while((SYSTICK_STCSR & 0x00010000) == 0){
		//wait for count flag to be asserted
	}
}

void Delay_1ms(){
	//setup of loop takes ~13 clock cycles
	//clock rate is 3 MHz = 0.0033 ms
	//to get 1ms, must go 3000 clock cycles
	//13 taken by loop set up, 1 by breakdown -> loop takes up 2986 cycles
	//loop iteration takes up ~18 cycles
	//so you need 166 loop iterations to get ~3000 clock cycles
	int i;
	for(i = 0; i < 166; i++){
		//each iteration of the loop takes ~18 cycles
		asm(" add R0, R0, #0\n");
	}
	//breakdown take 1 instruction
}

void Initialize_Port1(void)
{
	P1SEL0 = 0x00;
	P1SEL1 = 0x00;
	P1REN = 0x12;
	P1DIR = 0x00;
}

void Initialize_Port2(void)
{
	P2SEL0 = 0x00;
	P2SEL1 = 0x00;
	P2REN = 0x03;
	P2DIR = 0x03;
}

uint8_t Read_Port1(void)
{
	return P1IN;
}

uint8_t Read_Port2(void)
{
	return P2OUT;
}

void Write_Port1(uint8_t value)
{
	P1OUT = value;
}

void Write_Port2(uint8_t value)
{
	P2OUT = P2OUT|value;
}

void Write_RedLED(){
	P2OUT ^= BIT0;
}
void Write_BlueLED(){
	P2OUT ^= BIT1;
}

void Debug_Init(uint8_t * data_buffer, uint32_t * time_buffer, int buf_len){
	int i;
	for(i = 0; i<buf_len; i++){
		data_buffer[i]=0xff;
		time_buffer[i]=0xffffffff;
	}
	SysTick_Init();
}

int Debug_Capture(uint8_t * data_buffer, uint32_t * time_buffer, int buf_len, int index){
	if(index < buf_len){
		uint8_t in = Read_Port1();
		in = in & 0x02;
		in = in << 3;
		uint8_t out = Read_Port2();
		out = out & 0x01;
		uint8_t inout = in | out;
		data_buffer[index] = inout;
		if(index > 0)
			time_buffer[index] = time_buffer[index-1] + SYSTICK_STCVR;
		else
			time_buffer[index] = SYSTICK_STCVR;
		index++;
	}
	return index;
}

int main(void){

	Initialize_Port1();
	Initialize_Port2();

	int num_readings = 24; //if we clock at 8 hz then we have 8 readings per second, we want 3 seconds of data so 8*3=24
	uint8_t data_buffer[24];
	uint32_t time_buffer[24];

	Debug_Init(data_buffer, time_buffer, num_readings);

	Write_RedLED();
	Write_BlueLED();
	int red_LED_state = 1;
	int heartbeat_LED_state = 1;

	int index = 0;

	uint8_t button_bitmask = 0x02;
	while(1){
		uint8_t port1_val = Read_Port1();
		uint32_t my_button = port1_val & button_bitmask;
		Delay_8Hz();

		index = Debug_Capture(data_buffer, time_buffer, num_readings, index);

		if(heartbeat_LED_state == 1){
			heartbeat_LED_state = 0;
			Write_BlueLED();
		}
		else{
			heartbeat_LED_state = 1;
			Write_BlueLED();
		}

		if(my_button == 0x00){
			if(red_LED_state == 1){
				red_LED_state = 0;
				Write_RedLED();
			}
			else{
				red_LED_state = 1;
				Write_RedLED();
			}
		}
		else{
			Write_Port2(0x01);
		}
	}
}

