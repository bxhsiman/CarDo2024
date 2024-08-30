//
// Created by shimmer on 2024/8/24.
//

#include <stdio.h>
#include "angleCtrl.h"
#include "car_drive.h"
#include "MPU6050.h"

AngleCtrl_t angleCtrl = {
        .Kp = 50,
        .Kd = 0,
        .Ki = 0,
        .target = 0,
        .integral = 0,
        .last_error = 0,
        .output_max = 450,
        .output_min = 300,
        .integral_max = 200,
};

float AngleCtrl(float angle, int16_t base_speed)
{
    // 局部变量替代静态变量
    float error, derivative, output;
    int16_t speed;

    // 设置目标角度
    angleCtrl.target = angle;

    // 计算误差
    error = angleCtrl.target - g_yaw;

    // 计算积分项并限制在最大值范围内
    angleCtrl.integral += error;
    angleCtrl.integral = LIMIT_MAX(angleCtrl.integral, -angleCtrl.integral_max, angleCtrl.integral_max);

    // 计算微分项
    derivative = error - angleCtrl.last_error;
    angleCtrl.last_error = error;

    // 计算PID输出
    output = (angleCtrl.Kp * error + angleCtrl.Kd * derivative + angleCtrl.Ki * angleCtrl.integral);

    output = LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // 限制输出并计算速度
    speed = (int16_t) LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // 调整电机速度
    CarMotoCtrl(LIMIT_MIN((int16_t)(base_speed - speed), -angleCtrl.output_min, angleCtrl.output_min),
                LIMIT_MIN((int16_t) (base_speed + speed), -angleCtrl.output_min, angleCtrl.output_min));

    // 延迟处理
    static uint32_t delay = 0;
    if (delay > 20)
    {
        HAL_GPIO_TogglePin(LED_BL_GPIO_Port, LED_BR_Pin);
    }
    if (++delay > 200)
    {
        delay = 0;
        printf("err is %.2f \n", error); // 输出误差，用于调试
    }

    // 返回当前的误差值
    return error;
}

float AnglePID(float error)
{
    // 局部变量替代静态变量
    float derivative, output;

    // 计算积分项并限制在最大值范围内
    angleCtrl.integral += error;
    angleCtrl.integral = LIMIT_MAX(angleCtrl.integral, -angleCtrl.integral_max, angleCtrl.integral_max);

    // 计算微分项
    derivative = error - angleCtrl.last_error;
    angleCtrl.last_error = error;

    // 计算PID输出
    output = (angleCtrl.Kp * error + angleCtrl.Kd * derivative + angleCtrl.Ki * angleCtrl.integral);

    output = LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // 限制输出
    return output;
}