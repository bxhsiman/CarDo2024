//
// Created by shimmer on 2024/8/24.
//

#ifndef CAR_2023_ANGLECTRL_H
#define CAR_2023_ANGLECTRL_H

#include <stdint.h>

typedef struct
{
    float Kp;
    float Ki;
    float Kd;
    float target;
    float integral;
    float last_error;
    float output_max;
    float integral_max;
} AngleCtrl_t;

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define LIMIT(x, min, max) (x < min ? min : (x > max ? max : x))
#define TURN_ANGLE(angle, death, base_speed) while(ABS(AngleCtrl(angle, base_speed)) > death);

float AngleCtrl(float angle, int16_t base_speed);

#endif //CAR_2023_ANGLECTRL_H
