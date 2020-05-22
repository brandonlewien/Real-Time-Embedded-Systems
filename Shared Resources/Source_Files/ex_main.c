//**************************************************************************//**
// * @file ex_main.c
// * @brief Shared Resources!
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

#include  "main.h"

/******************************************************************************
 * @brief LedOutput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void LedOutput (void *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
    	OSFlagPend(&VehicleCtrl,                  /* Wait on this */
    			    LEDFlag,                      /* Events to check */
		            0,                            /* till all set */
					OS_OPT_PEND_FLAG_SET_ALL,     /* then consume */
					DEF_NULL,
				   &err);                         /* time out */
		OSFlagPost(&VehicleCtrl, (OS_FLAGS)LEDFlag, OS_OPT_POST_FLAG_CLR, &err);
    	if (led1)
    	{
    	    GPIO_PinOutSet(LED1_port, LED1_pin);
    	}
    	else
    	{
    		GPIO_PinOutClear(LED1_port, LED1_pin);
    	}
    	if (led0)
    	{
    	    GPIO_PinOutSet(LED0_port, LED0_pin);
    	}
    	else
    	{
    		GPIO_PinOutClear(LED0_port, LED0_pin);
    	}
    }
}
/******************************************************************************
 * @brief Speed_Data_Init
 * @param void
 * @return none
 *****************************************************************************/
static void Speed_Data_Init(void)
{
	SpeedStorage = (SpeedData *)malloc(sizeof(SpeedData));
	SpeedStorage->curSpeed = 0;
	SpeedStorage->numInc   = 0;
	SpeedStorage->numDec   = 0;
}
/******************************************************************************
 * @brief SpeedSetpoint
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void SpeedSetpoint (void * p_arg)
{
    RTOS_ERR  err;
    uint16_t but0State     = 0;
    uint16_t but1State     = 0;
	InputValue_t FifoValue = 0;
    PP_UNUSED_PARAM(p_arg);

    Speed_Data_Init();
    Fifo_Init();

    while (DEF_ON) {
		OSSemPend(&buttonSemaphore, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
	    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		InputFifo_Get(FifoBuffer, &FifoValue);
		// Set FifoValue from queue
		if (FifoValue >= 2)
		{
			but1State = FifoValue - BUTTON1_ID; // Differentiate between but1 and 0 by adding an ID to but1
		}
		else
		{
			but0State = FifoValue;
		}
		// Handle Value
		if ((!but0State && !but1State) || (but0State && but1State))
		{
			// Do nothing
		}
		else if(!but0State && but1State)
		{
			OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
			SpeedStorage->curSpeed = SpeedStorage->curSpeed - PER_BUTTON_MPH;
			SpeedStorage->numDec++;
			OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);
		}
		else if(but0State && !but1State)
		{
			OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
			SpeedStorage->curSpeed = SpeedStorage->curSpeed + PER_BUTTON_MPH;
			SpeedStorage->numInc++;
			OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);
		}
		else
		{
			APP_RTOS_ASSERT_DBG_FAIL(;);
		}
		OSFlagPost(&VehicleCtrl, (OS_FLAGS)SpeedFlag, OS_OPT_POST_FLAG_SET, &err);
    }
}
/******************************************************************************
 * @brief Direction_Data_Init
 * @param void
 * @return none
 *****************************************************************************/
