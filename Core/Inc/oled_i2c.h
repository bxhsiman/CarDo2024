#ifndef __OLED_I2C_H
#define	__OLED_I2C_H
#include "main.h"

#define OLED_ADDRESS      0x78
#define OLED_COMMAND_REG  0x00
#define OLED_DATA_REG     0x40

#define TOPCMD_TRANS  1
#define LOWCMD_TRANS  2
#define TOP_HALF_TRANS 3
#define LOW_HALF_TRANS 4

void OLED_all_on( void );
void OLED_clear( void );
void OLED_init( void );
void OLED_ShowAscii( uint8_t row , uint8_t col , uint8_t * data , uint8_t length , uint8_t inv );
void OLED_Notice( void );
void OLED_Debug(uint8_t *data, uint8_t row , uint8_t col);

extern I2C_HandleTypeDef * g_OLED_i2c ;

#endif
