#include <stdint.h>
#include "msp432p401r.h"

void Delay_1ms(){
	//setup of loop takes ~13 clock cycles
	//clock rate is 3 MHz = 0.0033 ms
	//to get 1ms, must go 3000 clock cycles
	//13 taken by loop set up, 1 by breakdown -> loop takes up 2986 cycles
	//loop iteration takes up ~24 cycles
	//so you need 125 loop iterations to get ~3000 clock cycles
	int i;
	for(i = 0; i < 125; i++){
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
	P2REN = 0x07;
	P2DIR = 0x07;
}

uint8_t Read_Port1(void)
{
	return P1IN;
}

void Write_Port2(uint8_t value)
{
	P2OUT = value;
}

int main(void){
	Initialize_Port1();
	Initialize_Port2();

	uint8_t LED_on = 0x01;
	uint8_t LED_off = 0x00;
	Write_Port2(LED_on);
	int LED_state = 1;

	uint8_t button_bitmask = 0x02;
	while(1){
		uint8_t port1_val = Read_Port1();
		uint8_t my_button = port1_val & button_bitmask;
		if(my_button == 0x00){
			Delay_1ms();
			Delay_1ms();
			if(LED_state == 1){
				LED_state = 0;
				Write_Port2(LED_off);
			}
			else{
				LED_state = 1;
				Write_Port2(LED_on);
			}
		}
		else{
			Write_Port2(LED_on);
		}
	}
}
