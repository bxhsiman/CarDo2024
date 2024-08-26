#include "oled_i2c.h"
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
const uint8_t ASCII[][16] = 
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
{0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x00},/*"!",1*/
{0x00,0x10,0x0C,0x02,0x10,0x0C,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
{0x00,0x40,0xC0,0x78,0x40,0xC0,0x78,0x00,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x00},/*"#",3*/
{0x00,0x70,0x88,0x88,0xFC,0x08,0x30,0x00,0x00,0x18,0x20,0x20,0xFF,0x21,0x1E,0x00},/*"$",4*/
{0xF0,0x08,0xF0,0x80,0x60,0x18,0x00,0x00,0x00,0x31,0x0C,0x03,0x1E,0x21,0x1E,0x00},/*"%",5*/
{0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x2C,0x19,0x27,0x21,0x10},/*"&",6*/
{0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",7*/
{0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00},/*"(",8*/
{0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00},/*")",9*/
{0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00},/*"*",10*/
{0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x0F,0x01,0x01,0x01},/*"+",11*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x70,0x00,0x00,0x00,0x00,0x00},/*",",12*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00},/*"-",13*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00},/*".",14*/
{0x00,0x00,0x00,0x00,0xC0,0x38,0x04,0x00,0x00,0x60,0x18,0x07,0x00,0x00,0x00,0x00},/*"/",15*/
{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00},/*"0",16*/
{0x00,0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00},/*"1",17*/
{0x00,0x70,0x08,0x08,0x08,0x08,0xF0,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00},/*"2",18*/
{0x00,0x30,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x18,0x20,0x21,0x21,0x22,0x1C,0x00},/*"3",19*/
{0x00,0x00,0x80,0x40,0x30,0xF8,0x00,0x00,0x00,0x06,0x05,0x24,0x24,0x3F,0x24,0x24},/*"4",20*/
{0x00,0xF8,0x88,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x20,0x20,0x20,0x11,0x0E,0x00},/*"5",21*/
{0x00,0xE0,0x10,0x88,0x88,0x90,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x20,0x1F,0x00},/*"6",22*/
{0x00,0x18,0x08,0x08,0x88,0x68,0x18,0x00,0x00,0x00,0x00,0x3E,0x01,0x00,0x00,0x00},/*"7",23*/
{0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00},/*"8",24*/
{0x00,0xF0,0x08,0x08,0x08,0x10,0xE0,0x00,0x00,0x01,0x12,0x22,0x22,0x11,0x0F,0x00},/*"9",25*/
{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00},/*":",26*/
{0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00},/*";",27*/
{0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00},/*"<",28*/
{0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x00},/*"=",29*/
{0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00},/*">",30*/
{0x00,0x70,0x48,0x08,0x08,0x88,0x70,0x00,0x00,0x00,0x00,0x30,0x37,0x00,0x00,0x00},/*"?",31*/
{0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x28,0x2F,0x28,0x17,0x00},/*"@",32*/
{0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20},/*"A",33*/
{0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00},/*"B",34*/
{0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00},/*"C",35*/
{0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00},/*"D",36*/
{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00},/*"E",37*/
{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00},/*"F",38*/
{0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00},/*"G",39*/
{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20},/*"H",40*/
{0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"I",41*/
{0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00},/*"J",42*/
{0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00},/*"K",43*/
{0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00},/*"L",44*/
{0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x01,0x3E,0x01,0x3F,0x20,0x00},/*"M",45*/
{0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00},/*"N",46*/
{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00},/*"O",47*/
{0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00},/*"P",48*/
{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x28,0x28,0x30,0x50,0x4F,0x00},/*"Q",49*/
{0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20},/*"R",50*/
{0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00},/*"S",51*/
{0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},/*"T",52*/
{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},/*"U",53*/
{0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00},/*"V",54*/
{0x08,0xF8,0x00,0xF8,0x00,0xF8,0x08,0x00,0x00,0x03,0x3E,0x01,0x3E,0x03,0x00,0x00},/*"W",55*/
{0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20},/*"X",56*/
{0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},/*"Y",57*/
{0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00},/*"Z",58*/
{0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00},/*"[",59*/
{0x00,0x04,0x38,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00},/*"\",60*/
{0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00},/*"]",61*/
{0x00,0x00,0x04,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},/*"_",63*/
{0x00,0x12,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",64*/
{0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x19,0x24,0x24,0x12,0x3F,0x20,0x00},/*"a",65*/
{0x10,0xF0,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00},/*"b",66*/
{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00},/*"c",67*/
{0x00,0x00,0x80,0x80,0x80,0x90,0xF0,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20},/*"d",68*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x24,0x24,0x24,0x24,0x17,0x00},/*"e",69*/
{0x00,0x80,0x80,0xE0,0x90,0x90,0x20,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"f",70*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00},/*"g",71*/
{0x10,0xF0,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},/*"h",72*/
{0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"i",73*/
{0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00},/*"j",74*/
{0x10,0xF0,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x06,0x29,0x30,0x20,0x00},/*"k",75*/
{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"l",76*/
{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F},/*"m",77*/
{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},/*"n",78*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},/*"o",79*/
{0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0x91,0x20,0x20,0x11,0x0E,0x00},/*"p",80*/
{0x00,0x00,0x00,0x80,0x80,0x00,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0x91,0xFF,0x80},/*"q",81*/
{0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00},/*"r",82*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00},/*"s",83*/
{0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x10,0x00},/*"t",84*/
{0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20},/*"u",85*/
{0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x03,0x0C,0x30,0x0C,0x03,0x00,0x00},/*"v",86*/
{0x80,0x80,0x00,0x80,0x80,0x00,0x80,0x80,0x01,0x0E,0x30,0x0C,0x07,0x38,0x06,0x01},/*"w",87*/
{0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x20,0x31,0x0E,0x2E,0x31,0x20,0x00},/*"x",88*/
{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x81,0x86,0x78,0x18,0x06,0x01,0x00},/*"y",89*/
{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00},/*"z",90*/
{0x00,0x00,0x00,0x00,0x00,0xFC,0x02,0x02,0x00,0x00,0x00,0x00,0x01,0x3E,0x40,0x40},/*"{",91*/
{0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00},/*"|",92*/
{0x02,0x02,0xFC,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x3E,0x01,0x00,0x00,0x00,0x00},/*"}",93*/
{0x00,0x02,0x01,0x02,0x02,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"~",94*/
};



I2C_HandleTypeDef * g_OLED_i2c = &hi2c2;
__IO uint8_t i2c_dma_status = 0;

void OLED_command( uint8_t cmd )
{	
	HAL_I2C_Mem_Write( g_OLED_i2c, OLED_ADDRESS, OLED_COMMAND_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &cmd , 1 , 100) ;
}

void OLED_data( uint8_t data , uint8_t inv )
{
	if (inv == 0 )
	{
		HAL_I2C_Mem_Write( g_OLED_i2c, OLED_ADDRESS, OLED_DATA_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data , 1 , 100 ) ;
	}
	else
	{
		data = ~ data ;
		HAL_I2C_Mem_Write( g_OLED_i2c, OLED_ADDRESS, OLED_DATA_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data , 1 , 100 ) ;
	}
}

void OLED_address( uint8_t page , uint8_t col )
{
	uint8_t t ;
	t = 0xb0 + page ;
	OLED_command( t );
	t = (( col >> 4 ) & 0x0f ) + 0x10 ;
	OLED_command( t );
	t = col & 0x0f  ;
	OLED_command( t );
}

void OLED_init( void )
{
    OLED_command(0xae); 
 
    OLED_command(0xd5);	 
    OLED_command(0x80); 
 
    OLED_command(0xa8);	 
    OLED_command(0x1f); 
 
    OLED_command(0xd3); 
    OLED_command(0x00); 
 
    OLED_command(0x40);	
 
    OLED_command(0x8d);	
    OLED_command(0x14);
 
    OLED_command(0x20);	
    OLED_command(0x02);
 
    OLED_command(0xc8); 
    OLED_command(0xa1); 
 
    OLED_command(0xda); 
//    OLED_command(0x12);
    OLED_command(0x02);
 
    OLED_command(0x81);
    OLED_command(0xcf);
 
    OLED_command(0xd9);
    OLED_command(0xf1);
 
    OLED_command(0xdb); 
    OLED_command(0x40);
 
    OLED_command(0xaf); 
}


void OLED_clear( void )
{
	uint8_t i , j ;
	for ( i = 0  ; i < 4 ; i++)
	{
		OLED_address( i , 0 );
		for ( j = 0 ; j < 128 ; j++ )
		{ 
			OLED_data( 0x00 , 0 ) ;
		}
	}
}
/*
row = 0 ~ 1
col = 0 ~ 127
*/
void OLED_ShowAscii( uint8_t row , uint8_t col , uint8_t * data , uint8_t length , uint8_t inv )
{
	uint8_t i, j ,pos ;
	const uint8_t * pchar ;
	if ( row > 1 || col > 31 ) return ;
	for ( i = 0 ; i < length ; i++ )
	{
		pos = data[i] - ' ';
		if ( pos > 94 ) pos = 0 ;
		pchar = ASCII[ pos ];
	  OLED_address( row*2 , (col+i)*8 );
		for ( j = 0 ; j < 8 ; j++ )
		{
		  OLED_data( pchar[j] , inv ) ;
		}
	  OLED_address( row*2+1 , (col+i)*8 );
		for ( j = 0 ; j < 8 ; j++ )
		{
		  OLED_data( pchar[j + 8] ,inv ) ;
		}		
	}
}

uint8_t g_i2c_top_cmd[3];
uint8_t g_i2c_low_cmd[3];
uint8_t g_i2c_top_buf[128];
uint8_t g_i2c_low_buf[128];

void OLED_ShowAscii_2_Buf(uint8_t row , uint8_t col , uint8_t * data , uint8_t length , uint8_t inv)
{
	uint8_t t , pos, index = 0;
	uint16_t i, j;
	const uint8_t * pchar ;
	
	t = 0xb0 + row*2 ;
	g_i2c_top_cmd[index++] = t;
	t = (( col >> 1 ) & 0x0f ) + 0x10 ;
	g_i2c_top_cmd[index++] = t;
	t = ( col << 3 ) & 0x0f  ;
	g_i2c_top_cmd[index++] = t;
	
	index = 0;
	
	if ( row > 1 || col > 31 ) return ;
	
	for( i = 0 ; i < length ; i++ )
	{
		pos = data[i] - ' ';
		if ( pos > 94 ) pos = 0 ;
		pchar = ASCII[ pos ];
		for ( j = 0 ; j < 8 ; j++ )
		{
			g_i2c_top_buf[index++] = (inv == 0) ? pchar[j] : ~pchar[j];
		}
	}
	
	index = 0;
	
	t = 0xb0 + row*2+1 ;
	g_i2c_low_cmd[index++] = t;
	t = (( col >> 1 ) & 0x0f ) + 0x10 ;
	g_i2c_low_cmd[index++] = t;
	t = ( col << 3 ) & 0x0f  ;
	g_i2c_low_cmd[index++] = t;
	
	index = 0;
	
	for( i = 0 ; i < length ; i++ )
	{
		pos = data[i] - ' ';
		if ( pos > 94 ) pos = 0 ;
		pchar = ASCII[ pos ];
		for ( j = 0 ; j < 8 ; j++ )
		{
			g_i2c_low_buf[index++] = (inv == 0) ? pchar[j + 8] : ~pchar[j + 8];
		}
	}
	
}

void OLED_Notice( void )
{
	uint8_t data[2][17] = { {"Remove OLED     "} ,
								          {"Connect DotMatri"} } ;
	OLED_ShowAscii( 0,0,data[0] , 16 ,0  );
	OLED_ShowAscii( 1,0,data[1] , 16 ,0  );
	
}

void OLED_all_on( void )
{
	uint8_t data[3][17] = { {"Init QMI8610.  "} ,
								          {"Init QMI8610.. "} ,
								          {"Init QMI8610..."} 
	                    } ;
  uint8_t i = 0 ;
	
	while ( 1)
	{
	  OLED_ShowAscii( 0,0,data[i] ,strlen(data[i]) ,1  );
		HAL_Delay(500);
		if ( i < 2 )i++;
		else i = 0;
	}
}

void OLED_Debug(uint8_t *data, uint8_t row , uint8_t col)
{
	uint8_t buf[16];
	uint8_t len;
	len = strlen(data);
	if(len < 16)
	{
		memset(buf, ' ', sizeof(buf));
		strncpy(buf, data, len);
		OLED_ShowAscii_2_Buf(row, col, buf, 16, 0);
		i2c_dma_status = TOPCMD_TRANS;
		HAL_I2C_Mem_Write_DMA( g_OLED_i2c, OLED_ADDRESS, OLED_COMMAND_REG, I2C_MEMADD_SIZE_8BIT, g_i2c_top_cmd , 3) ;
	}
	else
	{
		OLED_ShowAscii_2_Buf(row, col, data, 16, 0);
		i2c_dma_status = TOPCMD_TRANS;
		HAL_I2C_Mem_Write_DMA( g_OLED_i2c, OLED_ADDRESS, OLED_COMMAND_REG, I2C_MEMADD_SIZE_8BIT, g_i2c_top_cmd , 3) ;
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	switch(i2c_dma_status)
	{
		case TOPCMD_TRANS:
			i2c_dma_status = TOP_HALF_TRANS;
		  HAL_I2C_Mem_Write_DMA( g_OLED_i2c, OLED_ADDRESS, OLED_DATA_REG, I2C_MEMADD_SIZE_8BIT,  g_i2c_top_buf , 128 ) ;
			break;
		case TOP_HALF_TRANS:
			i2c_dma_status = LOWCMD_TRANS;
			HAL_I2C_Mem_Write_DMA( g_OLED_i2c, OLED_ADDRESS, OLED_COMMAND_REG, I2C_MEMADD_SIZE_8BIT, g_i2c_low_cmd , 3) ;
			break;
		case LOWCMD_TRANS:
			i2c_dma_status = LOW_HALF_TRANS;
		  HAL_I2C_Mem_Write_DMA( g_OLED_i2c, OLED_ADDRESS, OLED_DATA_REG, I2C_MEMADD_SIZE_8BIT,  g_i2c_low_buf , 128 ) ;
		break;
		case LOW_HALF_TRANS:
			i2c_dma_status = 0;
		break;
		default:
			break;
	}
}