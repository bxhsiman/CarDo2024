//
// Created by shimmer on 2024/8/29.
//

#include "st.h"
#include "car_drive.h"
#include "ir_track.h"
#include "angle_kalman_filter.h"
#include <math.h>
#include "angleCtrl.h"
#include "MPU6050.h"
#include <stdio.h>
#include "motor_drive.h"

uint8_t up2_state = 0;
uint8_t last_state = 0;
uint8_t white_to_black_cnt = 0;
uint8_t black_to_white_cnt = 0;
uint8_t turn_right_cnt = 0;
uint8_t turn_left_cnt = 0;

static void trans_state(uint8_t state)
{
    last_state = up2_state;
    up2_state = state;
    if (last_state == UP2_STATE_TURN_RIGHT || last_state == UP2_STATE_TURN_LEFT)
    {
        if (up2_state == UP2_STATE_BLACK_TRACKING)
        {
            white_to_black_cnt++;
        }
    }
    if (last_state == UP2_STATE_BLACK_TRACKING)
    {
        if (up2_state == UP2_STATE_TURN_RIGHT || up2_state == UP2_STATE_TURN_LEFT)
        {
            black_to_white_cnt++;
        }
    }
    if (up2_state == UP2_STATE_TURN_RIGHT)
    {
        turn_right_cnt++;
    }
    if (up2_state == UP2_STATE_TURN_LEFT)
    {
        turn_left_cnt++;
    }
}

void UP2_StateMachine(void)
{
    static uint32_t st_time = 0;
    st_time++;
    switch (up2_state)
    {
        case UP2_STATE_READY:
        {
            if (g_CarCtrl.car_mode == CAR_TRACKING)
            {
                trans_state(UP2_STATE_TURN_RIGHT);
            }
            break;
        }
        case UP2_STATE_BLACK_TRACKING:
        {
            int last_err_diff;
            last_err_diff = g_TrackStatus.track_error[g_TrackStatus.adc_value] - g_CarCtrl.last_error;
            if (g_CarConfig.kalman_enable)
            {
                KalmanFilter(g_TrackStatus.track_error[g_TrackStatus.adc_value], last_err_diff,
                             g_CarConfig.adc_interval);
            }
            else
            {
                g_CarCtrl.total_err_diff += last_err_diff;
                g_CarCtrl.total_err += g_TrackStatus.track_error[g_TrackStatus.adc_value];
            }
            g_CarCtrl.last_error = g_TrackStatus.track_error[g_TrackStatus.adc_value];

            g_CarCtrl.car_speed = g_CarConfig.car_speed_min;

            g_CarCtrl.track_err_diff = (float) g_CarCtrl.total_err_diff / (float) g_CarConfig.car_ctrl_interval;
            g_CarCtrl.track_err = (float) g_CarCtrl.total_err / (float) g_CarCtrl.car_ctrl_freq;
            CarTrackCtrl();
            g_CarCtrl.total_err_diff = 0.0;
            g_CarCtrl.total_err = 0.0;

            if (g_TrackStatus.full_white == 1)
            {
                if (turn_right_cnt % 2 == 0 || ((int16_t )turn_left_cnt % 2 - 1) == 0)
                {
                    trans_state(UP2_STATE_TURN_RIGHT);
                }
                else
                {
                    trans_state(UP2_STATE_TURN_LEFT);
                }
            }

            break;
        }
        case UP2_STATE_TURN_RIGHT:
        {
            if (turn_right_cnt == 4)
            {
                trans_state(UP2_STATE_STOP);
            }
            int16_t yaw_speed = 0;
            yaw_speed = AnglePID(g_yaw + 42);

            yaw_speed = LIMIT_MAX(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 70)
            {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }
            StopAllMoto();
            CarMotoCtrl(550 + yaw_speed, 550 - yaw_speed);

            if (g_TrackStatus.full_white != 1 && st_time > 1000)
            {
                trans_state(UP2_STATE_BLACK_TRACKING);
            }
            break;
        }
        case UP2_STATE_TURN_LEFT:
        {
            int16_t yaw_speed = 0;
            yaw_speed = AnglePID(g_yaw - (180 - 42));

            yaw_speed = LIMIT_MAX(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 70)
            {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }
            StopAllMoto();
            CarMotoCtrl(550 + yaw_speed, 550 - yaw_speed);

            if (g_TrackStatus.full_white != 1)
            {
                trans_state(UP2_STATE_BLACK_TRACKING);
            }
            break;
        }
        case UP2_STATE_STOP:
        {
            StopAllMoto();
            break;
        }
        default:
        {
            break;
        }
    }
}