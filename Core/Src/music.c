#include "main.h"
#include "tim.h"

#define MUSIC_LENGTH 200


const uint8_t  g_music_bpm = 72 ;
const uint16_t g_music_whole_note = 60*1000 / g_music_bpm * 10 ;

// Freq base on 12Mhz clock
const uint32_t g_music_freq[22] = {
	0,
	45802, 40816, 36364, 34384, 30612, 27273, 24292,
	22945, 20443, 18209, 17241, 15306, 13636, 12170,
	11461, 10213,  9105,  8590,  7653,  6818,  6101 
};

const uint8_t g_music_tone[MUSIC_LENGTH] = {
	5, 
	8,8,8,9,10,12, 10,10,0,10,9, 8,8,6,8,6,8, 5,5,0,0,5, 
 	8,8,8,9,10,12, 13,13,12,12,10,9, 8,8,6,5,8, 8,8,0,0,5, 
	8,8,8,9,10,12, 10,10,0,10,9, 8,8,6,8,6,8, 5,0,0,0,5, 
	8,8,8,9,10,12, 13,13,12,12,10,9, 8,8,6,5,5,9, 8,8,0,0,
	12,12,9,10,10,9, 8,9,10,12,12, 13,15,13,12,10,9,8, 6,10,9,10,9,
	8,9,10,12,12, 8,13,13,15,10,12,10, 9,9,8,9,10,13,12, 12,12,0,0,
	10,12,15,16,17, 8,10,15,13,12,12, 13,12,10,9,8, 6,8,12,10,9,9,
	10,12,15,16,17, 8,10,15,16,15,15, 0,15,13,12,10,9,8, 6,8,12,10,9,9,8,
	8,8,0,0,255	
};
const uint8_t g_music_note[MUSIC_LENGTH] = {
  2, 
	1,2,2,1,2,2, 1,1,1,2,2, 1,2,2,2,2,1, 1,1,1,2,2, 
	1,2,2,1,2,2, 1,2,2,1,2,2, 1,2,2,1,1, 1,1,1,2,2, 
	1,2,2,1,2,2, 1,1,1,2,2, 1,2,2,2,2,1, 1,1,1,2,2, 
	1,1,2,2,2,2, 1,2,2,1,2,2, 1,2,2,2,2,1, 1,1,1,1, 
	1,2,2,1,2,2, 1,2,2,1,1, 1,2,2,2,2,2,2, 1,1,1,2,2,
	1,2,2,1,1, 2,2,2,2,1,2,2, 1,2,2,2,2,2,2, 1,1,1,1,
	1,2,2,1,1, 2,2,2,2,1,1, 1,2,2,1,1, 2,2,2,2,1,1,
	1,2,2,1,1, 2,2,2,2,1,1, 2,2,2,2,2,2,1 ,2,2,2,2,1,2,2,
	1,1,1,1, 0	
};

__IO uint8_t  g_music_enable = 0 ;
__IO uint8_t  g_music_flag = 0 ;

uint8_t  g_music_running = 0 ;

uint16_t g_music_timer_set = 0 ;
uint16_t g_music_index = 0 ;

TIM_HandleTypeDef *g_music_beep = &htim1;
TIM_HandleTypeDef *g_music_time = &htim7;

void MusicProcess( void )
{
	uint8_t i ;
	uint32_t counter ;
	
	if ( g_music_enable == 0 ) 
	{
		if ( g_music_running == 0 ) return ;
		g_music_running = 0 ;
		HAL_TIM_PWM_Stop( g_music_beep ,TIM_CHANNEL_1 );
		return ;
	}
	
	if ( g_music_flag == 1 )
	{
		g_music_timer_set = g_music_whole_note / g_music_note[g_music_index] ;
		
		HAL_TIM_PWM_Stop( g_music_beep ,TIM_CHANNEL_1 );
		__HAL_TIM_SET_PRESCALER( g_music_beep , 5);
		__HAL_TIM_SET_AUTORELOAD( g_music_beep , g_music_freq[g_music_tone[g_music_index]]-1 );
		__HAL_TIM_SET_COUNTER( g_music_beep , 0 );
		__HAL_TIM_SET_COMPARE( g_music_beep ,TIM_CHANNEL_1,g_music_freq[g_music_tone[g_music_index]]/2 );
		HAL_TIM_PWM_Start( g_music_beep ,TIM_CHANNEL_1 );
		
		//HAL_TIM_Base_Stop_IT( &htim3 );
		__HAL_TIM_SET_PRESCALER(g_music_time , 7199 );
		__HAL_TIM_SET_AUTORELOAD( g_music_time , g_music_timer_set );
		//__HAL_TIM_SET_COUNTER( &htim3 , 0 );
		//HAL_TIM_Base_Start_IT( &htim3 );
		g_music_index++;
		if ( g_music_tone[g_music_index] == 255 ) g_music_index = 0 ;
		g_music_flag = 0 ;
		g_music_running = 1 ;
	}
}