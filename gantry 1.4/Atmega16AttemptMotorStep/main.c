/*
 * Atmega16AttemptMotorStep.c
 *
 * Created: 22.12.2016 21:37:05
 * Author : lukasz.jamroz
 */ 
#include <avr/io.h>
#include "stepper.h"

volatile char command[3];

ISR(USART_RXC_vect)
{
	command[0] = command[1];
	command[1] = command[2];
	command[2] = UDR;
	
	if(command[2] == '.')
	{
		enum StepperDirection dir = UNDEFINED;
		if(command[1] == '<') dir = COUNTERCLOCKWISE;
			else
				if(command[1] == '>')	dir = CLOCKWISE;
			
		switch(command[0])
		{
			case 'a':
			stepper_push_back_order(GRIP,60000,dir);
			break;
			case 'b':
			stepper_push_back_order(ELBOW_S,60000,dir);
			stepper_push_back_order(ELBOW_F,60000,dir);
			break;
			case 'c':
			stepper_push_back_order(ELBOW_F,60000,dir);
			if(dir == COUNTERCLOCKWISE) stepper_push_back_order(ELBOW_S,60000,CLOCKWISE);
				else stepper_push_back_order(ELBOW_S,60000,COUNTERCLOCKWISE);
			break;
			case 'd':
			stepper_push_back_order(KNEE,60000,dir);
			break;
			case 'e':
			stepper_push_back_order(PELVIS,60000,dir);
			break;
			case 'f':
			stepper_push_back_order(TURNTABLE,60000,dir);
			break;
			default: break;
		}
	}
	else 
		if(command[2] == ';')
		{	
			for(uint8_t i = 0; i < STEPPER_NUM_OF_MOTORS; i++) stepper_pop_front_order(i);
		}
}
ISR(USART_TXC_vect)
{
	char buff;
	switch(txIndex)
	{
		case 0:
		UDR = '!';
		break;
		case 1:
		buff = (stepper_motors_table[0].step>>8);
		UDR = buff;
		break;
		case 2:
		buff = (stepper_motors_table[0].step);
		UDR = buff;
		break;
		case 3:
		buff = (stepper_motors_table[1].step>>8);
		UDR = buff;
		break;
		case 4:
		buff = (stepper_motors_table[1].step);
		UDR = buff;
		break;
		case 5:
		buff = (stepper_motors_table[2].step>>8);
		UDR = buff;
		break;
		case 6:
		buff = (stepper_motors_table[2].step);
		UDR = buff;
		break;
		case 7:
		buff = (stepper_motors_table[3].step>>8);
		UDR = buff;
		break;
		case 8:
		buff = (stepper_motors_table[3].step);
		UDR = buff;
		break;
		case 9:
		buff = (stepper_motors_table[4].step>>8);
		UDR = buff;
		break;
		case 10:
		buff = (stepper_motors_table[4].step);
		UDR = buff;
		break;
		case 11:
		buff = (stepper_motors_table[5].step>>8);
		UDR = buff;
		break;
		case 12:
		buff = (stepper_motors_table[5].step);
		UDR = buff;
		break;
		case 13:
		//for(uint8_t i = 0; i < STEPPER_NUM_OF_MOTORS; i++) stepper_pop_front_order(i);
		UDR = '.';
		break;
		default: break;
	}
	
	if(txIndex == 14) txIndex = 0;
	else txIndex++;
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
	UBRRL = 16; //stop bits:  1 //parity:  Even
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0)|(1<<UPM1);  
	UCSRB |= (1<<RXEN)|(1<<RXCIE)|(1<<TXEN)|(1<<TXCIE);
	//UCSRA |= (1<<U2X);
	
	stepper_add_motor(GRIP,&PORTB,LSB);
	stepper_add_motor(ELBOW_S,&PORTB,MSB);
	stepper_add_motor(ELBOW_F,&PORTA,LSB);
	stepper_set_speed(ELBOW_F,180);
	stepper_add_motor(KNEE,&PORTA,MSB);
	stepper_set_speed(KNEE,100);
	stepper_add_motor(PELVIS,&PORTC,MSB);
	stepper_set_speed(PELVIS,120);
	stepper_add_motor(TURNTABLE,&PORTC,LSB);
	stepper_fastest_and_init_motors();
	stepper_init();
	
	txIndex = 0;
	
    while(1) {}
}

