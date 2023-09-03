/**
 * @file fsm.h
 * @author Ian Semikozov 
 * @brief simple implementation of the finite state machine in c
 * @version 0.1
 * @date 2023-08-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

#define FIFO_SIZE			16UL 		// max size of events in event pool

enum ReturnCode {
	COMPLETE = 0,
	REPEAT = 1,

	HANDLED = 0,
	IGNORED = 1
};

enum Event {
	EV_NONE = 0,
	EV_ADC
};

enum State {
	ST_STARTUP = 0,
	ST_UPDATESCREEN
};

typedef enum Event Event_t;
typedef enum State State_t;
typedef enum ReturnCode ReturnCode_t;

typedef struct {
    Event_t events[FIFO_SIZE];
    int8_t front;
    int8_t rear;
} FifoEventPool_t;

typedef struct {
    State_t nextState;
    State_t prevState;
    Event_t event;     
} Fsm_Cell_t;

typedef struct {
	FifoEventPool_t fifo;
	State_t curState;
	uint32_t timeOut;
	Fsm_Cell_t fsmTransitionTable[ST_NUM];
	void (*stateHandler[ST_H_NUM])();
} Fsm;

extern size_t numCells;
extern Fsm_Cell_t fsmTable[];
extern void (*stateHandler[])();
extern Fsm fsm;

/**
 * @brief push next event in top of pool to move to the next state
 * 
 * @param fsm finite state machine struct
 * @param ev new event (for example, from handler callers)
 * @return ReturnCode_t return status (HANDLED or IGNORED)
 */
ReturnCode_t fifoPush(Fsm *fsm, Event_t ev);

/**
 * @brief pull the first event in bot of pool to process it
 * 
 * @param fsm finite state machine struct
 * @return Event_t return pulled event
 */
Event_t fifoPull(Fsm *fsm);

/**
 * @brief finite state machine init
 * 
 * @param fsm finite state machine struct
 * @param fsmTable table of transitions of the fsm
 * @param numCells number of states in table
 * @param stateHandler table with func pointers 
 * @param numStates number of states
 */
void fsmInit(Fsm *fsm, Fsm_Cell_t *fsmTable, size_t numCells, void (*stateHandler[])(), size_t numStates);

/**
 * @brief process event, move to next state and call state handler function
 * 
 * @param fsm finite state machine struct
 * @param ev event, received from event pool
 * @return ReturnCode_t return status (HANDLED or IGNORED)
 */
ReturnCode_t fsmDispatch(Fsm *fsm, Event_t ev);

#endif // FSM_H