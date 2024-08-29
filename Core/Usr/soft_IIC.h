//
// Created by shimmer on 2024/8/28.
//

#ifndef CAR_2023_SOFT_IIC_H
#define CAR_2023_SOFT_IIC_H

#include <stdint.h>
static void I2C_Delay(void);

void I2C_Start(void);
void I2C_Stop(void);
void I2C_WriteBit(uint8_t bit);
uint8_t I2C_ReadBit(void);
void I2C_WriteByte(uint8_t byte);
uint8_t I2C_ReadByte(void);
void I2C_Ack(void);
void I2C_NAck(void);
uint8_t I2C_WaitAck(void);

void I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length);
void I2C_Read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length);

#endif //CAR_2023_SOFT_IIC_H
