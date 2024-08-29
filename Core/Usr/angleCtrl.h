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
    float output_min;
    float integral_max;
    float feedforward_gain;
} AngleCtrl_t;

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define LIMIT_MAX(x, min, max) (x < min ? min : (x > max ? max : x))
#define LIMIT_MIN(x, min_n, min_p) (x < 0 ? (x < min_n ? x : min_n) : (x > min_p ? min_p : x))
#define TURN_ANGLE(angle, death, base_speed) while(ABS(AngleCtrl(angle, base_speed)) > death);

float AngleCtrl(float angle, int16_t base_speed);
float AngleCtrl_feedforward(float angle, int16_t base_speed);
float AnglePID(float angle);

#endif //CAR_2023_ANGLECTRL_H