static void Direction_Data_Init(void)
{
	DirectionStorage = (DirectionData *)malloc(sizeof(DirectionData));
	DirectionStorage->curDirection = -1;
	DirectionStorage->numLeft      = 0;
	DirectionStorage->numRight     = 0;
	DirectionStorage->timeHeld     = 0;
}
/******************************************************************************
 * @brief SliderInput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void VehicleDirection (void * p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_CSEN_HF, true);

    CSEN_Init(CSEN, &csenInit);
    csenInitMode.convSel = csenDMRes16;
    csenInitMode.accMode = csenAccMode4;
    Direction_Data_Init();
    while (DEF_ON) {
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
		if ((capsenseIsPressed[SLIDER_RIGHT] ||
			capsenseIsPressed[SLIDER_MID_RIGHT]) &&
			(capsenseIsPressed[SLIDER_MID_LEFT] ||
			capsenseIsPressed[SLIDER_LEFT]))
		{
			// Both on
			sliderPosition = SLIDER_NONE;
		}
		else if (capsenseIsPressed[SLIDER_RIGHT])
		{
			sliderPosition = SLIDER_RIGHT;
		}
		else if (capsenseIsPressed[SLIDER_MID_RIGHT])
		{
			sliderPosition = SLIDER_MID_RIGHT;
		}
		else if (capsenseIsPressed[SLIDER_MID_LEFT])
		{
			sliderPosition = SLIDER_MID_LEFT;
		}
		else if (capsenseIsPressed[SLIDER_LEFT])
		{
			sliderPosition = SLIDER_LEFT;
		}
		else
		{
			sliderPosition = SLIDER_NONE;
		}
		OSFlagPost(&VehicleCtrl, (OS_FLAGS)DirectionFlag, OS_OPT_POST_FLAG_SET, &err);
        OSTimeDly(OS_TIME_DELAY_MS, OS_OPT_TIME_DLY, &err);


    }
}
/******************************************************************************
 * @brief VehicleMonitor
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
void VehicleMonitor (void * p_arg)
{
    RTOS_ERR  err;

	static bool turnWarning = false;

    while (DEF_ON) {
    	OSFlagPend(&VehicleCtrl,                  /* Wait on this */
    			    DirectionFlag + SpeedFlag,    /* Events to check */
		            0,                            /* till all set */
					OS_OPT_PEND_FLAG_SET_ANY,     /* then consume */
					DEF_NULL,
				   &err);                         /* time out */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
        // Find which flag was called
    	uint16_t whichFlag = (uint16_t)OSFlagPendGetFlagsRdy(&err);
    	if (whichFlag == DirectionFlag)
    	{
    		OSFlagPost(&VehicleCtrl, (OS_FLAGS)DirectionFlag, OS_OPT_POST_FLAG_CLR, &err);
			OSMutexPend(&DirectionMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
    		if (DirectionStorage->curDirection == sliderPosition && sliderPosition != SLIDER_NONE)
    		{
    			DirectionStorage->timeHeld++;
    			if (sliderPosition == SLIDER_LEFT || sliderPosition == SLIDER_MID_LEFT)
    			{
    				DirectionStorage->numLeft++;
    			}
    			else if (sliderPosition == SLIDER_RIGHT || sliderPosition == SLIDER_MID_RIGHT)
    			{
    				DirectionStorage->numRight++;
    			}
    			if (DirectionStorage->timeHeld == MAX_TIMEOUT)
    			{
    				LED_Critical(NOT_HANDLED, true);
        			DirectionStorage->timeHeld = 0;
    			}
        		else
        		{
    				LED_Critical(false, NOT_HANDLED);
        		}
    		}
    		else
    		{
    			DirectionStorage->timeHeld = 0;
    			DirectionStorage->curDirection = sliderPosition;
				LED_Critical(NOT_HANDLED, false);
    		}
			OSMutexPost(&DirectionMutex, OS_OPT_POST_NONE, &err);

			OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
    		if ((SpeedStorage->curSpeed >= MAX_SPEED_LIMIT_TURN ||
    		     SpeedStorage->curSpeed <= MAX_SPEED_LIMIT_TURN_NEG) &&
    			(sliderPosition != SLIDER_NONE))
    		{
				LED_Critical(true, NOT_HANDLED);
				turnWarning = true;
    		}
    		else if (turnWarning == true)
    		{
    			turnWarning = false;
    			if (!(SpeedStorage->curSpeed >= MAX_SPEED_LIMIT_NOTURN ||
      				SpeedStorage->curSpeed <= MAX_SPEED_LIMIT_NOTURN_NEG))
    			{
    				LED_Critical(false, NOT_HANDLED);
    			}
    		}
			OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);
    	}
    	if (whichFlag == SpeedFlag)
    	{
    		OSFlagPost(&VehicleCtrl, (OS_FLAGS)SpeedFlag, OS_OPT_POST_FLAG_CLR, &err);
			OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
    		if (SpeedStorage->curSpeed >= MAX_SPEED_LIMIT_NOTURN ||
    			SpeedStorage->curSpeed <= MAX_SPEED_LIMIT_NOTURN_NEG)
    		{
				LED_Critical(true, NOT_HANDLED);
    			led0 = ON;
    			OSFlagPost(&VehicleCtrl, (OS_FLAGS)LEDFlag, OS_OPT_POST_FLAG_SET, &err);
    		}
    		else if ((SpeedStorage->curSpeed >= MAX_SPEED_LIMIT_TURN ||
    				  SpeedStorage->curSpeed <= MAX_SPEED_LIMIT_TURN_NEG) &&
    				  (sliderPosition != SLIDER_NONE))
    		{
				LED_Critical(true, NOT_HANDLED);
    		}
    		else
    		{
				LED_Critical(false, NOT_HANDLED);
    		}
			OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);
    	}
    }
}
/******************************************************************************
 * @brief DisplayTask
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static DISPLAY_Device_t displayDevice;    /* Display device handle.         */

