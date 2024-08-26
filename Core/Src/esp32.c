#include <string.h>
#include "main.h"
#include "esp32.h"
#include "car_drive.h"

esp32_t g_esp32;
extern UART_HandleTypeDef huart1;

/*
state   :  1 -- work , 0 -- flash esp32
*/

void ESP32ModeCtrl(uint8_t state) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // ESP32 power on
    //HAL_GPIO_WritePin(ESP32_PWR_ON_GPIO_Port,ESP32_PWR_ON_Pin,GPIO_PIN_SET);
    //HAL_Delay( 500 ) ;
    GPIO_InitStruct.Pin = ESP32_BOOT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ESP32_BOOT_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(ESP32_BOOT_GPIO_Port, ESP32_BOOT_Pin, GPIO_PIN_RESET);

    // PHY reset
    //HAL_GPIO_WritePin(MAC_CLK_EN_GPIO_Port,MAC_CLK_EN_Pin,GPIO_PIN_RESET);
    // ESP32 reset
    HAL_GPIO_WritePin(ESP32_EN_GPIO_Port, ESP32_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    if (state == 1) {
        HAL_GPIO_WritePin(ESP32_BOOT_GPIO_Port, ESP32_BOOT_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(ESP32_BOOT_GPIO_Port, ESP32_BOOT_Pin, GPIO_PIN_RESET);
    }
    HAL_Delay(100);

    HAL_GPIO_WritePin(ESP32_EN_GPIO_Port, ESP32_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(500);


    if (state == 1) {
        GPIO_InitStruct.Pin = ESP32_BOOT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(ESP32_BOOT_GPIO_Port, &GPIO_InitStruct);
        //HAL_GPIO_WritePin(MAC_CLK_EN_GPIO_Port,MAC_CLK_EN_Pin,GPIO_PIN_SET);
    }

    HAL_GPIO_WritePin(ESP32_BOOT_GPIO_Port, ESP32_BOOT_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

}


void ESP32_Init(void) {
    // ESP32 power on
    HAL_GPIO_WritePin(ESP32_PWR_ON_GPIO_Port, ESP32_PWR_ON_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    g_esp32.mode = 1;  // 1 normal mode , 0 flash mode
    g_esp32.flash_ok = 0;
    ESP32ModeCtrl(g_esp32.mode);
    HAL_Delay(100);

    //user name and pin set
    char bt_name[32] = "@bt_name|";
    char bt_pin[32] = "@bt_pin|";
    strcat(bt_name, g_CarConfig.bt_name + 8);
    strcat(bt_name, "&");

    strcat(bt_pin, g_CarConfig.bt_pwd);
    strcat(bt_pin, "&");

    HAL_UART_Transmit(&huart1, bt_name, strlen(bt_name), 100);
    HAL_Delay(500);
    HAL_UART_Transmit(&huart1, bt_pin, strlen(bt_pin), 100);
}

void ESP32_Start(uint32_t mode) {
    g_esp32.mode = mode;
    ESP32ModeCtrl(mode);
}

