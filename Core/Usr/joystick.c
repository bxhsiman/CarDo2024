//
// Created by shimmer on 2024/8/28.
//

#include "joystick.h"
#include <string.h>
#include <stdio.h>
#include "car_drive.h"

void Joystick_DataProcess(uint8_t *data,  uint16_t len)
{
    if (len < 10) // ȷ�����ݳ����㹻
        return;

    // ����洢�ٶ�ֵ�ı���
    int speed1 = 0, speed2 = 0;

    // ���ַ����в��� "moving:" ��λ��
    char *start = strstr((char*)data, "moving:");
    if (start != NULL)
    {
        // ʹ�� sscanf �� "moving:" ����ȡ�����ٶ�ֵ
        sscanf(start, "moving:%d,%d", &speed1, &speed2);

        // ��ӡ���������ٶ�ֵ�����ڵ���
        printf("Parsed speeds: %d, %d\n", speed1, speed2);

        int16_t left_speed, right_speed;
        left_speed = LIMIT_MAX((speed1 - speed2), -MAX_JOYSTICK_SPEED, MAX_JOYSTICK_SPEED);
        right_speed = LIMIT_MAX((speed1 + speed2), -MAX_JOYSTICK_SPEED, MAX_JOYSTICK_SPEED);
        CarMotoCtrl(left_speed, right_speed);
    }
}