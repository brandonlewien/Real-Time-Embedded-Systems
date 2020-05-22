/**************************************************************************//**
 * @file gpio.h
 * @brief General purpose input output header for LEDs and whatnot.
 * @author Brandon Lewien
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2020 Brandon Lewien
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Brandon Lewien will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#ifndef SRC_HEADER_FILES_GPIO_H_
#define SRC_HEADER_FILES_GPIO_H_

#include "em_gpio.h"
#include "all.h"

#define LED0_port      gpioPortF
#define LED1_port      gpioPortF
#define BUT0_port      gpioPortF
#define BUT1_port      gpioPortF

#define LED0_default       false    // Default false (0) = off, true (1) = on
#define LED1_default       false    // Default false (0) = off, true (1) = on
#define BUT0_default        true
#define BUT1_default        true

#define LED0_pin         4
#define LED1_pin         5
#define BUT0_pin         6 
#define BUT1_pin         7

#define LED_ON           0
#define LED_OFF          1

// Captouch Slider
#define SLIDER_RIGHT     3
#define SLIDER_MID_RIGHT 2
#define SLIDER_MID_LEFT  1
#define SLIDER_LEFT      0
#define SLIDER_NONE     -1

extern OS_FLAG_GRP ButtonGroup;
extern OS_FLAGS    Button0Flag;
extern OS_FLAGS    Button1Flag;
extern OS_Q        App_QUSB;
extern OS_SEM      sliderSemaphore;

#define BUT0 1
#define BUT1 2

typedef enum {
	BUT_NONE,
	BUT_LED0_ON,
	BUT_LED1_ON,
	BUT_BOTH,
	SENSE_NONE,
	SENSE_LED0_ON,
	SENSE_LED1_ON,
	SENSE_BOTH
} eMessageOptions;

int  Button1_Sample(void);
int  Button0_Sample(void);
int  Slider_Sample (void);
void Drive_LEDs(void);
void GPIO_Open (void);
void GPIO_Interrupt_Setup(void);
void LedOutput   (void * p_arg);
void ButtonInput (void * p_arg);
void SliderInput (void * p_arg);

#endif
