//
// Created by shimmer on 2024/8/28.
//

#include "soft_IIC.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "tim.h"

// 模拟I2C的时钟和数据引脚
#define I2C_SCL_PIN  SW_SCL_Pin
#define I2C_SDA_PIN  SW_SDA_Pin
#define I2C_GPIO_PORT  SW_SCL_GPIO_Port      // 假设使用GPIOB

#define I2C_DELAY() DelayUs(4)

static void DelayUs(uint32_t us)
{
    (&htim8)->Instance->CNT = (0x0000efff -us*8);//  __HAL_TIM_SET_COUNTER(&htim3,differ);

//  HAL_TIM_Base_Start(&htim3);
    SET_BIT(TIM8->CR1, TIM_CR1_CEN);

//__HAL_TIM_GET_COUNTER(&htim3);
    while(((&htim8)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT)
    {

    }
//  HAL_TIM_Base_Stop(&htim3);
    CLEAR_BIT(TIM8->CR1, TIM_CR1_CEN);
}
void I2C_Start(void) {
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
}

void I2C_Stop(void) {
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
}

void I2C_WriteBit(uint8_t bit) {
    if (bit) {
        HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    }
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
}

uint8_t I2C_ReadBit(void) {
    uint8_t bit;
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET); // 释放SDA
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    bit = HAL_GPIO_ReadPin(I2C_GPIO_PORT, I2C_SDA_PIN);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    return bit;
}

void I2C_WriteByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        I2C_WriteBit(byte & 0x80);
        byte <<= 1;
    }
    I2C_WaitAck();
}

uint8_t I2C_ReadByte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        byte <<= 1;
        byte |= I2C_ReadBit();
    }
    return byte;
}

void I2C_Ack(void) {
    I2C_WriteBit(0);
}

void I2C_NAck(void) {
    I2C_WriteBit(1);
}

uint8_t I2C_WaitAck(void) {
    uint8_t ack;
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET); // 释放SDA
    I2C_DELAY();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    ack = HAL_GPIO_ReadPin(I2C_GPIO_PORT, I2C_SDA_PIN);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    return ack == 0 ? 1 : 0;
}

void I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length) {
    I2C_Start();
    I2C_WriteByte(devAddr);
    I2C_WriteByte(regAddr);
    for (uint8_t i = 0; i < length; i++) {
        I2C_WriteByte(data[i]);
    }
    I2C_Stop();
}

void I2C_Read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length) {
    I2C_Start();
    I2C_WriteByte(devAddr);
    I2C_WriteByte(regAddr);
    I2C_Start(); // Repeated start
    I2C_WriteByte(devAddr | 0x01); // Read mode
    for (uint8_t i = 0; i < length; i++) {
        data[i] = I2C_ReadByte();
        if (i == length - 1) {
            I2C_NAck();
        } else {
            I2C_Ack();
        }
    }
    I2C_Stop();
}