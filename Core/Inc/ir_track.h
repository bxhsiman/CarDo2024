#ifndef __IR_TRACK_H
#define __IR_TRACK_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

#define IR_CHANNEL_NUM 	5
#define ADC_THRESHOLD		3000  // 1/3 full scale
typedef struct
{
  uint16_t 	ir_adc[IR_CHANNEL_NUM]  ;   
	int     	track_error[32];
	uint16_t 	total_adc_value;
	uint8_t 	adc_value;
	uint8_t 	full_black;
	uint8_t 	full_white;
}track_status_t ;

void IR_Track_Init( void );
void IR_Track_Start( void );
void ADC_NormalCal(void);
void IR_Track_Power_On( void );
void IR_Track_Power_Off( void );
	
extern track_status_t g_TrackStatus;

#ifdef __cplusplus
}
#endif
#endif /*__IR_TRACK_H */
