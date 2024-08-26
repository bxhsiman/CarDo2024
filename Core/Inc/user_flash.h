#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#include "main.h"
#include "car_drive.h"

#define CAR_FLASH_TAG	0xAA5555AB
#pragma pack(4)
typedef struct
{
	uint32_t 			flash_tag;
	car_config_t 	car_config;
}flash_data_t;

void UserFlashInit(void);
uint8_t UserFlashCheck(void);
void EreaseFlashData(  uint32_t addr , uint32_t size );
HAL_StatusTypeDef UserFlashDataWrite(car_config_t * p_car_cfg );
uint8_t UserDataInitFromFlash(car_config_t * p_car_cfg);

#endif
