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
    UP2_STATE_BLACK_TRACKING = 1,
    UP2_STATE_WHITE_STRIGHT = 2,
    UP2_STATE_TURN_RIGHT = 3,
    UP2_STATE_TURN_LEFT = 4,
    UP2_STATE_STOP = 5,
    UP2_STATE_FINDLINE_LEFT = 6,
    UP2_STATE_FINDLINE_RIGHT = 7,
    UP2_STATE_ERROR
};

void UP2_StateMachine(void);

#endif //CAR_2023_ST_H
