#ifndef __BEEP_H
#define	__BEEP_H

#define POLICE_CAR_LOW_TIME  500
#define POLICE_CAR_HIGH_TIME 500

#define AMBULANCE_CAR_LOW_TIME  1000
#define AMBULANCE_CAR_HIGH_TIME 1000

void Beep_Start( void );
extern uint8_t g_beep_on  ;

#endif /* __BEEP_H */

