/*
 * stepper.c
 *
 * Created: 22.12.2016 23:33:52
 *  Author: lukasz.jamroz
 */ 
#include "stepper.h"

void stepper_add_motor(enum StepperIndexes index_arg, volatile uint8_t* port_arg, enum StepperPlacement placement_arg)
{
	stepper_motors_table[index_arg].index = index_arg;
	stepper_motors_table[index_arg].port = port_arg;
	stepper_motors_table[index_arg].placement = placement_arg;
	stepper_motors_table[index_arg].step = 0;
	stepper_motors_table[index_arg].speed = 0xFF;
	stepper_motors_table[index_arg].limit = 108; // 127 DEC 0111 1111 BIN
}
void stepper_set_speed(enum StepperIndexes index_arg, uint8_t speed_arg)
{
	stepper_motors_table[index_arg].limit = speed_arg;
}
void stepper_init()
{	
	TIMSK |= (1<<TOIE0);
	TCNT0 = 0;
	TCCR0 |= (1<<CS00);
	
	sei();
}
ISR(TIMER0_OVF_vect)
{
	for(uint8_t index = 0; index < STEPPER_NUM_OF_MOTORS; index++)
	{
		if(stepper_motors_table[index].timer == stepper_motors_table[index].speed) 
		{
			if(stepper_motors_table[index].order_list != NULL)
			{
				stepper_process_motor(index);
			}
			stepper_motors_table[index].timer = 0;
		}else
			stepper_motors_table[index].timer++;
	}
}
void stepper_process_motor(uint8_t index_arg)
{
	uint8_t stepper_a, stepper_b, stepper_c, stepper_d;
	if(stepper_motors_table[index_arg].placement == MSB)
	{
		stepper_a = 4;
		stepper_b = 5;
		stepper_c = 6;
		stepper_d = 7;
	}else
		{
			stepper_a = 0;
			stepper_b = 1;
			stepper_c = 2;
			stepper_d = 3;
		}
	
	switch(stepper_motors_table[index_arg].mode)
	{
		case PHASE_A:
		*stepper_motors_table[index_arg].port &= ~(1<<stepper_c);
		*stepper_motors_table[index_arg].port |= (1<<stepper_a);
		break;
		
		case PHASE_B:
		*stepper_motors_table[index_arg].port &= ~(1<<stepper_d);
		*stepper_motors_table[index_arg].port |= (1<<stepper_b);
		break;
		
		case PHASE_C:
		*stepper_motors_table[index_arg].port &= ~(1<<stepper_a);
		*stepper_motors_table[index_arg].port |= (1<<stepper_c);
		break;
		
		case PHASE_D:
		*stepper_motors_table[index_arg].port &= ~(1<<stepper_b);
		*stepper_motors_table[index_arg].port |= (1<<stepper_d);
		break;
		
		default: break;
	}
	
	if(stepper_motors_table[index_arg].order_list->direction == CLOCKWISE)
	{
		if(stepper_motors_table[index_arg].mode == PHASE_D) 
		{
			stepper_motors_table[index_arg].mode = PHASE_A;
		}
		else 
			stepper_motors_table[index_arg].mode++;
	}else
		if(stepper_motors_table[index_arg].order_list->direction == COUNTERCLOCKWISE)
		{
			if(stepper_motors_table[index_arg].mode == PHASE_A)
			{
				stepper_motors_table[index_arg].mode = PHASE_D;
			}
			else
				stepper_motors_table[index_arg].mode--;
		}
	if(stepper_motors_table[index_arg].step == (stepper_motors_table[index_arg].order_list->move+4))
	{
		*stepper_motors_table[index_arg].port &= ~((1<<stepper_a)|(1<<stepper_b)|(1<<stepper_c)|(1<<stepper_d));
		stepper_pop_front_order(index_arg);
		//stepper_motors_table[index_arg].order_list->direction = UNDEFINED;

	}else
		stepper_motors_table[index_arg].step++;
		
	if((stepper_motors_table[index_arg].step%100) == 0) 
	{
		//UDR = '!';
		//txIndex++;
	}
	
	if( stepper_motors_table[index_arg].speed > stepper_motors_table[index_arg].limit ) stepper_motors_table[index_arg].speed--;
}
void stepper_fastest_and_init_motors()
{
	uint8_t fastest_speed = 255;
	for(uint8_t index = 0; index < STEPPER_NUM_OF_MOTORS; index++)
	{
		if( stepper_motors_table[index].speed < fastest_speed )
		{
			fastest_speed = stepper_motors_table[index].speed;
		}
		stepper_motors_table[index].order_list = NULL;
		stepper_motors_table[index].timer = 0;
	}
	fastest_speed = fastest_speed;
}

void stepper_pop_front_order(uint8_t index_arg)
{
	volatile Motor_Order * temp;
	temp = stepper_motors_table[index_arg].order_list;
	if(temp != NULL)
	{
		stepper_motors_table[index_arg].speed = 0xFF;
		if(temp->next != NULL)
		{
			if(temp->next->direction == CLOCKWISE)
				stepper_motors_table[index_arg].mode = PHASE_A;
			else
				stepper_motors_table[index_arg].mode = PHASE_D;
		}
		stepper_motors_table[index_arg].step = 0;	
		stepper_motors_table[index_arg].timer = 0;
		stepper_motors_table[index_arg].order_list = temp->next;
		
		if(stepper_motors_table[index_arg].placement == MSB)
		{
			*stepper_motors_table[index_arg].port &= ~(0xF0);
		}else
			{
				*stepper_motors_table[index_arg].port &= ~(0xF);
			}
		free((void*)temp);   
	}
}
void stepper_push_back_order(uint8_t index_arg,uint16_t limit_arg,enum StepperDirection direction_arg)
{
	volatile Motor_Order* temp = stepper_motors_table[index_arg].order_list;
	volatile Motor_Order* newOrder = malloc(sizeof(Motor_Order));

	newOrder->next = NULL;
	newOrder->move = limit_arg;
	newOrder->direction = direction_arg;
	

	if(temp != NULL)
	{
		while(temp->next != NULL) temp = temp->next;
		temp->next = newOrder;
	}
	else
		{
			if(newOrder->direction == CLOCKWISE) 
				stepper_motors_table[index_arg].mode = PHASE_A;
			else 
				stepper_motors_table[index_arg].mode = PHASE_D;
			
			stepper_motors_table[index_arg].order_list = newOrder;
		}
}