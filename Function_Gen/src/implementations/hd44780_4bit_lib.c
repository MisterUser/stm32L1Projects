/*
	A hd44780 library for use with the TI MSP430
    hd44780_4bit_lib_.c

    Copyright (C) 2014  Joe Pitz (j_pitz@yahoo.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Special Thanks goes to nikosapi at
    http://nikosapi.org/w/index.php/MSP430_HD44780_Controller_Software
    I used the code provided at the above site as a model for my code
*/

/*
 * HD44780_RW 	= GND -> always WRITE
 * HD44780_RS 	= PB10
 * HD44780_EN 	= PB11
 * HD44780_DATA4= PB12
 * HD44780_DATA5= PB13
 * HD44780_DATA6= PB14
 * HD44780_DATA7= PB15
 */




#include "hd44780_4bit_lib.h"

// Internal functions
static void _write_4bit(uint8_t);
static void _write_8bit(uint8_t);

//Internal file-scope variables
int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };



void hd44780_init(uint8_t dispLines, uint8_t fontSize)
{
   uint8_t fsByte;
   
   //Enable Peripheral Clocks
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

   //Configure Pins
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_StructInit(& GPIO_InitStructure );

   //PB10 -> RS 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   //PB11 -> EN
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   //PB12 -> DATA4
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   //PB13 -> DATA5
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
  
   //PB14 -> DATA6
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   //PB15 -> DATA7
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   fsByte = (FUNCTION_SET | DL_4BITS | dispLines | fontSize);

   _write_8bit(fsByte);

   __delay_cycles(1);
   // We can only set the number of lines upon initialization (see
   // datasheet, page 27). The status bit cannot be checked otherwise the
   // controller will ignore this command.
   // _write_8bit(line_mode);

   hd44780_send_command(HD44780_CMD_DISPLAY_ON);
   __delay_cycles(1);
   hd44780_send_command(HD44780_CMD_RETURN_HOME);
   __delay_cycles(1);
   //hd44780_send_command(HD44780_CMD_CLEAR_DISPLAY);
}

/*
 * Sends the function set byte to setup data length, number of
 * lines and font size
 */

void hd44780_send_command(uint8_t c)
{
    resetPin=HD44780_RS; 
    _write_8bit(c);
}

/*void hd44780_write_special_char(uint8_t c)
{
    
    setPin=HD44780_RS;
    _write_8bit(c);
    resetPin=HD44780_RS;
    __delay_cycles(5);
}
*/

void hd44780_write_char(char c)
{
    setPin=HD44780_RS;
    _write_8bit(c);
    resetPin=HD44780_RS;
}

void hd44780_write_string(char *str)
{
    while (*str != '\0')
    {
        hd44780_write_char(*str++);
    }
}

// sets cursor position for characters can be read or written
void hd44780_setCursorPosition(uint8_t row, uint8_t col)
{
  assert_param(IS_LCD_ROW(row));
  
  hd44780_send_command(HD44780_CMD_SETDDRAMADDR | (col + row_offsets[row]));
}


// Writes an 8bit value on the bus using _write_4bit()
static void _write_8bit(uint8_t data)
{
    _write_4bit(data>>4);
    _write_4bit(data & 0x0f);
}

// Does a 4bit write (the 4 MSB are ignored)
// RS setup -> 40ns before EN is high -> handled in function calls
// EN must be high for EN rise (20ns) + EN hold (230ns) -> 250ns
// Data setup time (80ns) + Data hold (10ns) -> under the EN time
// Assuming we're running at 16MHz (~62ns per clock) -> actually 29ns

static void _write_4bit(uint8_t data)
{
     __delay_cycles(1);
     resetPin=HD44780_EN;
     __delay_cycles(1);

    //set data pins
    //write them before setting EN so that we don't have to worry about their setup/fall/hold times.
    setPin = data << HD44780_DATA_OFFSET;

    //start write by holding EN high for >250ns
    setPin=HD44780_EN;
    __delay_cycles(1);//each tick approximately 15us

    resetPin=HD44780_EN;

    // Delay required to satisfy t_cycE parameter (min 1000ns for 1 EN cycle)
    __delay_cycles(1);

    resetPin=HD44780_Data; //clear data lines

     __delay_cycles(1);
}

void __delay_cycles(int cycles)
{
    volatile int i = 0,j;
    for(i;i<cycles;i++)
    {
        j++;
    }
}
