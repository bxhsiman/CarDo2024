#include "motor_drive.h"

void MotoCtrlInit(void)
{
	__HAL_TIM_SET_COUNTER(&htim3,0);
	__HAL_TIM_SET_COUNTER(&htim4,0);
}

void MotoLeftFrontCtrl(int16_t speed)
{
	if(speed < 0)
	{
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,-speed);//速度参数小于0，反转电机
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,speed);//速度参数大于0，正转电机
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	}

}
void MotoRightFrontCtrl(int16_t speed)
{
	if(speed < 0)
	{
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,-speed);//速度参数小于0，反转电机
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,speed);//速度参数大于0，正转电机
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	}
}
void MotoLeftBackCtrl(int16_t speed)
{
	if(speed < 0)
	{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,-speed);//速度参数小于0，反转电机
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,speed);//速度参数大于0，正转电机
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	}
}
void MotoRightBackCtrl(int16_t speed)
{
	if(speed < 0)
	{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,-speed);//速度参数小于0，反转电机
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,speed);//速度参数大于0，正转电机
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	}
}

void StopAllMoto(void)
{
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_4);
}
