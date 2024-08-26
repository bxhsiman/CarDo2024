#ifndef __CAR_DRIVE_H
#define __CAR_DRIVE_H

#include "main.h"

#define USER_CMD_START				"tr_start" 		
#define USER_CMD_BT_ON				"bt_on"
#define USER_CMD_BT_OFF				"bt_off"
#define USER_CMD_MC_F					"mc_f"
#define USER_CMD_MC_B					"mc_b"
#define USER_CMD_MC_L					"mc_l"
#define USER_CMD_MC_R					"mc_r"
#define USER_CMD_MC_S					"mc_s"
#define USER_CMD_SPEED				"tr_speed"
#define USER_CMD_KP						"kp"
#define USER_CMD_KI						"ki"
#define USER_CMD_KD						"kd"
#define USER_CMD_KLM_EN				"kalman_en"
#define USER_CMD_KLM_CFD0			"kalman_c0"
#define USER_CMD_KLM_CFD1  		"kalman_c1"
#define USER_CMD_BT_NAME			"bt_name"
#define USER_CMD_BT_PWD				"bt_pwd"
#define USER_CMD_ADC_TIME			"adc_time"
#define USER_CMD_CTRL_TIME		"ctrl_time"
#define USER_CMD_SPEED_MAX		"speed_max"
#define USER_CMD_SPEED_MIN		"speed_min"
#define USER_CMD_SHOW_CFG 		"show_cfg"
#define USER_CMD_FLASH_INIT		"flash_init"
#define USER_CMD_MUSIC_ON    	"music_on"
#define USER_CMD_MUSIC_OFF   	"music_off"

#pragma pack(4)
typedef struct
{
	float 		KP, KI, KD   ;
	uint16_t 	car_speed_set;
	uint16_t  car_speed_max;
	uint16_t  car_speed_min;	
	uint16_t	adc_compare_gate;	

	uint8_t		adc_interval ;
	uint8_t		car_ctrl_interval ;
	uint8_t 	kalman_enable;
	float			kalman_confidence[2];
	uint8_t 	bt_name[15];
	uint8_t  	bt_pwd[8];
} car_config_t ;

typedef struct
{
	
	uint8_t 	car_mode;
	uint8_t 	track_start;
	int     	left_speed;
	int     	right_speed;
	uint16_t 	adc_full_white;
	uint16_t 	adc_full_black;
	
	uint8_t 	car_ctrl_freq;
	int      	last_error;	
	
	int 			total_err_diff;	
	int 			total_err;		
	
	float 		track_err ;
	float 		track_err_diff ;
	
	uint16_t 	car_speed ;
	
}car_ctrl_t;

typedef struct
{
	uint8_t cmd[32];
	void (*func_callback)(uint8_t *buf, void *param);
	void *param;
	uint8_t type;
}car_ctrl_cmd_t;

#define CAR_IDLE      			0
#define CAR_FIND_START 			1
#define CAR_TRACKING				2
#define CAR_MANU_CTRL				3



void ManualCarCtrl(void);
void CarTrackCtrl(void);
void CarMotoCtrl(int16_t left_speed, int16_t right_speed);
void CarPIDSpeedCtrl(float angle, float diff);

#define CMD_MAP_NUM 10
#define CMD_BUF_LEN	64

#define CONV_INT			0
#define CONV_FLOAT		1
#define CONV_NUM_CHAR	2
#define CONV_U8				3
#define CONV_U16			4


#define BT_ENABLE			"@bt_on&"
#define BT_DISABLE		"@bt_off&"


void CarCtrlInit( car_config_t *p_car_cfg );

uint32_t UartCtrl_RxDataCallback( uint8_t * buf , uint32_t len );

extern car_config_t g_CarConfig;
extern car_ctrl_t   g_CarCtrl ;
#endif
