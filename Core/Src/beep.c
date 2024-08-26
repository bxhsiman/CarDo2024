#include "main.h"
#include "tim.h"
#include "common.h"
#include "beep.h"


uint8_t g_beep_on = 0 ;
uint16_t g_beep_low_time = 500 ;
uint16_t g_beep_high_time = 500 ;
uint8_t g_beep_state = 0 ;
uint16_t g_beep_time = 0 ;



void Beep_Start( void )
{
	if ( g_beep_on == 0 ) return ;
	if ( g_beep_time == 0 ) 
	{
		if ( g_beep_state == 0 )
		{
			g_beep_state = 1;
			g_beep_time = g_beep_low_time ;
		  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);			
			__HAL_TIM_SET_PRESCALER( &htim1 , 71 );   // 72M / 72 = 1MHz(1 us )
			__HAL_TIM_SET_AUTORELOAD( &htim1 , 1599 );   // 1us *20 = 20us
			__HAL_TIM_SET_COUNTER( &htim1 , 0 );
		  __HAL_TIM_SET_COMPARE( &htim1 , TIM_CHANNEL_1 , 799 );
//			HAL_TIM_GenerateEvent( &htim1 , TIM_EVENTSOURCE_UPDATE);
		  HAL_TIM_PWM_Start( &htim1 , TIM_CHANNEL_1);			
		  //ShowLED(LED_G, LED_OFF ); //TBD: 实现这个功能
		}
		else
		{
			g_beep_state = 0;
			g_beep_time = g_beep_high_time ;
		  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);			
			__HAL_TIM_SET_PRESCALER( &htim1 , 71 );   // 72M / 72 = 1MHz(1 us )
			__HAL_TIM_SET_AUTORELOAD( &htim1 , 599 );   // 1us *20 = 20us
			__HAL_TIM_SET_COUNTER( &htim1 , 0 );
		  __HAL_TIM_SET_COMPARE( &htim1 , TIM_CHANNEL_1 , 299 );
	//		HAL_TIM_GenerateEvent( &htim1 , TIM_EVENTSOURCE_UPDATE);
		  HAL_TIM_PWM_Start( &htim1 , TIM_CHANNEL_1);			
		  // ShowLED(LED_G, LED_ON );	// TBD: implement this function
		}
	}
	else
	{
	  g_beep_time--;
	}

}