void DisplayTask (void * p_arg)
{
    RTOS_ERR  err;
    static int16_t curSpeed     = 0;
    static int16_t curDirection = 0;
	/* Initialize the display module. */
	DISPLAY_Init();

	/* Retrieve the properties of the display. */
	if (DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK) {
		while (1) ;
	}

	/* Retarget stdio to the display. */
	if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit()) {
		while (1) ;
	}
	while (DEF_ON)
	{
		// Get Speed
		OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
	    curSpeed = SpeedStorage->curSpeed;
		OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);
		// Get Direction
		OSMutexPend(&DirectionMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
		curDirection = DirectionStorage->curDirection;
		OSMutexPost(&DirectionMutex, OS_OPT_POST_NONE, &err);
		printf("\f\n\n\n\n\n\n\n\n\n");
		if (curDirection == SLIDER_NONE)
		{
			printf("\rStraight");
		}
		else if (curDirection == SLIDER_LEFT)
		{
			printf("\rHard Left");
		}
		else if (curDirection == SLIDER_MID_LEFT)
		{
			printf("\rLeft");
		}
		else if (curDirection == SLIDER_RIGHT)
		{
			printf("\rHard Right");
		}
		else if (curDirection == SLIDER_MID_RIGHT)
		{
			printf("\rRight");
		}
		printf("\nSpeed: %d\n", curSpeed);

        OSTimeDly(OS_TIME_DELAY_MS * 5, OS_OPT_TIME_DLY, &err);

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

    OSTaskCreate(&SpeedSetpointTCB,
                 "Speed Setpoint Task",
				  SpeedSetpoint,
                  DEF_NULL,
				  SPEED_SETPOINT_PRIO,
                 &SpeedSetpointStk[0],
                 (SPEED_SETPOINT_STK_SIZE / 10u),
                  SPEED_SETPOINT_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&VehicleDirectionTCB,
                 "Vehicle Direction",
				  VehicleDirection,
                  DEF_NULL,
                  VEHICLE_DIRECTION_PRIO,
                 &VehicleDirectionStk[0],
                 (VEHICLE_DIRECTION_STK_SIZE / 10u),
				 VEHICLE_DIRECTION_STK_SIZE,
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

    OSTaskCreate(&VehicleMonitorTCB,
                 "Vehicle Monitor Task",
				  VehicleMonitor,
                  DEF_NULL,
				  VEHICLE_MONITOR_PRIO,
                 &VehicleMonitorStk[0],
                 (VEHICLE_MONITOR_STK_SIZE / 10u),
				  VEHICLE_MONITOR_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&DisplayTCB,
                 "Display Task",
				 DisplayTask,
                  DEF_NULL,
				  DISPLAY_PRIO,
                 &Display_TaskStk[0],
                 (DISPLAY_STK_SIZE / 10u),
				 DISPLAY_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

    OSSemCreate(&buttonSemaphore, "Button Semaphore", (OS_SEM_CTR)0, &err);

    OSFlagCreate(&VehicleCtrl, "Speed Flag", SpeedFlag, &err);
    OSFlagCreate(&VehicleCtrl, "Direction Flag", DirectionFlag, &err);
    OSFlagCreate(&VehicleCtrl, "LED Flag", LEDFlag, &err);

    OSMutexCreate(&SpeedMutex, "Speed Mutex", &err);
    OSMutexCreate(&DirectionMutex, "Direction Mutex", &err);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
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

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
    OSStart(&err);                                              /* Start the kernel.                                    */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    return (1);
}
