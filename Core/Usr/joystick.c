//
// Created by shimmer on 2024/8/28.
//

#include "joystick.h"
#include <string.h>
#include <stdio.h>
#include "car_drive.h"

void Joystick_DataProcess(uint8_t *data,  uint16_t len)
{
    if (len < 10) // 确保数据长度足够
        return;

    // 定义存储速度值的变量
    int speed1 = 0, speed2 = 0;

    // 在字符串中查找 "moving:" 的位置
    char *start = strstr((char*)data, "moving:");
    if (start != NULL)
    {
        // 使用 sscanf 从 "moving:" 后提取两个速度值
        sscanf(start, "moving:%d,%d", &speed1, &speed2);

        // 打印解析出的速度值，用于调试
        printf("Parsed speeds: %d, %d\n", speed1, speed2);

        int16_t left_speed, right_speed;
        left_speed = LIMIT_MAX((speed1 - speed2), -MAX_JOYSTICK_SPEED, MAX_JOYSTICK_SPEED);
        right_speed = LIMIT_MAX((speed1 + speed2), -MAX_JOYSTICK_SPEED, MAX_JOYSTICK_SPEED);
        CarMotoCtrl(left_speed, right_speed);
    }
}