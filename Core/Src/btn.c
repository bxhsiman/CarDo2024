#include "btn.h"

uint8_t BTN_Read_Left()
{
	static uint8_t key_state = 0;
	
	if ( HAL_GPIO_ReadPin(BTN_L_GPIO_Port,BTN_L_Pin) == GPIO_PIN_RESET )
	{
		if ( key_state == 0 )
		{
			key_state = 1 ;
			return 1 ;
		}
		else
		{
			return  0 ;
		}
	}
	else		
	{
		key_state = 0 ; 
	}
	return 0;
}


uint8_t BTN_Read_Right()
{
	static uint8_t key_state = 0;
	
	if ( HAL_GPIO_ReadPin(BTN_R_GPIO_Port,BTN_R_Pin) == GPIO_PIN_RESET )
	{
		if ( key_state == 0 )
		{
			key_state = 1 ;
			return 1 ;
		}
		else
		{
			return  0 ;
		}
	}
	else		
	{
		key_state = 0 ; 
	}
	return 0;
}
		
