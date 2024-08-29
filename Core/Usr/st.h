//
// Created by shimmer on 2024/8/29.
//

#ifndef CAR_2023_ST_H
#define CAR_2023_ST_H

#include <stdint.h>

extern uint8_t up2_state;

enum UP2_State
{
    UP2_STATE_READY = 0,
    UP2_STATE_BLACK_TRACKING,
    UP2_STATE_WHITE_STRIGHT,
    UP2_STATE_TURN_RIGHT,
    UP2_STATE_TURN_LEFT,
    UP2_STATE_STOP,
    UP2_STATE_ERROR
};

void UP2_StateMachine(void);

#endif //CAR_2023_ST_H
