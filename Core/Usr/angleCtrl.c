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
    // �ֲ����������̬����
    float error, derivative, output;
    int16_t speed;

    // ����Ŀ��Ƕ�
    angleCtrl.target = angle;

    // �������
    error = angleCtrl.target - g_yaw;

    // �����������������ֵ��Χ��
    angleCtrl.integral += error;
    angleCtrl.integral = LIMIT_MAX(angleCtrl.integral, -angleCtrl.integral_max, angleCtrl.integral_max);

    // ����΢����
    derivative = error - angleCtrl.last_error;
    angleCtrl.last_error = error;

    // ����PID���
    output = (angleCtrl.Kp * error + angleCtrl.Kd * derivative + angleCtrl.Ki * angleCtrl.integral);

    output = LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // ��������������ٶ�
    speed = (int16_t) LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // ��������ٶ�
    CarMotoCtrl(LIMIT_MIN((int16_t)(base_speed - speed), -angleCtrl.output_min, angleCtrl.output_min),
                LIMIT_MIN((int16_t) (base_speed + speed), -angleCtrl.output_min, angleCtrl.output_min));

    // �ӳٴ���
    static uint32_t delay = 0;
    if (delay > 20)
    {
        HAL_GPIO_TogglePin(LED_BL_GPIO_Port, LED_BR_Pin);
    }
    if (++delay > 200)
    {
        delay = 0;
        printf("err is %.2f \n", error); // ��������ڵ���
    }

    // ���ص�ǰ�����ֵ
    return error;
}

float AnglePID(float error)
{
    // �ֲ����������̬����
    float derivative, output;

    // �����������������ֵ��Χ��
    angleCtrl.integral += error;
    angleCtrl.integral = LIMIT_MAX(angleCtrl.integral, -angleCtrl.integral_max, angleCtrl.integral_max);

    // ����΢����
    derivative = error - angleCtrl.last_error;
    angleCtrl.last_error = error;

    // ����PID���
    output = (angleCtrl.Kp * error + angleCtrl.Kd * derivative + angleCtrl.Ki * angleCtrl.integral);

    output = LIMIT_MAX(output, -angleCtrl.output_max, angleCtrl.output_max);

    // �������
    return output;
}