#include "deadBeats_main.h"


char userCurrentHit;
char rudimentPattern[maxArraySize]= {'L','R','L','L','R','L','R','R'};
uint16_t rudimentTiming[maxArraySize]= {0,500,500,500,500,500,500,500};
char userPattern[maxArraySize];
uint16_t userTimeArray[maxArraySize];
char userArray[10];
int userPatternArraySize;

void deadbeats_main()
{
    deadbeats_timer_init();

    deadbeats_usart_init();

    task_MainLoop();
}


void task_MainLoop()
{
    char userVal;
    //userPatternArraySize = 8;

    int  userArrayIndex = 0;


    //u32_t dyn_arraySize = 1;
    //ttc_heap_array_dynamic(char,dyn_userArray);
    //write_32bit(ttc_heap_get_free_size());
    //putchar('\n');
    //if(ttc_heap_array_is_null(dyn_userArray))
    //{
    //    ttc_heap_array_allocate(dyn_userArray,dyn_arraySize);
    //}
    //write_32bit(ttc_heap_get_free_size());
    //putchar('\n');
    while(1)
    {
        //Check if new data from USART
        if(!db_USART_RX_QueueEmpty())
        {

            userVal = getchar();
            //A(dyn_userArray,userArrayIndex++) = userVal;
            userArray[userArrayIndex++]= userVal;
            /*
            switch(userVal){
            case 'U':
                putchar('U');
                putchar('n');
                putchar('i');
                putchar('v');
                putchar('e');
                putchar('r');
                putchar('s');
                putchar('a');
                putchar('l');
                putchar('\n');
                break;
            case 'S':
                putchar('S');
                putchar('y');
                putchar('n');
                putchar('c');
                putchar('h');
                putchar('r');
                putchar('o');
                putchar('n');
                putchar('o');
                putchar('u');
                putchar('s');
                putchar('\n');
                break;
            case 'A':
                putchar('A');
                putchar('s');
                putchar('y');
                putchar('n');
                putchar('c');
                putchar('h');
                putchar('r');
                putchar('o');
                putchar('n');
                putchar('o');
                putchar('u');
                putchar('s');
                putchar('\n');
                break;
            case 'R':
                putchar('R');
                putchar('e');
                putchar('c');
                putchar('e');
                putchar('i');
                putchar('v');
                putchar('e');
                putchar('r');
                putchar('\n');
                break;

            case 'T':
                putchar('T');
                putchar('r');
                putchar('a');
                putchar('n');
                putchar('s');
                putchar('m');
                putchar('i');
                putchar('t');
                putchar('t');
                putchar('e');
                putchar('r');
                putchar('\n');
                break;

            default:

                putchar('F');
                putchar('o');
                putchar('o');
                putchar('b');
                putchar('a');
                putchar('r');
                putchar('\n');

                break;
            }//end switch
            */
            if(userVal == '#')
            {

                if(strncmp(userArray, "start", 5)==0)
                {
                    start_Timer();
                    putchar('A');
                    putchar('C');
                    putchar('K');
                    putchar(':');
                    putchar('O');
                    putchar('N');
                    putchar('#');
                    putchar('\r');

                }
                else if (strncmp(userArray, "stop", 4)==0)
                {
                    stop_Timer();
                    putchar('A');
                    putchar('C');
                    putchar('K');
                    putchar(':');
                    putchar('O');
                    putchar('F');
                    putchar('F');
                    putchar('#');
                    putchar('\r');
                }
                userArrayIndex = 0;


            }//end if userVal
            //putchar(A(dyn_userArray,userArrayIndex - 1));



        }//end if QueueEmpty

	/** @todo reinstate this for EXTI
        //Check if new data from GPIOs
        if(db_user_button_pressed)
        {
            update_user_input_Array();
            update_user_time_Array();
            db_user_button_pressed = 0;


            send_input_Array();
            send_user_time_Array();

            if(compare_patterns())
            {
                if(compare_time_arrays())
                {
                    putchar('S');
                    putchar('U');
                    putchar('C');
                    putchar('C');
                    putchar('E');
                    putchar('S');
                    putchar('S');
                    putchar('!');
                    putchar('\n');
                }
            }

        }*/

    }
}
/*
void update_user_time_Array()
{
    shift_user_time_Array();
    userTimeArray[userPatternArraySize-1] = period_in_ms;
}

void shift_user_time_Array()
{
    for(int i = 0; i< (userPatternArraySize-1); i++)
    {
        userTimeArray[i] = userTimeArray[i+1];
    }
}

void send_user_time_Array()
{
    putchar('{');
    for(int i = 0; i< (userPatternArraySize-1); i++)
    {
        write_time(userTimeArray[i]);
        putchar('|');
    }
    write_time(userTimeArray[userPatternArraySize-1]);
    putchar('}');
    putchar('\n');
}

void write_32bit(u32_t value)
{
    putchar((char)(((int)'0')+((value%10000000) /1000000)));
    putchar((char)(((int)'0')+((value%1000000) /100000)));
    putchar((char)(((int)'0')+((value%100000) /10000)));
    putchar((char)(((int)'0')+((value%10000)  /1000)));
    putchar('.');
    putchar((char)(((int)'0')+((value%1000)   /100)));
    putchar((char)(((int)'0')+((value%100)    /10)));
    putchar((char)(((int)'0')+ (value%10)));

}

void write_time(uint16_t single_user_time)
{
    
  //  tens_sec        = (char)(((int)'0')+((single_user_time%100000) /10000));
  //  one_sec         = (char)(((int)'0')+((single_user_time%10000)  /1000));
  //  hundo_ms        = (char)(((int)'0')+((single_user_time%1000)   /100));
  //  tens_ms         = (char)(((int)'0')+((single_user_time%100)    /10));
  //  one_ms          = (char)(((int)'0')+ (single_user_time%10));
   

    putchar((char)(((int)'0')+((single_user_time%100000) /10000)));
    putchar((char)(((int)'0')+((single_user_time%10000)  /1000)));
    putchar('.');
    putchar((char)(((int)'0')+((single_user_time%1000)   /100)));
    putchar((char)(((int)'0')+((single_user_time%100)    /10)));
    putchar((char)(((int)'0')+ (single_user_time%10)));
}

void update_user_input_Array()
{
    shift_user_input_Array();
    userPattern[userPatternArraySize-1] = userCurrentHit;
}

void shift_user_input_Array()
{
    for(int i = 0; i< (userPatternArraySize-1); i++)
    {
        userPattern[i] = userPattern[i+1];
    }
}

void send_input_Array()
{
    for(int i = 0; i< (userPatternArraySize); i++)
    {
        putchar(userPattern[i]);
    }
    putchar('\n');
}

bool compare_patterns()
{
    for(int i = 1; i < userPatternArraySize; i++)
    {

        if(rudimentPattern[i] != userPattern[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

bool compare_time_arrays()
{
    for(int i = 1; i < userPatternArraySize; i++)
    {
        uint16_t timeDiff;
        if( userTimeArray[i] > rudimentTiming[i])
        {
             timeDiff = userTimeArray[i] - rudimentTiming[i];
        }
        else
        {
            timeDiff =rudimentTiming[i] - userTimeArray[i];
        }

        if(timeDiff > 250)
        {
            return FALSE;
        }
    }

    return TRUE;
}
*/
