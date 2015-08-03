/*
    hd44780lib_4bit_config.h

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


#ifndef HD44780LIB_4BIT_CONFIG_H
#define HD44780LIB_4BIT_CONFIG_H

#include <stm32l1xx_gpio.h>

#define resetPin	    GPIOB->BSRRH
#define setPin              GPIOB->BSRRL

#define HD44780_RS          0x0400 //pin 10
#define HD44780_EN          0x0800 //pin 11
#define HD44780_Data	    0xF000 //pins 12-15
/*
#define HD44780_DATA4       PB12
#define HD44780_DATA5       PB13
#define HD44780_DATA6       PB14
#define HD44780_DATA7       PB15
*/

#define HD44780_DATA_OFFSET 12

#endif // HD44780LIB_4BIT_CONFIG_H
