#include "main.h"
#include "adc.h"
#include "car_drive.h"
#include "string.h"
#include "ir_track.h"
#include "angle_kalman_filter.h"
#include "common.h"

ADC_HandleTypeDef * g_ir_track_adc = &hadc1;
track_status_t g_TrackStatus  ;

void IR_Track_Init( void )
{
	memset( (void*)&g_TrackStatus , 0 , sizeof(g_TrackStatus));
	// 00100 				line is middle and no error
#ifdef MY_UP_1_NO_GYRO
	g_TrackStatus.track_error[4] = 0;			//00100
	// 00010/01000 	line is left or right one sensor position, error is 18mm
	g_TrackStatus.track_error[2] = -18;		//00010
	g_TrackStatus.track_error[8] = 18;		//01000
	// 00001/10000 	line is left or right two sensor position, error is 36mm
	g_TrackStatus.track_error[1]  = -30;	//00001 36
	g_TrackStatus.track_error[16] = 30;	//10000 -36
	// 00110/01100 	line is left or right about half sensor position, error is 9mm
	g_TrackStatus.track_error[6]  = -9;	//00110
	g_TrackStatus.track_error[12] = 9;	//01100
	// 00011/11000 	line is left or right one and half sensor position, error is 27mm
	g_TrackStatus.track_error[3]  = -25;	//00011 -27
	g_TrackStatus.track_error[24] = 25;	//11000 27
	// 00111/11100 	line is left or right one sensor position, error is 18mm
	g_TrackStatus.track_error[7]  = -18;	//00111
	g_TrackStatus.track_error[28] = 18;	//11100
	// 11111/00000 	all black or white , start line or no line found , no error , keep straint
	g_TrackStatus.track_error[0]  = 0;		//00000
	g_TrackStatus.track_error[31] = 0;	//11111
#endif
#ifdef GIVE_UP_DOWN
    // 00110 0
    g_TrackStatus.track_error[6] = 0;
    // 00010 -9
    g_TrackStatus.track_error[2] = -9;
    // 00100 9
    g_TrackStatus.track_error[4] = 9;
    // 00011 -18
    g_TrackStatus.track_error[3] = -22;
    // 01100 18
    g_TrackStatus.track_error[12] = 18;
    // 00001 -27
    g_TrackStatus.track_error[1] = -22;
    // 01000 27
    g_TrackStatus.track_error[8] = 27;
    // 00111 -36
    g_TrackStatus.track_error[7] = 0;
    // 01110 36 //?
    g_TrackStatus.track_error[14] = 0;
    // 11100 40
    g_TrackStatus.track_error[28] = 40;

    // 11111/00000 	all black or white , start line or no line found , no error , keep straint
    g_TrackStatus.track_error[0]  = 0;		//00000
    g_TrackStatus.track_error[31] = 0;	//11111


#endif

	#ifdef STM32L476xx
		HAL_ADCEx_Calibration_Start( g_ir_track_adc , ADC_SINGLE_ENDED );
	#els
		HAL_ADCEx_Calibration_Start( g_ir_track_adc );
	#endif
	IR_Track_Power_On();
}

void IR_Track_Power_On( void )
{
	HAL_GPIO_WritePin(SENSOR_PWR_ON_GPIO_Port, SENSOR_PWR_ON_Pin, GPIO_PIN_SET);
}

void IR_Track_Power_Off( void )
{
	HAL_GPIO_WritePin(SENSOR_PWR_ON_GPIO_Port, SENSOR_PWR_ON_Pin, GPIO_PIN_RESET);
}

void IR_Track_Start( void )
{
	if ( HAL_ADC_Start_DMA( g_ir_track_adc , (uint32_t*)&g_TrackStatus.ir_adc[0] , IR_CHANNEL_NUM ) != HAL_OK )
	{
		return  ;
	}
}

/*
 *     IR Track ADC Value process
 *     @brief adc值二值化 全黑全白判断
 */
void ADC_NormalCal(void)
{
	g_TrackStatus.adc_value = 0;
	g_TrackStatus.total_adc_value = 0 ;
	g_TrackStatus.full_white = 0;
	g_TrackStatus.full_black= 0;
	for(int i = 0; i < IR_CHANNEL_NUM; i++)
	{
		g_TrackStatus.adc_value <<= 1;
		if(g_TrackStatus.ir_adc[i] > g_CarConfig.adc_compare_gate ) // easy condition
//		if(abs(g_TrackStatus.ir_adc[i] - g_CarConfig.adc_compare_gate) < 300 ) // adcThreshold
		{
			g_TrackStatus.adc_value |= 0x01 ;
		}
		g_TrackStatus.total_adc_value += g_TrackStatus.ir_adc[i] ;
	}
	if(g_TrackStatus.adc_value == 0)
	{
		g_TrackStatus.full_white = 1;
	}
	else if(g_TrackStatus.adc_value == 31)
	{
		g_TrackStatus.full_black = 1;
	}
}



