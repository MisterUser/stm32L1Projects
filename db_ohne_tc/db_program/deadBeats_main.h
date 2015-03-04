#ifndef DEADBEATS_MAIN_H
#define DEADBEATS_MAIN_H

//#include "ttc-lib/ttc_extensions.h"
#include "deadBeats_common.h"
#include "deadBeats_timer.h"
#include "deadBeats_usart.h"
//#include "deadBeats_vector.h"

extern uint16_t period_in_ms;


//--------File Scope Variables-------//
//char rudimentPattern[userPatternArraySize];// = {'L','R','L','L','R','L','R','R'};
//uint16_t rudimentTiming[userPatternArraySize]; // = {0,500,500,500,500,500,500,500};


/*uint16_t tens_sec;
uint16_t one_sec;
uint16_t hundo_ms;
uint16_t tens_ms;
uint16_t one_ms;
uint16_t tenth_ms;
*/


//--------Functions-----------//
void deadbeats_main();
void task_MainLoop(void *TaskArgument);
void update_user_time_Array();
void shift_user_time_Array();
void write_32bit(u32_t value); //for debug
void write_time(uint16_t single_user_time);
void update_user_input_Array();
void shift_user_input_Array();
void send_input_Array();
bool compare_patterns();
bool compare_time_arrays();

#endif
