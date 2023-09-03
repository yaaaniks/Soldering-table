#include "fsm.h"
#include "io.h"
#include "constants.h"
Fsm fsm;

// each transition should be an array of three elements - next state, previous state and event
// smth like that - {ST_NEXT, ST_PREV, EVENT}
// Fsm_Cell_t fsmTable[] = {};             

// size_t numCells = sizeof(fsmTable) / sizeof(fsmTable[0]);

// each state has its own cell with a func pointer
// smth like that - [ST_STATE] = funcPointer
// void (*stateHandler[])() = {};

static inline bool isQueueEmpty(FifoEventPool_t *fifo) {
    return (fifo->front == -1 || fifo->rear == fifo->front);
}

static inline bool isQueueFull(FifoEventPool_t *fifo) {
    return (fifo->front == 0 && fifo->rear == FIFO_SIZE - 1) ||
           (fifo->rear == (fifo->front - 1) % (FIFO_SIZE - 1));
}

ReturnCode_t fifoPush(Fsm *fsm, Event_t ev) {
    __disable_irq();
    ReturnCode_t ret;
    if (isQueueFull(&fsm->fifo)) {
        ret = IGNORED;
    }

    if (fsm->fifo.front == -1)
        fsm->fifo.front = fsm->fifo.rear = 0;
    else
        fsm->fifo.rear = (fsm->fifo.rear + 1) % FIFO_SIZE;

    fsm->fifo.events[fsm->fifo.rear] = ev;
    ret = HANDLED;
    __enable_irq();
    return ret;
}

Event_t fifoPull(Fsm *fsm) {
    __disable_irq();
    Event_t ret;
    if (isQueueEmpty(&fsm->fifo)) {
        ret = EV_NONE;
    } else {
        ret = fsm->fifo.events[fsm->fifo.front];
        fsm->fifo.events[fsm->fifo.front] = EV_NONE;
        if (fsm->fifo.front == fsm->fifo.rear)
            fsm->fifo.front = fsm->fifo.rear = -1;
        else
            fsm->fifo.front = (fsm->fifo.front + 1) % FIFO_SIZE;
    }
    __enable_irq();
    return ret;
}

void fsmInit(Fsm *fsm, Fsm_Cell_t *fsmTable, size_t numCells, void (*stateHandler[])(), size_t numStates) {
    // fsm->fsmTransitionTable = fsmTable;
    // memcpy(fsm->fsmTransitionTable, fsmTable, sizeof(fsm->fsmTransitionTable));
    memcpy(fsm->fsmTransitionTable, fsmTable, numCells * sizeof(Fsm_Cell_t));
    for (State_t state = 0; state < numStates; ++state) {
        fsm->stateHandler[state] = stateHandler[state];
    }
    // fsm->fsmTransitionTable = (Fsm_Cell_t *)malloc(numCells * sizeof(Fsm_Cell_t)); // allocate memory for fsm table
    // if (fsm->fsmTransitionTable == NULL) {
    //     return;
    // }
    fsm->fifo.front = -1;
    fsm->fifo.rear = -1;
    fsm->curState = ST_UPDATESCREEN;
}

void fsmCleanup(Fsm *fsm) {
    free(fsm->fsmTransitionTable);
}

ReturnCode_t fsmDispatch(Fsm *fsm, Event_t ev) {
    for (uint8_t i = 0; i < ST_NUM; i++) {
        if (fsm->fsmTransitionTable[i].event == ev && fsm->fsmTransitionTable[i].prevState == fsm->curState) {
            fsm->curState = fsm->fsmTransitionTable[i].nextState;
        }
    }
    void (*handler)(void) = fsm->stateHandler[fsm->curState]; 
    if (handler) {
        handler();
        return HANDLED;
    }
    return IGNORED;
}