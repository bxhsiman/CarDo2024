#ifndef __esp32_H
#define __esp32_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

typedef struct
{
	uint8_t mode ;
	uint8_t flash_ok ;
	
}esp32_t ;
	 
void ESP32ModeCtrl( uint8_t state);
void ESP32_Start( uint32_t mode );
void ESP32_Init( void );

extern esp32_t g_esp32 ;

#ifdef __cplusplus
}
#endif
#endif /*__esp32_H */
