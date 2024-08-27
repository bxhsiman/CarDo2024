#include "angle_kalman_filter.h"
#include "common.h"

kalman_param_t g_AngleKalman;

void AngleKalmanInit(car_config_t *p_car_cfg) {
    g_AngleKalman.cov[0][0] = 1;
    g_AngleKalman.cov[0][1] = 0;
    g_AngleKalman.cov[1][0] = 0;
    g_AngleKalman.cov[1][1] = 1;

    g_AngleKalman.stateTrans[0][0] = 1;
    g_AngleKalman.stateTrans[0][1] = p_car_cfg->adc_interval;
    g_AngleKalman.stateTrans[1][0] = 0;
    g_AngleKalman.stateTrans[1][1] = 1;

    g_AngleKalman.trans_cov[0][0] = p_car_cfg->kalman_confidence[0];
    g_AngleKalman.trans_cov[0][1] = 0;
    g_AngleKalman.trans_cov[1][0] = 0;
    g_AngleKalman.trans_cov[1][1] = p_car_cfg->kalman_confidence[1];

    g_AngleKalman.ctrl[0] = (float) (p_car_cfg->adc_interval * p_car_cfg->adc_interval) / 2;
    g_AngleKalman.ctrl[1] = (float) p_car_cfg->adc_interval;

    g_AngleKalman.obs[0] = 1;
    g_AngleKalman.obs[1] = 1;

    g_AngleKalman.obs_noise = 0;
    g_AngleKalman.angle_a = 0;

    g_AngleKalman.forecast_angle = 0;
    g_AngleKalman.forecast_diff = 0;

    g_AngleKalman.last_angle = 0;
    g_AngleKalman.last_diff = 0;
}

