#ifndef __ANGLE_KALMAN_FILTER_H
#define __ANGLE_KALMAN_FILTER_H

#include "main.h"
#include "car_drive.h"

typedef struct
{
	float cov[2][2];
	float stateTrans[2][2];
	float trans_cov[2][2];
	float ctrl[2];
	float obs[2];
	float angle_a;
	float obs_noise;
	float forecast_angle;
	float forecast_diff;
	float last_angle;
	float last_diff;
}kalman_param_t;

void AngleKalmanInit(car_config_t * p_car_cfg);
void KalmanFilter(float angle, float angle_diff , uint8_t	adc_interval );

extern kalman_param_t g_AngleKalman;

#endif
