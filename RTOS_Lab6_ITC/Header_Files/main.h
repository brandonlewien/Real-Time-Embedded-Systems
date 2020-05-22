/**************************************************************************//**
 * @file main.h
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
#include <stdint.h>
#include <stdbool.h>
#include "all.h"


#define  EX_MAIN_START_TASK_PRIO       21u
#define  EX_MAIN_START_TASK_STK_SIZE  512u

#define  LED_TASK_PRIO                 21u
#define  LED_TASK_STK_SIZE            512u

#define  BUTTON_TASK_PRIO              22u
#define  BUTTON_TASK_STK_SIZE         512u

#define  SLIDER_TASK_PRIO              23u
#define  SLIDER_TASK_STK_SIZE         512u

#define  IDLE_TASK_PRIO                62u // Lowest due to 63-1 assert
#define  IDLE_TASK_STK_SIZE           512u

#define  OS_TIME_DELAY_MS             100u

#define  ON  1
#define  OFF 0

#if !(LAB2_USE_INTERRUPT)
static volatile uint32_t msTicks; /* counts 1ms timeTicks */
#endif


