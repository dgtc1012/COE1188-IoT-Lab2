#include <stdint.h>
#include "msp432p401r.h"


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


int main(void)
{
	Initialize_Port1();
	Initialize_Port2();

	uint8_t bitmask = 0x12;
	while(1){
		uint8_t input = Read_Port1();
		uint8_t myBits = input & bitmask;

		if(myBits == 0x00){
			Write_Port2(0x02);
		}
		else if(myBits == 0x12){
			Write_Port2(0x04);
		}
		else{
			Write_Port2(0x01);
		}
	}
}
