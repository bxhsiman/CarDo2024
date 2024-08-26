#include "led_matrix.h"

#define MATRIX_COL  51
#define COL_NUM     32
#define SHOW_SPEED  50

uint8_t g_sys_mode = 0 ;

uint8_t g_col_start = 0 ;
uint16_t g_col_cnt = 0 ;

uint8_t g_ShowData[ MATRIX_COL ]  = {
	0x81,0xFF,0xFF,0x89,0x89,0xFF,0x76,0x00,
	0x01,0x7F,0xFF,0x80,0x80,0xFF,0x7F,0x01,
	0x00,0x81,0xFF,0xFF,0x91,0x91,0x1F,0x0E,
	0x00,0x07,0x81,0xFF,0xFF,0x81,0x07,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0};

uint32_t g_ShowPos[ COL_NUM ] = {
	0xFFFFFFFE , 0xFFFFFFFD , 0xFFFFFFFB , 0xFFFFFFF7 ,  
	0xFFFFFFEF , 0xFFFFFFDF , 0xFFFFFFBF , 0xFFFFFF7F ,  
	0xFFFFFEFF , 0xFFFFFDFF , 0xFFFFFBFF , 0xFFFFF7FF ,  
	0xFFFFEFFF , 0xFFFFDFFF , 0xFFFFBFFF , 0xFFFF7FFF ,
	0xFFFEFFFF , 0xFFFDFFFF , 0xFFFBFFFF , 0xFFF7FFFF ,  
	0xFFEFFFFF , 0xFFDFFFFF , 0xFFBFFFFF , 0xFF7FFFFF ,  
	0xFEFFFFFF , 0xFDFFFFFF , 0xFBFFFFFF , 0xF7FFFFFF ,  
	0xEFFFFFFF , 0xDFFFFFFF , 0xBFFFFFFF , 0x7FFFFFFF ,
} ;  
	
void led_matrix_init( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t i ;
	
  GPIO_InitStruct.Pin = LED_SHIFT_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_SHIFT_CLK_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = LED_LATCH_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_LATCH_CLK_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = LED_SHIFT_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_SHIFT_DATA_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = SENSOR_PWR_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SENSOR_PWR_ON_GPIO_Port, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
	HAL_GPIO_WritePin( LED_LATCH_CLK_GPIO_Port  , LED_LATCH_CLK_Pin , GPIO_PIN_RESET );
  HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_RESET );
	HAL_GPIO_WritePin( SENSOR_PWR_ON_GPIO_Port  , SENSOR_PWR_ON_Pin , GPIO_PIN_SET );
	for ( i = 0 ; i < (COL_NUM + 8 ) ; i++ )
	{
		HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_SET );
  	HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
	}
  HAL_GPIO_WritePin( LED_LATCH_CLK_GPIO_Port  , LED_LATCH_CLK_Pin , GPIO_PIN_SET );
	HAL_GPIO_WritePin( LED_LATCH_CLK_GPIO_Port  , LED_LATCH_CLK_Pin , GPIO_PIN_RESET );			
}
	
void move_led_matrix( void )
{
  g_col_cnt++ ;
	
	if ( g_col_cnt > SHOW_SPEED )  // 
	{
		g_col_cnt = 0 ;
		if ( g_col_start < ( MATRIX_COL - 1 ) ) g_col_start++ ;
		else g_col_start = 0 ;			
	}
}

void show_led_matrix( void )
{
	uint32_t i , j , k ;
	uint8_t  show_char ;
	uint32_t show_pos  ;
	
	
	for ( i = 0 ; i < COL_NUM ; i++ )
	{
		show_pos = g_ShowPos[i] ;
		for ( k = 0 ; k < 16 ; k++ )
		{
			if ( show_pos & 0x80000000 ) 
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_SET );
			else
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_RESET );
			show_pos = show_pos << 1 ;
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_SET );
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
		}
		
		j = g_col_start + i  ;
		if ( j > ( MATRIX_COL - 1 ) ) j -= MATRIX_COL ;			
		show_char = g_ShowData[j];
		
		for ( k = 0 ; k < 8 ; k++ )
		{
			if ( show_char & 0x80 ) 
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_SET );
			else
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_RESET );
			show_char = show_char << 1 ;
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_SET );
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
		}
		
		for ( k = 0 ; k < 16 ; k++ )
		{
			if ( show_pos & 0x80000000 ) 
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_SET );
			else
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_RESET );
			show_pos = show_pos << 1 ;
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_SET );
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
		}
		
		show_char = g_ShowData[j];
		
		for ( k = 0 ; k < 8 ; k++ )
		{
			if ( show_char & 0x80 ) 
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_SET );
			else
				HAL_GPIO_WritePin( LED_SHIFT_DATA_GPIO_Port  , LED_SHIFT_DATA_Pin , GPIO_PIN_RESET );
			show_char = show_char << 1 ;
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_SET );
			HAL_GPIO_WritePin( LED_SHIFT_CLK_GPIO_Port  , LED_SHIFT_CLK_Pin , GPIO_PIN_RESET );
		}
		
		HAL_GPIO_WritePin( LED_LATCH_CLK_GPIO_Port  , LED_LATCH_CLK_Pin , GPIO_PIN_SET );
		HAL_GPIO_WritePin( LED_LATCH_CLK_GPIO_Port  , LED_LATCH_CLK_Pin , GPIO_PIN_RESET );			
	}
}
