#ifndef DEADBEATS_TIMER_H
#define DEADBEATS_TIMER_H

#include "deadBeats_common.h"
#include "deadBeats_usart.h"

//extern char userCurrentHit;

void deadbeats_timer_init();

void start_Timer();
void stop_Timer();

void R_hit_ISR(void);
void L_hit_ISR(void);



#endif
