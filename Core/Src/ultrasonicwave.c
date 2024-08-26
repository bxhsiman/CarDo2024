#include "main.h"
#include "tim.h"
#include "oled_i2c.h"
#include "UltrasonicWave.h"
extern float g_DistanceSet; 

volatile uint8_t g_DistanceState = 0 ;
volatile uint16_t g_DistanceCounter = 0 ;
float g_Distance = 0 ;

void UltrasonicWave_Trig_Start( void ) 
{
	UltrasonicWave_ShowDistance();
	HAL_GPIO_WritePin( TRIG_F_GPIO_Port , TRIG_F_Pin , GPIO_PIN_SET );
	g_DistanceState =  1 ;
	g_DistanceCounter = 0 ;
	
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);	
	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_SET_PRESCALER( &htim1 , 71 );   // 72M / 72 = 1MHz(1 us )
	__HAL_TIM_SET_AUTORELOAD( &htim1 , 19 );   // 1us *20 = 20us
	__HAL_TIM_SET_COUNTER( &htim1 , 0 );
	HAL_TIM_GenerateEvent( &htim1 , TIM_EVENTSOURCE_UPDATE);
	HAL_TIM_Base_Start_IT(&htim1);
}

void UltrasonicWave_Trig_Stop( void ) 
{
	HAL_GPIO_WritePin( TRIG_F_GPIO_Port , TRIG_F_Pin , GPIO_PIN_RESET );
	g_DistanceState =  2 ;
	
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);	
	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_SET_PRESCALER( &htim1 , 71 );   // 72M / 72 = 1MHz(1 us )
	__HAL_TIM_SET_AUTORELOAD( &htim1 , 49999 );   // 1us *50000 = 50ms
	__HAL_TIM_SET_COUNTER( &htim1 , 0 );
	HAL_TIM_GenerateEvent( &htim1 , TIM_EVENTSOURCE_UPDATE);
	HAL_TIM_Base_Start_IT(&htim1);
}

void UltrasonicWave_Echo_Start( void ) 
{
	g_DistanceState =  3 ;
	
  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);	
	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_SET_PRESCALER( &htim1 , 71 );   // 72M / 72 = 1MHz(1 us )
	__HAL_TIM_SET_AUTORELOAD( &htim1 , 49999 );   // 1us *50000 = 50ms
	__HAL_TIM_SET_COUNTER( &htim1 , 0 );
	HAL_TIM_GenerateEvent( &htim1 , TIM_EVENTSOURCE_UPDATE);
	HAL_TIM_Base_Start_IT(&htim1);
}

void UltrasonicWave_Error( void ) 
{
	g_DistanceState = 0 ;
	g_DistanceCounter = 0xFFFF ;

  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);	
	HAL_TIM_Base_Stop_IT(&htim1);
}

void UltrasonicWave_Echo_Stop( void ) 
{
	g_DistanceState =  0 ;
	g_DistanceCounter = __HAL_TIM_GET_COUNTER( &htim1 );
  HAL_TIM_PWM_Stop( &htim1 , TIM_CHANNEL_1);	
	HAL_TIM_Base_Stop_IT(&htim1);
	g_Distance = g_DistanceCounter * 0.17 ; // 340000 mm / 1000000us / 2 = 0.17 mm/us
//	if ( g_Distance < 490 )
//	{
//		g_DistanceSet = -200 ;
//	}
//	else if ( g_Distance > 510 )
//	{
//	  g_DistanceSet = 200 ;
//	}
//	else
//	{
//		g_DistanceSet = 0 ;
//	}
}

void UltrasonicWave_ShowDistance( void ) 
{
	uint8_t buf[8];
	sprintf( buf , "%04dmm" , (int16_t)g_Distance );
//	OLED_ShowAscii( 1,0,buf ,strlen(buf) );
}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if ( GPIO_Pin == ECHO_F_Pin )
//	{
//	  if ( HAL_GPIO_ReadPin( ECHO_F_GPIO_Port , ECHO_F_Pin ) == GPIO_PIN_SET )
//		{
//			if ( g_DistanceState == 2 )
//			{
//				UltrasonicWave_Echo_Start();
//			}
//		}
//		else
//		{
//			if ( g_DistanceState == 3 )
//			{
//				UltrasonicWave_Echo_Stop();
//			}			
//		}
//	}
//}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	static uint8_t freq_count = 0 ;
//	
//  if ( htim == &htim1 )  // 100KHz conter ( 10 us )
//	{
//		if ( g_DistanceState == 1 ) // start measurement
//		{
//			UltrasonicWave_Trig_Stop();
//		}
//	}
//}
