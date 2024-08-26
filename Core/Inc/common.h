#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
	
typedef enum
{
  TARGET_UART         = 0x00U,    /*send to UART    */
  TARGET_USB          = 0x01U,    /*send to USB     */
  TARGET_SPI          = 0x02U,    /*send to SPI     */
  TARGET_I2C          = 0x03U,    /*send to I2C     */
	TARGET_CPU					= 0x04U,    /*send to CPU     */
} Switch_Target_TypeDef;

typedef struct {    
    __O  uint8_t  SN[12];
    __O  uint8_t  PAD0;
    __O  uint8_t  PAD1;
} UNI_DEV_SN_Type;

typedef struct {
	GPIO_TypeDef * gpio ;
	uint16_t				 pin ;
} LED_t ;	

// F0,F3  0x1FFFF7AC
// F1     0x1FFFF7E8
// F2,F4  0x1FFF7A10
// F7     0x1FF0F420
// L0     0x1FF80050

#define UNIQUE_DEV_SN_BASE       0x1ffff7e8UL
#define UNI_DEV_SN  ((UNI_DEV_SN_Type *)UNIQUE_DEV_SN_BASE)


//#define SYSTEM_IDLE 			0
#define SYSTEM_TRACK			0
#define SYSTEM_DOTMATRIX	1
//#define SYSTEM_CAL_WHITE	3
#define SYSTEM_CAL_BLACK	2
	
void System_Init( void );
void UserTasks( void);
void LEDCtrl( uint8_t pos  , uint8_t mode );

extern uint8_t g_device_uid[25];
extern uint8_t g_SystemMode;
#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H */
