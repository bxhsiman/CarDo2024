#ifndef __MOTOR_DRIVE_H
#define __MOTOR_DRIVE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "tim.h"
#include "stm32_hal_legacy.h"

void MotoCtrlInit(void);
void MotoLeftFrontCtrl(int16_t speed);
void MotoLeftBackCtrl(int16_t speed);
void MotoRightFrontCtrl(int16_t speed);
void MotoRightBackCtrl(int16_t speed);
void StopAllMoto(void);

#endif
