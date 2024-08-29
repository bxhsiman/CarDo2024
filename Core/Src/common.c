#include <stdio.h>
#include <string.h>
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "common.h"
#include "uart_dma.h"
#include "usb_uart.h"
#include "esp32.h"
#include "ir_track.h"
#include "car_drive.h"
#include "angle_kalman_filter.h"
#include "adc.h"
#include "btn.h"
#include "user_flash.h"
#include "motor_drive.h"
#include "led_matrix.h"
#include "../Usr/MPU6050.h"

TIM_HandleTypeDef *g_track_time = &htim6;
extern __IO uint8_t g_music_enable;
extern __IO uint8_t g_music_flag;

void MusicProcess(void);

uint8_t g_device_uid[25] = {0};
uint8_t g_SystemMode = SYSTEM_TRACK;
LED_t g_led[] =
        {
                {LED_FL_GPIO_Port, LED_FL_Pin},
                {LED_FR_GPIO_Port, LED_FR_Pin},
                {LED_BL_GPIO_Port, LED_BL_Pin},
                {LED_BR_GPIO_Port, LED_BR_Pin}
        };

__IO uint8_t g_LED_flag = 0;

void SystemModeCtrl(void);

UART_HandleTypeDef *g_user_uart = &huart1;  // printf output

// for keil
int fputc(int ch, FILE *p) {
    HAL_UART_Transmit(g_user_uart, (uint8_t *) &ch, 1, 10);
//	USBTxDataDMA((uint8_t *)&ch, 1);
    return ch;
}

// for gcc
int _write(int fd, char *pBuffer, int size) {
//    UartTxDataDMA(3, (uint8_t *) pBuffer, size);
    HAL_UART_Transmit(&huart3, (uint8_t *) pBuffer, size, 10); //for test when dma tx is stop
    HAL_UART_Transmit(g_user_uart, (uint8_t *) pBuffer, size, 10);
    return size;
}

//1ms interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim6) {
        IR_Track_Start();
    } else if (htim == &htim7) {
        if (g_music_enable == 1) {
            g_music_flag = 1;
        }
        g_LED_flag = 1;
    }

//#ifdef MY_UP_1
    else if (htim == &htim2) // add callback for yaw calculation
    {
        static MPU6050_t data_t;
        MPU6050_Read_Gyro(&data_t);

        float raw_yaw = ((float) (data_t.Gz - gyroZ_offset) / 16.384f);
        g_yaw += 0.005f * raw_yaw * 100 / 6 * 9 / 100 * 90; //magic number

        // 确保g_yaw在-180到180度之间
        if (g_yaw > 180.0f)
        {
            g_yaw -= 360.0f;
        }
        else if (g_yaw < -180.0f)
        {
            g_yaw += 360.0f;
        }
    }
//#endif

}

//////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
}


//////////////////////////////////////////////////////////////////
void InitUSBPort(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin : PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(65);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_Delay(65);
}


static void GetUniqueID(void) {
    uint8_t uid[12] = {0};

    uid[0] = UNI_DEV_SN->SN[11];
    uid[1] = UNI_DEV_SN->SN[10];
    uid[2] = UNI_DEV_SN->SN[9];
    uid[3] = UNI_DEV_SN->SN[8];
    uid[4] = UNI_DEV_SN->SN[7];
    uid[5] = UNI_DEV_SN->SN[6];
    uid[6] = UNI_DEV_SN->SN[5];
    uid[7] = UNI_DEV_SN->SN[4];
    uid[8] = UNI_DEV_SN->SN[3];
    uid[9] = UNI_DEV_SN->SN[2];
    uid[10] = UNI_DEV_SN->SN[1];
    uid[11] = UNI_DEV_SN->SN[0];

    sprintf((char *) g_device_uid, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            uid[0], uid[1], uid[2], uid[3], uid[4], uid[5],
            uid[6], uid[7], uid[8], uid[9], uid[10], uid[11]);
}

uint32_t USB_RxDataCallback(uint8_t *buf, uint32_t len) {
    uint32_t ret = 1;
    if (g_esp32.mode == 1) // work mode
    {
        if ((len >= 5) && (strncmp(buf, "RESET", 5) == 0)) {
            __set_FAULTMASK(1);
            NVIC_SystemReset();
        }
        if ((len >= 5) && (strncmp(buf, "FLASH", 5) == 0)) {
            ESP32_Start(0); // Flash mode
            return ret;
        }
        if ((len >= 4) && (strncmp(buf, "INIT", 4) == 0)) {
            EreaseFlashData(FLASH_BANK1_END - FLASH_PAGE_SIZE + 1, 1);
            __set_FAULTMASK(1);
            NVIC_SystemReset();
        }
        if ((len >= 5) && (strncmp(buf, "MUSIC", 5) == 0)) {
            g_music_enable = 1;
            return 1;
        }
    } else  // flash mode
    {
        if ((len == 4) && (strncmp(buf, "WIFI", 4) == 0)) {
            ESP32_Start(1); // Normal mode
        } else {
            if (g_usb_buf.target_type == TARGET_UART) {
                ret = UartTxDataDMA(g_usb_buf.target_id, buf, len);
            }
        }
    }
    return ret;
}

