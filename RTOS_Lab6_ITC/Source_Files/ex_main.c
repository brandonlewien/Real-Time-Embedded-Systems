///**************************************************************************//**
// * @file ex_main.c
// * @brief Tasks!
// * @author Brandon Lewien
// * @version 1.00
// ******************************************************************************
// * @section License
// * <b>(C) Copyright 2020 Brandon Lewien
// ******************************************************************************
// *
// * Permission is granted to anyone to use this software for any purpose,
// * including commercial applications, and to alter it and redistribute it
// * freely, subject to the following restrictions:
// *
// * 1. The origin of this software must not be misrepresented; you must not
// *    claim that you wrote the original software.
// * 2. Altered source versions must be plainly marked as such, and must not be
// *    misrepresented as being the original software.
// * 3. This notice may not be removed or altered from any source distribution.
// *
// * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
// * obligation to support this Software. Silicon Labs is providing the
// * Software "AS IS", with no express or implied warranties of any kind,
// * including, but not limited to, any implied warranties of merchantability
// * or fitness for any particular purpose or warranties against infringement
// * of any proprietary rights of a third party.
// *
// * Brandon Lewien will not be liable for any consequential, incidental, or
// * special damages, or any other relief, or for any claim by any third party,
// * arising from your use of this Software.
// *
// ******************************************************************************/

#include  <bsp_os.h>
#include  "bsp.h"

#include  "main.h"
#include  "app.h"
#include  "em_emu.h"
#include  "gpio.h"
#include <../../platform/emlib/inc/em_csen.h>
#include "em_cmu.h"


#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  CPU_STK  Button_TaskStk[BUTTON_TASK_STK_SIZE];
static  CPU_STK  Slider_TaskStk[SLIDER_TASK_STK_SIZE];
static  CPU_STK  LED_TaskStk[LED_TASK_STK_SIZE];
static  CPU_STK  Idle_TaskStk[IDLE_TASK_STK_SIZE];

static  OS_TCB   IdleTaskTCB;
static  OS_TCB   ButtonTaskTCB;
static  OS_TCB   LEDTaskTCB;
static  OS_TCB   SliderTaskTCB;
static  OS_TCB   Ex_MainStartTaskTCB;

static  void  Ex_MainStartTask (void * p_arg);
static  void  IdleTask (void * p_arg);

OS_FLAG_GRP ButtonGroup;
OS_FLAGS    Button0Flag = 1;
OS_FLAGS    Button1Flag = 2;
OS_Q        App_QUSB;
OS_SEM      sliderSemaphore;
volatile bool pushButton1State   = OFF;
volatile bool pushButton0State   = OFF;
volatile int32_t sliderPosition = -1;

/* Local Defines */
#define CAPSENSE_CHANNELS       { csenSingleSelAPORT1XCH0, csenSingleSelAPORT1YCH1, csenSingleSelAPORT1XCH2, csenSingleSelAPORT1YCH3 }
#define CSEN_CHANNELS           4             /**< Number of channels in use for capsense */
#define CAPSENSE_THRESHOLD		25000u

/* Global Variables */
CSEN_Init_TypeDef csenInit = CSEN_INIT_DEFAULT;
CSEN_InitMode_TypeDef csenInitMode = CSEN_INITMODE_DEFAULT;
CSEN_InputSel_TypeDef channelList[CSEN_CHANNELS] = CAPSENSE_CHANNELS;

bool capsenseIsPressed[CSEN_CHANNELS];


