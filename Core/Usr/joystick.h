//
// Created by shimmer on 2024/8/28.
//

#ifndef CAR_2023_JOYSTICK_H
#define CAR_2023_JOYSTICK_H

#include <stdint.h>

#define MAX_JOYSTICK_SPEED 500
#define LIMIT_MAX(x, min, max) (x < min ? min : (x > max ? max : x))
void Joystick_DataProcess(uint8_t *data,  uint16_t len);

#endif //CAR_2023_JOYSTICK_H