void System_Init(void) {
    GetUniqueID();
    UserDataInitFromFlash(&g_CarConfig);
    AngleKalmanInit(&g_CarConfig);
    StartAllUartDMAReceive();
    ESP32_Init();
    HAL_TIM_Base_Start_IT(&htim7);
    SystemModeCtrl();
}

void SystemModeCtrl(void) {
    switch (g_SystemMode) {

        case SYSTEM_TRACK:
            HAL_TIM_Base_Stop_IT(g_track_time);
            MX_ADC1_Init();
            IR_Track_Init();
            g_CarCtrl.car_mode = CAR_IDLE;
            HAL_TIM_Base_Start_IT(&htim7);
            break;

        case SYSTEM_DOTMATRIX:
            HAL_TIM_Base_Stop_IT(g_track_time);
            StopAllMoto();
            HAL_ADC_Stop_DMA(&hadc1);
            HAL_TIM_Base_Start_IT(&htim7);
            led_matrix_init();
            break;

        case SYSTEM_CAL_BLACK:
            HAL_ADC_MspInit(&hadc1);
            IR_Track_Init();
            StopAllMoto();
            HAL_TIM_Base_Start_IT(&htim7);
            HAL_TIM_Base_Start_IT(g_track_time);
            break;

        default:
            break;

    }
}

void CheckBtnRight(void) {
    if (BTN_Read_Right() == 1) {
        if (g_SystemMode == SYSTEM_TRACK) {
            //HAL_TIM_Base_Stop_IT(&htim7);
            IR_Track_Init();
            CarCtrlInit(&g_CarConfig);
            __HAL_TIM_SET_PRESCALER(g_track_time, 7199);
            __HAL_TIM_SET_AUTORELOAD(g_track_time, g_CarConfig.adc_interval * 10 - 1);
            HAL_TIM_Base_Start_IT(g_track_time);
            CarMotoCtrl(g_CarConfig.car_speed_set, g_CarConfig.car_speed_set);
        } else if (g_SystemMode == SYSTEM_CAL_BLACK) {
            HAL_TIM_Base_Stop_IT(&htim7);
//            g_CarConfig.adc_compare_gate = g_TrackStatus.total_adc_value / IR_CHANNEL_NUM; //ava

            uint16_t adc_value_max = 0;
            uint16_t adc_value_min = 1600;

            for (int i = 0; i < IR_CHANNEL_NUM; i++) {
                if (g_TrackStatus.ir_adc[i] > adc_value_max) {
                    adc_value_max = g_TrackStatus.ir_adc[i];
                }
                if (g_TrackStatus.ir_adc[i] < adc_value_min) {
                    adc_value_min = g_TrackStatus.ir_adc[i];
                }
            }
            int temp_threshold = (adc_value_max - adc_value_min) / 2;
            if (temp_threshold > 0) {
                g_CarConfig.adc_compare_gate = temp_threshold;
            } else {
                g_CarConfig.adc_compare_gate = 1000;
            }
            printf("adc_compare_gate is set to %d\n", g_CarConfig.adc_compare_gate);
            UserFlashDataWrite(&g_CarConfig);
            HAL_TIM_Base_Start_IT(&htim7);
        }
    }
}

void CheckBtnLeft(void) {
    if (BTN_Read_Left() == 1) {
        if (g_SystemMode < SYSTEM_CAL_BLACK) {
            g_SystemMode++;
        } else {
            g_SystemMode = SYSTEM_TRACK;
        }
        SystemModeCtrl();
    }
}


void LEDCtrl(uint8_t pos, uint8_t mode) {
    int i = 0;
    uint8_t led_pos = pos;
    uint8_t led_mode = mode;

    for (i = 0; i < 4; i++) {
        if (led_pos & 0x01) {
            switch (led_mode & 0x03) {
                case 0 :
                    HAL_GPIO_WritePin(g_led[i].gpio, g_led[i].pin, GPIO_PIN_SET);
                    break;
                case 1 :
                    HAL_GPIO_WritePin(g_led[i].gpio, g_led[i].pin, GPIO_PIN_RESET);
                    break;
                case 2 :
                    HAL_GPIO_TogglePin(g_led[i].gpio, g_led[i].pin);
                    break;
            }
        } else {
            HAL_GPIO_WritePin(g_led[i].gpio, g_led[i].pin, GPIO_PIN_SET);
        }
        led_pos >>= 1;
        led_mode >>= 2;
    }
}

//TBD: add this func
void ShowLED(void) {
    if (g_LED_flag == 0)
        return;
    g_LED_flag = 0;
    switch (g_SystemMode) {
        case SYSTEM_TRACK:
            if (g_CarCtrl.car_mode == CAR_IDLE) {
                LEDCtrl(0x01, 0x02);
            } else {
                LEDCtrl(0x0C, 0xA0);
            }
            break;

        case SYSTEM_DOTMATRIX:
            LEDCtrl(0x02, 0x08);
            break;

        case SYSTEM_CAL_BLACK:
            LEDCtrl(0x08, 0x80);
            break;
        default:
            break;
    }
}

void UserTasks(void) {
    if (g_SystemMode == SYSTEM_DOTMATRIX) {
        show_led_matrix();
        move_led_matrix();
    }
    CheckUartRxData();
    CheckUartTxData();
    CheckUSBRxData();
    CheckUSBTxData();

    CheckBtnLeft();
    CheckBtnRight();
    ShowLED();
    ManualCarCtrl();
    MusicProcess();
}