/******************************************************************************
 * @brief SliderCallback
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void SliderCallback (void *p_arg)
{
	RTOS_ERR err;

	OSSemPost(&sliderSemaphore, OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED, &err);
}
/******************************************************************************
 * @brief LedOutput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void LedOutput (void *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);
    uint32_t message;
    OS_MSG_SIZE q_msg_size = sizeof(eMessageOptions);
    while (DEF_ON) {
		message = (uint32_t)OSQPend(&App_QUSB, 0, OS_OPT_PEND_BLOCKING, &q_msg_size,  (CPU_TS *)0, &err);
		if (message == BUT_NONE)
		{
			pushButton0State = OFF;
			pushButton1State = OFF;
		}
		else if (message == BUT_LED0_ON)
		{
			pushButton0State = ON;
			pushButton1State = OFF;
		}
		else if (message == BUT_LED1_ON)
		{
			pushButton0State = OFF;
			pushButton1State = ON;
		}
		else if (message == BUT_BOTH)
		{
			pushButton0State = OFF;
			pushButton1State = OFF;
		}
		if (message == SENSE_NONE)
		{
			sliderPosition = SLIDER_NONE;
		}
		else if (message == SENSE_LED0_ON)
		{
			sliderPosition = SLIDER_RIGHT;
		}
		else if (message == SENSE_LED1_ON)
		{
			sliderPosition = SLIDER_LEFT;
		}
		else if (message == SENSE_BOTH)
		{
			sliderPosition = SLIDER_NONE;
		}

    	Drive_LEDs();
    }
}
/******************************************************************************
 * @brief ButtonInput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void ButtonInput (void * p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);
//    eMessageOptions send_msg;
    while (DEF_ON) {

//    	char * p_buf = "";
    	OSFlagPend(&ButtonGroup,                  /* Wait on this */
    			    Button1Flag + Button0Flag,    /* Events to check */
		            0,                            /* till all set */
					OS_OPT_PEND_FLAG_SET_ANY,     /* then consume */
					DEF_NULL,
					&err);                        /* time out */
    	int whichFlag = (int)OSFlagPendGetFlagsRdy(&err);
		OSFlagPost(&ButtonGroup, (OS_FLAGS)whichFlag, OS_OPT_POST_FLAG_CLR, &err);
		int but0State = Button0_Sample();
		int but1State = Button1_Sample();
		eMessageOptions message;
		if (!but0State && !but1State)
		{
			message = BUT_NONE;
		}
		else if (but0State && but1State)
		{
			message = BUT_BOTH;
		}
		else if(!but0State && but1State)
		{
			message = BUT_LED1_ON;
		}
		else if(but0State && !but1State)
		{
			message = BUT_LED0_ON;
		}
		else
		{
			//assert
		}
        OSQPost((OS_Q *)&App_QUSB, (void *)&message, (OS_MSG_SIZE)10, (OS_OPT)OS_OPT_POST_FIFO, (RTOS_ERR *)&err);

    }
}
/******************************************************************************
 * @brief SliderInput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void SliderInput (void * p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_CSEN_HF, true);

    CSEN_Init(CSEN, &csenInit);
    csenInitMode.convSel = csenDMRes16;
    csenInitMode.accMode = csenAccMode4;

    while (DEF_ON) {
		OSSemPend(&sliderSemaphore, 1000, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
    	uint32_t csResult = 0;
		uint32_t i = 0;

		/* Reset CAPSENSE state variables before updating */
		for (i = CSEN_CHANNELS; i > 0; i--)
		{
		    capsenseIsPressed[i-1] = 0;
		}

		/* Loop over CAPTOUCH channels and take CSEN measurements */
		for (i = CSEN_CHANNELS; i > 0; i--)
		{
			/* Disable CSEN before updating channel to measure */
			CSEN_Disable(CSEN);
			csenInitMode.singleSel = channelList[i-1];
			CSEN_InitMode(CSEN, &csenInitMode);
			/* Re-enable CSEN */
			CSEN_Enable(CSEN);

			/* Start a CAPSENSE conversion */
			CSEN_Start(CSEN);
			/* Wait for measurement to finish */
			while (CSEN_IsBusy(CSEN));

			/* Store result in local variable */
			csResult = CSEN_DataGet(CSEN);

			/* Update global variable */
			if (csResult > CAPSENSE_THRESHOLD) {
			    capsenseIsPressed[i-1] = true;
			}
			else {
			    capsenseIsPressed[i-1] = false;
			}
		}
		int sliderPosition = -1;
		if (((capsenseIsPressed[SLIDER_RIGHT] == true)
				|| (capsenseIsPressed[SLIDER_MID_RIGHT] == true))
				&& !((capsenseIsPressed[SLIDER_LEFT] == true)
				|| (capsenseIsPressed[SLIDER_MID_LEFT] == true)))
		{
			sliderPosition = SENSE_LED0_ON;
		}
		else if (!((capsenseIsPressed[SLIDER_RIGHT] == true)
				|| (capsenseIsPressed[SLIDER_MID_RIGHT] == true))
				&& ((capsenseIsPressed[SLIDER_LEFT] == true)
				|| (capsenseIsPressed[SLIDER_MID_LEFT] == true)))
		{
			sliderPosition = SENSE_LED1_ON;
		}
		else
		{
			sliderPosition = SENSE_BOTH;
		}


