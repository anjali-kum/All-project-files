/*
 * defines.h
 *
 * Created: 25-12-2022 07:43:50
 *  Author: abbho
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_


enum{false, true};

#define MOTOR_OP	PA3
#define LID_SENSE	PA1
#define RUN			PA2
#define BLUE_LED	PB0
#define BUZZER   	PB1
#define RED_LED		PB2


#define STATE_IDLE		0
#define STATE_RUN		1
#define STATE_PHASE1	2
#define STATE_PHASE2	3
#define STATE_COMPLETE	4
#define STATE_OPEN_LID	5

#define LID_CLOSE					0
#define START_PRESSED				1
#define PHASE1_COMPLETE				6
#define PHASE2_COMPLETE				7
#define LID_CLOSE_MASK				1
#define READY_TO_RUN_MASK			3
#define PHASE1_COMPLETE_MASK		0x40
#define PHASE2_COMPLETE_MASK		0x80
#define PHASE1				0
#define PHASE2				1

typedef struct _KeySt
{
	unsigned keyPressed:	1;
	unsigned keyReleased:	1;
	unsigned CheckDebounce:	1;
	
}__KeySt;
struct  _KeySt KeySt;

#endif /* DEFINES_H_ */