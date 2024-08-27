//
// Created by shimmer on 2024/8/23.
//

#ifndef CAR_2023_MOTORCTRL_H
#define CAR_2023_MOTORCTRL_H

#include <stdint.h>
#include <stdbool.h>
#include "car_drive.h"

#define FORWARD_SPEED (300)
#define BACKWARD_SPEED (300)
#define TURN_SPEED (300)
#define LIMIT_SPEED (500)

// ���ڿ�ʼʱ��Ħ�����ܴ󣬲���һ�����ٶ�������
#define START_SPEED (600)
#define START_TIME (100)

void MotoCtrlTime(uint32_t time, int16_t left, int16_t right);

/*******************************������***************************/

void MotoGoStrightCtrl(uint32_t time);

void MotoGoBackCtrl(uint32_t time);

void MotoTurnLeftCtrl(uint32_t time);

void MotoTurnRightCtrl(uint32_t time);

void MotoBreakCtrl(uint32_t time);

/*******************************����***************************/

void MotoGoStrightCtrlBlocking(uint32_t time);

void MotoGoBackCtrlBlocking(uint32_t time);

void MotoTurnLeftCtrlBlocking(uint32_t time);

void MotoTurnRightCtrlBlocking(uint32_t time);

void MotoBreakCtrlBlocking(uint32_t time);

#endif //CAR_2023_MOTORCTRL_H
