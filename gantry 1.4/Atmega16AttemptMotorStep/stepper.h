/*
 * stepper.h
 *
 * Created: 22.12.2016 23:34:04
 *  Author: lukasz.jamroz
 */
#ifndef STEPPER_H_
#define STEPPER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
volatile char txIndex;

enum StepperDirection{ CLOCKWISE, COUNTERCLOCKWISE, UNDEFINED };
enum StepperModes{ PHASE_A, PHASE_B, PHASE_C, PHASE_D };

typedef struct Motor_Order
{
	uint16_t move;
	enum StepperDirection direction;
	volatile struct Motor_Order* next;
	
} Motor_Order;

enum StepperIndexes{ GRIP, ELBOW_S, ELBOW_F, KNEE, PELVIS, TURNTABLE, STEPPER_NUM_OF_MOTORS };
enum StepperPlacement{ MSB, LSB };
typedef struct Motor_Struct
{
	enum StepperIndexes index;
	volatile uint8_t* port; 
	enum StepperPlacement placement;   
	uint8_t speed;
	volatile struct Motor_Order* order_list;
	
	volatile uint8_t timer;
	uint16_t step;
	enum StepperModes mode;
	uint8_t limit;
} Motor_Struct;

volatile Motor_Struct stepper_motors_table[STEPPER_NUM_OF_MOTORS];

void stepper_init();
void stepper_add_motor(enum StepperIndexes index_arg, volatile uint8_t* port_arg, enum StepperPlacement placement_arg);
void stepper_set_speed(enum StepperIndexes index_arg, uint8_t speed_arg);
void stepper_process_motor(uint8_t index_arg);
void stepper_fastest_and_init_motors();
void stepper_pop_front_order(uint8_t index_arg);
void stepper_push_back_order(uint8_t index_arg,uint16_t limit_arg,enum StepperDirection direction_arg);

#endif /* STEPPER_H_ */