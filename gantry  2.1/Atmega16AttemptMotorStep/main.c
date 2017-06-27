/*
 * Atmega16AttemptMotorStep.c
 *
 * Created: 22.12.2016 21:37:05
 * Author : lukasz.jamroz
 */ 
#include <avr/io.h>
#include "stepper.h"
#include "hd44780.h"

volatile char command[8];

ISR(USART_RXC_vect)
{
	command[0] = command[1];
	command[1] = command[2];
	command[2] = command[3];
	command[3] = command[4];
	command[4] = command[5];
	command[5] = command[6];
	command[6] = command[7];
	command[7] = UDR;
	
	if(command[7] == '.')
	{
		enum StepperDirection dir = UNDEFINED;
		if(command[1] == '<') dir = COUNTERCLOCKWISE;
			else
				if(command[1] == '>')	dir = CLOCKWISE;
		uint16_t steps = 0;
		steps = 0xff & command[3];
		steps <<= 8;
		steps |= 0xff & command[2];
		switch(command[0])
		{
			case 'm':
				stepper_push_back_order(MOVE,steps,dir,command[4],command[5],command[6]);
			break;
			case 'l':
				stepper_push_back_order(LEFTRIGHT,steps,dir,command[4],command[5],command[6]);
			break;
			case 'u':
				stepper_push_back_order(UPDOWN,steps,dir,command[4],command[5],command[6]);
			break;
			case 't':
				stepper_push_back_order(TURN,steps,dir,command[4],command[5],command[6]);
			break;
			case 's':;
				if(command[1] == 'S') 
				{
					servo_set_servo(PUSH,steps);
				}else
					servo_push_back_order(PUSH,steps,command[4],command[5],command[6]);
			break;
			default:
			break;
		}

	}
	else 
	if(command[7] == ';')
	{
		for(uint8_t i = 0; i < STEPPER_NUM_OF_MOTORS; i++) stepper_pop_front_order(i);
	}else
	if(command[7] == 'X')
	{
		TCCR0 &= ~(1<<CS00);
		TCCR1B&= ~( (1<<CS11)|(1<<CS10) );
	}else
	if(command[7] == 'Y')
	{
		TCCR0 |= (1<<CS00);
		TCCR1B|= (1<<CS11)|(1<<CS10);
	}
		
}

int main(void)
{
	PORTB = 0x00;
	DDRB = 0xff;
	
	PORTA = 0x00;
	DDRA = 0xff;
	
	PORTC = 0x00;
	DDRC = 0xff;
	
	PORTD = 0x00;
	DDRD = 0x02;
	
	UBRRH = 0; //data bits: 8 //baud:  57600 
	UBRRL = 16; //stop bits:  1 //parity:  No
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  
	UCSRB |= (1<<RXEN)|(1<<RXCIE)|(1<<TXEN)|(1<<TXCIE);
	
	stepper_add_motor(TURN,&PORTB,LSB);
	stepper_add_motor(LEFTRIGHT,&PORTB,MSB);	
	stepper_add_motor(MOVE,&PORTC,MSB);
	stepper_add_motor(UPDOWN,&PORTC,LSB);
	stepper_set_speed(UPDOWN,200);
	servo_add_servo(PUSH,&PORTD,PD5,0);
	
	stepper_fastest_and_init_motors();	
	stepper_init();
	servo_init();
	
    while(1) {}
}