void KalmanFilter(float angle, float angle_diff, float adc_interval) {
    float pre_cov[2][2];
    float factor[2];
    float temp_cov00, temp_cov01, temp_cov10, temp_cov11;

    // ������ٶ�
    g_AngleKalman.angle_a = (angle_diff - g_AngleKalman.last_diff) / adc_interval;

    // Ԥ��ǶȺͽ��ٶ�
    g_AngleKalman.forecast_angle = g_AngleKalman.last_angle + g_AngleKalman.stateTrans[0][1] * g_AngleKalman.last_diff +
                                   g_AngleKalman.ctrl[0] * g_AngleKalman.angle_a;
    g_AngleKalman.forecast_diff = g_AngleKalman.last_diff + g_AngleKalman.ctrl[1] * g_AngleKalman.angle_a;

    // ����Ԥ��Э�������
    temp_cov00 = g_AngleKalman.stateTrans[0][0] * g_AngleKalman.cov[0][0] +
                 g_AngleKalman.stateTrans[0][1] * g_AngleKalman.cov[1][0];
    temp_cov01 = g_AngleKalman.stateTrans[0][0] * g_AngleKalman.cov[0][1] +
                 g_AngleKalman.stateTrans[0][1] * g_AngleKalman.cov[1][1];
    temp_cov10 = g_AngleKalman.stateTrans[1][0] * g_AngleKalman.cov[0][0] +
                 g_AngleKalman.stateTrans[1][1] * g_AngleKalman.cov[1][0];
    temp_cov11 = g_AngleKalman.stateTrans[1][0] * g_AngleKalman.cov[0][1] +
                 g_AngleKalman.stateTrans[1][1] * g_AngleKalman.cov[1][1];

    pre_cov[0][0] = temp_cov00 + g_AngleKalman.trans_cov[0][0];
    pre_cov[0][1] = temp_cov01;
    pre_cov[1][0] = temp_cov10;
    pre_cov[1][1] = temp_cov11 + g_AngleKalman.trans_cov[1][1];

    // ���㿨������������
    float denominator = (pre_cov[0][0] + pre_cov[0][1] + pre_cov[1][0] + pre_cov[1][1] + g_AngleKalman.obs_noise);
    if (denominator != 0) { // ���������
        factor[0] = (pre_cov[0][0] + pre_cov[0][1]) / denominator;
        factor[1] = (pre_cov[1][0] + pre_cov[1][1]) / denominator;
    } else {
        factor[0] = 0.0f;
        factor[1] = 0.0f;
    }

    // ���¹��ƽǶȺͽ��ٶ�
    float innovation = (angle + angle_diff) - (g_AngleKalman.forecast_angle + g_AngleKalman.forecast_diff);
    g_AngleKalman.last_angle = g_AngleKalman.forecast_angle + factor[0] * innovation;
    g_AngleKalman.last_diff = g_AngleKalman.forecast_diff + factor[1] * innovation;

    // ����Э�������
    g_AngleKalman.cov[0][0] = (1.0f - factor[0]) * pre_cov[0][0] - factor[0] * pre_cov[1][0];
    g_AngleKalman.cov[0][1] = (1.0f - factor[0]) * pre_cov[0][1] - factor[0] * pre_cov[1][1];
    g_AngleKalman.cov[1][0] = (-factor[1] * pre_cov[0][0]) + (1.0f - factor[1]) * pre_cov[1][0];
    g_AngleKalman.cov[1][1] = (-factor[1] * pre_cov[0][1]) + (1.0f - factor[1]) * pre_cov[1][1];
}
//
//void KalmanFilter(float angle, float angle_diff , float adc_interval )
//{
//	float pre_cov[2][2];
//	float factor[2];
//
//	g_AngleKalman.angle_a = ( angle_diff - g_AngleKalman.last_diff ) / adc_interval ;
//	g_AngleKalman.forecast_angle = g_AngleKalman.last_angle + g_AngleKalman.stateTrans[0][1] * g_AngleKalman.last_diff + g_AngleKalman.ctrl[0] * g_AngleKalman.angle_a;
//	g_AngleKalman.forecast_diff = g_AngleKalman.last_diff + g_AngleKalman.ctrl[1] * g_AngleKalman.angle_a;
//
//	pre_cov[0][0] = g_AngleKalman.stateTrans[0][0] * g_AngleKalman.cov[0][0] + g_AngleKalman.stateTrans[0][1] * g_AngleKalman.cov[1][0];
//	pre_cov[0][1] = g_AngleKalman.stateTrans[0][0] * g_AngleKalman.cov[0][1] + g_AngleKalman.stateTrans[0][1] * g_AngleKalman.cov[1][1];
//	pre_cov[1][0] = g_AngleKalman.stateTrans[1][0] * g_AngleKalman.cov[0][0] + g_AngleKalman.stateTrans[1][1] * g_AngleKalman.cov[1][0];
//	pre_cov[1][1] = g_AngleKalman.stateTrans[1][0] * g_AngleKalman.cov[0][1] + g_AngleKalman.stateTrans[1][1] * g_AngleKalman.cov[1][1];
//
//	pre_cov[0][0] = pre_cov[0][0] * g_AngleKalman.cov[0][0] + pre_cov[0][1] * g_AngleKalman.cov[0][1] + g_AngleKalman.trans_cov[0][0];
//	pre_cov[0][1] = pre_cov[0][0] * g_AngleKalman.cov[1][0] + pre_cov[0][1] * g_AngleKalman.cov[1][1];
//	pre_cov[1][0] = pre_cov[1][0] * g_AngleKalman.cov[0][0] + pre_cov[1][1] * g_AngleKalman.cov[0][1];
//	pre_cov[1][1] = pre_cov[1][0] * g_AngleKalman.cov[1][0] + pre_cov[1][1] * g_AngleKalman.cov[1][1] + g_AngleKalman.trans_cov[1][1];
//
//	factor[0] = (pre_cov[0][0] + pre_cov[0][1]) * (pre_cov[0][0] + pre_cov[0][1] + pre_cov[1][0] + pre_cov[1][1] + g_AngleKalman.obs_noise);
//	factor[1] = (pre_cov[1][0] + pre_cov[1][1]) * (pre_cov[0][0] + pre_cov[0][1] + pre_cov[1][0] + pre_cov[1][1] + g_AngleKalman.obs_noise);
//
//	g_AngleKalman.last_angle = g_AngleKalman.forecast_angle + factor[0] * ((angle + angle_diff) - (g_AngleKalman.forecast_angle + g_AngleKalman.forecast_diff));
//	g_AngleKalman.last_diff = g_AngleKalman.forecast_diff + factor[1] * ((angle + angle_diff) - (g_AngleKalman.forecast_angle + g_AngleKalman.forecast_diff));
//
//	g_AngleKalman.cov[0][0] = (1 - factor[0]) * pre_cov[0][0] - factor[0] * pre_cov[1][0];
//	g_AngleKalman.cov[0][1] = (1 - factor[0]) * pre_cov[0][1] - factor[0] * pre_cov[1][1];
//	g_AngleKalman.cov[1][0] = ( -factor[1] * pre_cov[0][0] ) + (1 - factor[1]) * pre_cov[1][0];
//	g_AngleKalman.cov[1][1] = ( -factor[1] * pre_cov[0][1] ) + (1 - factor[1]) * pre_cov[1][1];
//}