//		CAPSENSE_Sense();
        OSQPost((OS_Q *)&App_QUSB, (void *)(sliderPosition), (OS_MSG_SIZE)1, (OS_OPT)OS_OPT_POST_FIFO, (RTOS_ERR *)&err);
    }
}
/******************************************************************************
 * @brief Ex_MainStartTask - This is the task that will be called by the Startup
 * when all services are initializes successfully.
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static  void  Ex_MainStartTask (void  *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */
    BSP_TickInit();                                             /* Initialize Kernel tick source.                       */

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
    OSStatTaskCPUUsageInit(&err);                               /* Initialize CPU Usage.                                */
                                                                /* Check error code.                                    */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
#endif
    OSStatTaskCPUUsageInit(&err);

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* Initialize interrupts disabled measurement.          */
#endif

    Common_Init(&err);                                          /* Call common module initialization example.           */
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */
                                                                /* ... will register all the hardware controller to ... */
                                                                /* ... the platform manager at this moment.             */
    OSStatReset(&err);
    GPIO_Open();
    CPU_TS_TmrInit();
//    CAPSENSE_Init();
    GPIO_Interrupt_Setup();

    OSTaskCreate(&LEDTaskTCB,
                 "LED Task",
				  LedOutput,
                  DEF_NULL,
                  LED_TASK_PRIO,
                 &LED_TaskStk[0],
                 (LED_TASK_STK_SIZE / 10u),
                  LED_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&ButtonTaskTCB,
                 "Button Task",
                  ButtonInput,
                  DEF_NULL,
                  BUTTON_TASK_PRIO,
                 &Button_TaskStk[0],
                 (BUTTON_TASK_STK_SIZE / 10u),
                  BUTTON_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&SliderTaskTCB,
                 "Slider Task",
				  SliderInput,
                  DEF_NULL,
                  SLIDER_TASK_PRIO,
                 &Slider_TaskStk[0],
                 (SLIDER_TASK_STK_SIZE / 10u),
                  SLIDER_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&IdleTaskTCB,
                 "Idle Task",
				  IdleTask,
                  DEF_NULL,
                  IDLE_TASK_PRIO,
                 &Idle_TaskStk[0],
                 (IDLE_TASK_STK_SIZE / 10u),
				  IDLE_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTmrCreate (&MyTmr1,
			      "Slider Callback",
			      0,
			      1,
				  OS_OPT_TMR_PERIODIC,
			     (OS_TMR_CALLBACK_PTR)SliderCallback,
			      0,
			     &err);
    OSSemCreate(&sliderSemaphore, "SliderSem", (OS_SEM_CTR)0, &err);
    OSQCreate((OS_Q *)&App_QUSB, (CPU_CHAR *)"Message Queue", (OS_MSG_QTY)20, (RTOS_ERR *)&err);
    OSFlagCreate(&ButtonGroup, "Button Flags", Button1Flag, &err);
    OSFlagCreate(&ButtonGroup, "Button Flags", Button0Flag, &err);
    OSTmrStart (&MyTmr1, &err);
}

/******************************************************************************
 * @brief IdleTask
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void IdleTask (void * p_arg)
{
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
    	EMU_EnterEM1();
    }
}

/******************************************************************************
 * @brief main
 * @param none
 * @return none
 *****************************************************************************/
int  main (void)
{
    RTOS_ERR  err;

    BSP_SystemInit();                                           /* Initialize System.                                   */
    CPU_Init();                                                 /* Initialize CPU.                                      */

    OS_TRACE_INIT();
    OSInit(&err);                                               /* Initialize the Kernel.                               */
                                                                /* Check error code.                                    */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&Ex_MainStartTaskTCB,                          /* Create the Start Task.                               */
                 "Ex Main Start Task",
                  Ex_MainStartTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &Ex_MainStartTaskStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /* Check error code.                                    */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSStart(&err);                                              /* Start the kernel.                                    */
                                                                /* Check error code.                                    */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    return (1);
}
