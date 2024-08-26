#include <string.h>
#include <stdio.h>
#include "user_flash.h"
#include "common.h"

uint32_t ReadFlashData( uint32_t addr )
{
    uint32_t pValue = *(__IO uint32_t*)(addr);
	  return pValue;

}

void EreaseFlashData(  uint32_t addr , uint32_t size )
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr ;
    f.NbPages = size ;
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&f, &PageError);
    HAL_FLASH_Lock();
}

HAL_StatusTypeDef WriteFlashData_DW( uint32_t addr ,  uint64_t Data   )
{	
    return HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, addr , Data);
}

HAL_StatusTypeDef WriteFlashData( uint32_t addr ,  uint32_t * Data , uint32_t size  )
{	
	uint32_t i ;
	HAL_StatusTypeDef ret ;
  HAL_FLASH_Unlock();
	for ( i = 0 ; i < size ; i ++ )
	{
    ret = HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, addr + i * sizeof( uint32_t) , *(Data+i) );
		if ( ret == HAL_ERROR ) break ;
	}
  HAL_FLASH_Lock();
	return ret ;
}

uint8_t UserFlashCheck(void)
{
	uint32_t flash_start = FLASH_BANK1_END -  FLASH_PAGE_SIZE + 1 ;
	
	if( ReadFlashData(flash_start) != CAR_FLASH_TAG )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

HAL_StatusTypeDef UserFlashDataWrite( car_config_t * p_car_cfg )
{
	flash_data_t flash_data;
	uint32_t flash_start = FLASH_BANK1_END -  FLASH_PAGE_SIZE + 1 ;
	
	flash_data.flash_tag = CAR_FLASH_TAG ;
	memcpy( &flash_data.car_config , p_car_cfg , sizeof(car_config_t) );

	EreaseFlashData( flash_start, 1 );
	
	return WriteFlashData( flash_start , (uint32_t *)&flash_data, sizeof(flash_data_t)/sizeof(uint32_t));
}

uint8_t UserDataInitFromFlash(car_config_t * p_car_cfg)
{
	flash_data_t flash_data;
	uint32_t flash_start = FLASH_BANK1_END -  FLASH_PAGE_SIZE + 1 ;
	uint32_t *ptr = NULL;

	ptr = (uint32_t *)&flash_data;
	
	if(UserFlashCheck())
	{
		for(int i = 0; i < sizeof(flash_data_t)/sizeof(uint32_t) ; i++)
		{
			*ptr = ReadFlashData( flash_start + i * sizeof(uint32_t) );
			ptr++;
		}
		memcpy( p_car_cfg , &flash_data.car_config , sizeof(car_config_t) );
	}
	else
	{
		UserFlashDataWrite(p_car_cfg);
	}
	return 1;
}


