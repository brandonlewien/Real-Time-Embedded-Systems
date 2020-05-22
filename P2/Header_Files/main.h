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
#include  <stdint.h>
#include  <stdio.h>
#include  <stdbool.h>
#include  <string.h>

#include  "all.h"

#include  "display.h"
#include  "textdisplay.h"
#include  "displayconfigapp.h"
#include  "dmd.h"
#include  "glib.h"
#include  "waypoints.h"

#include  <bsp_os.h>
#include  "bsp.h"

#include  "app.h"
#include  "em_emu.h"
#include  "gpio.h"
#include  "fifo.h"
#include  "shared_data.h"

#include  <stdlib.h>
#include  <../../platform/emlib/inc/em_csen.h>
#include  "em_cmu.h"

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>


#define  EX_MAIN_START_TASK_PRIO       21u
#define  EX_MAIN_START_TASK_STK_SIZE  512u

#define  LED_TASK_PRIO                 22u
#define  LED_TASK_STK_SIZE            512u

#define  SPEED_SETPOINT_PRIO           23u
#define  SPEED_SETPOINT_STK_SIZE      512u

#define  VEHICLE_DIRECTION_PRIO        24u
#define  VEHICLE_DIRECTION_STK_SIZE   512u

#define  VEHICLE_MONITOR_PRIO          25u
#define  VEHICLE_MONITOR_STK_SIZE     512u

#define  DISPLAY_PRIO                  26u
#define  DISPLAY_STK_SIZE             512u

#define  IDLE_TASK_PRIO                62u // Lowest due to 63-1 assert
#define  IDLE_TASK_STK_SIZE           512u

#define  OS_TIME_DELAY_MS             100u

#define  PER_BUTTON_MPH               5u
// Signed comparisons with negative
#define  MAX_SPEED_LIMIT_NOTURN        75
int16_t  MAX_SPEED_LIMIT_NOTURN_NEG = -75;
#define  MAX_SPEED_LIMIT_TURN          45
int16_t  MAX_SPEED_LIMIT_TURN_NEG   = -45;

#define  MAX_TIMEOUT                   50

#define  ON  1
#define  OFF 0

static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  CPU_STK  SpeedSetpointStk[SPEED_SETPOINT_STK_SIZE];
static  CPU_STK  VehicleDirectionStk[VEHICLE_DIRECTION_STK_SIZE];
static  CPU_STK  VehicleMonitorStk[VEHICLE_MONITOR_STK_SIZE];
static  CPU_STK  LED_TaskStk[LED_TASK_STK_SIZE];
static  CPU_STK  Display_TaskStk[LED_TASK_STK_SIZE];
static  CPU_STK  Idle_TaskStk[IDLE_TASK_STK_SIZE];

static  OS_TCB   IdleTaskTCB;
static  OS_TCB   SpeedSetpointTCB;
static  OS_TCB   LEDTaskTCB;
static  OS_TCB   VehicleDirectionTCB;
static  OS_TCB   VehicleMonitorTCB;
static  OS_TCB   DisplayTCB;
static  OS_TCB   Ex_MainStartTaskTCB;


static  void  Ex_MainStartTask (void * p_arg);
static  void  IdleTask (void * p_arg);

OS_FLAG_GRP VehicleCtrl;
OS_FLAGS    SpeedFlag     = 1;
OS_FLAGS    DirectionFlag = 2;
OS_FLAGS    LEDFlag       = 3;

OS_SEM      sliderSemaphore;
OS_SEM      buttonSemaphore;

OS_MUTEX    SpeedMutex;
OS_MUTEX    DirectionMutex;

volatile bool pushButton1State   = OFF;
volatile bool pushButton0State   = OFF;
volatile int32_t sliderPosition = -1;
InputFifo_t * FifoBuffer;
volatile SpeedStates SpeedSetpointOption = 0;
volatile bool led1 = OFF;
volatile bool led0 = OFF;

typedef struct {
	int16_t curSpeed;
	uint16_t numInc;
	uint16_t numDec;
}SpeedData;

typedef struct {
	int16_t curDirection;
	uint16_t timeHeld;
	uint16_t numLeft;
	uint16_t numRight;
}DirectionData;

DirectionData * DirectionStorage;
SpeedData * SpeedStorage;

////////////
//CAPSENSE//
////////////

#define CAPSENSE_CHANNELS       { csenSingleSelAPORT1XCH0, \
                                  csenSingleSelAPORT1YCH1, \
								  csenSingleSelAPORT1XCH2, \
								  csenSingleSelAPORT1YCH3 }
#define CSEN_CHANNELS           4             /**< Number of channels in use for capsense */
#define CAPSENSE_THRESHOLD		25000u

CSEN_Init_TypeDef csenInit = CSEN_INIT_DEFAULT;
CSEN_InitMode_TypeDef csenInitMode = CSEN_INITMODE_DEFAULT;
CSEN_InputSel_TypeDef channelList[CSEN_CHANNELS] = CAPSENSE_CHANNELS;

bool capsenseIsPressed[CSEN_CHANNELS];


/* Frequency of RTCC (CCV1) pulses on PRS channel 4
   (frequency of LCD polarity inversion). */
#define RTCC_PULSE_FREQUENCY    (64)

#define GLIB_FONT_WIDTH   (glibContext.font.fontWidth + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT  (glibContext.font.fontHeight)

/* Center of display */
#define CENTER_X (glibContext.pDisplayGeometry->xSize / 2)
#define CENTER_Y (glibContext.pDisplayGeometry->ySize / 2)

#define MAX_X (glibContext.pDisplayGeometry->xSize - 1)
#define MAX_Y (glibContext.pDisplayGeometry->ySize - 1)

#define MIN_X           0
#define MIN_Y           0

#define INIT_DEMO_NO    0

#define MAX_STR_LEN     48

/* The GLIB context */
static GLIB_Context_t   glibContext;

static volatile uint32_t demoNo = INIT_DEMO_NO;



