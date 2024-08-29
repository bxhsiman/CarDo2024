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
#include "adc.h"

extern __IO uint8_t g_music_enable;

uint8_t up2_state = 0;
uint8_t last_state = 0;
uint8_t white_to_black_cnt = 0;
uint8_t black_to_white_cnt = 0;
uint8_t turn_right_cnt = 0;
uint8_t turn_left_cnt = 0;

static void trans_state(uint8_t state) {

    last_state = up2_state;
    up2_state = state;

    printf("StateChange: %d -> %d \n", last_state, state);

    if (last_state == UP2_STATE_FINDLINE_RIGHT || last_state == UP2_STATE_FINDLINE_LEFT) {
        if (state == UP2_STATE_BLACK_TRACKING) {
            white_to_black_cnt++;
        }
    }
    if (last_state == UP2_STATE_BLACK_TRACKING) {
        if (state == UP2_STATE_TURN_RIGHT || state == UP2_STATE_TURN_LEFT) {
            black_to_white_cnt++;
        }
    }
    if (state == UP2_STATE_TURN_RIGHT) {
        turn_right_cnt++;
    }
    if (state == UP2_STATE_TURN_LEFT) {
        turn_left_cnt++;
    }
}

void UP2_StateMachine(void) {
    static uint32_t st_time = 0;
    st_time++;
    switch (up2_state) {
        case UP2_STATE_READY: {
            white_to_black_cnt = 0;
            black_to_white_cnt = 0;
            turn_right_cnt = 0;
            turn_left_cnt = 0;
            if (g_CarCtrl.car_mode == CAR_TRACKING) {
                trans_state(UP2_STATE_TURN_RIGHT);
            }
            break;
        }
        case UP2_STATE_BLACK_TRACKING: {
            g_music_enable = 1;
            int last_err_diff;
            last_err_diff = g_TrackStatus.track_error[g_TrackStatus.adc_value] - g_CarCtrl.last_error;
            if (g_CarConfig.kalman_enable) {
                KalmanFilter(g_TrackStatus.track_error[g_TrackStatus.adc_value], last_err_diff,
                             g_CarConfig.adc_interval);
            } else {
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

            if (g_TrackStatus.full_white == 1) {
                static uint32_t enter_times = 0;
                if (++enter_times > 500) {
                    enter_times = 0;
                    g_music_enable = 0;
                    if (white_to_black_cnt % 2 == 0) {
                        trans_state(UP2_STATE_TURN_RIGHT);
                    } else {
                        trans_state(UP2_STATE_TURN_LEFT);
                    }
                }
            }
            break;
        }
        case UP2_STATE_TURN_RIGHT: {
            if (turn_right_cnt == 4) {
                trans_state(UP2_STATE_STOP);
            }
            int16_t yaw_speed = 0;
            yaw_speed = AnglePID(g_yaw + 45);

            yaw_speed = LIMIT_MAX(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 100) {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }

            StopAllMoto();
            CarMotoCtrl(400 + yaw_speed, 400 - yaw_speed);

            static uint32_t enter_times = 0;
            if (g_TrackStatus.full_white != 1) {
                enter_times++;
                if (st_time > 1000 && enter_times > 100) {
                    trans_state(UP2_STATE_FINDLINE_RIGHT);
                }
            }
            break;
        }
        case UP2_STATE_TURN_LEFT: {
            int16_t yaw_speed = 0;
            int16_t target_yaw = -135; // must be negative
            yaw_speed = AnglePID(g_yaw - (180 - 45));
            yaw_speed = AnglePID(g_yaw > 0 ? (target_yaw - g_yaw) : (g_yaw - target_yaw));

            yaw_speed = LIMIT_MAX(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 100) {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }
            StopAllMoto();
            CarMotoCtrl(400 + yaw_speed, 400 - yaw_speed);

            static uint32_t enter_times = 0;
            if (g_TrackStatus.full_white != 1) {
                enter_times++;
                if (enter_times > 100) {
                    trans_state(UP2_STATE_FINDLINE_LEFT);
                }
            }
            break;
        }

        case UP2_STATE_FINDLINE_RIGHT: {
            // find line at target is 0��
            int16_t yaw_speed = 0;
            yaw_speed = AnglePID(g_yaw);
            yaw_speed = LIMIT_MAX(yaw_speed, -500, 500);
            yaw_speed = LIMIT_MIN(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 100) {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }

            StopAllMoto();
            CarMotoCtrl(0 + yaw_speed, 0 - yaw_speed);

            if (g_yaw < 20 && g_yaw > -20) {
                trans_state(UP2_STATE_BLACK_TRACKING);
            }
            break;
        }

        case UP2_STATE_FINDLINE_LEFT: {
            // find line at target is -180��
            int16_t yaw_speed = 0;
            yaw_speed = AnglePID(g_yaw < 0 ? (g_yaw + 180) : (g_yaw - 180));
            yaw_speed = LIMIT_MAX(yaw_speed, -500, 500);
            yaw_speed = LIMIT_MIN(yaw_speed, -300, 300);
            static uint32_t t = 0;
            if (++t > 100) {
                t = 0;
                printf("yaw is %.2f\n", g_yaw);
                printf("yaw speed is %d\n", yaw_speed);
            }

            StopAllMoto();
            CarMotoCtrl(0 + yaw_speed, 0 - yaw_speed);

            if (-((fabs(g_yaw) - 180)) < 20) {
                trans_state(UP2_STATE_BLACK_TRACKING);
            }
            break;
        }


        case UP2_STATE_STOP: {

            HAL_TIM_Base_Stop_IT(&htim6);
            StopAllMoto();
            CarMotoCtrl(0, 0);
            HAL_ADC_Stop_DMA(&hadc1);
            IR_Track_Power_Off();
            HAL_TIM_Base_Start_IT(&htim7);
            g_CarCtrl.car_mode = CAR_IDLE;
            break;
        }
        default: {
            break;
        }
    }

}