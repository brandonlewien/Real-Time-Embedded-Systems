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


#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
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


static  void  Ex_MainStartTask (void  *p_arg);
static  void  LedOutput (void  *p_arg);
static  void  ButtonInput (void  *p_arg);
static  void  SliderInput (void  *p_arg);
static  void  IdleTask (void  *p_arg);


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
    CAPSENSE_Init();

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
}
/******************************************************************************
 * @brief LedOutput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void LedOutput (void  *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
    	Drive_LEDs();

        OSTimeDly(OS_TIME_DELAY_MS, OS_OPT_TIME_DLY, &err);
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
}
/******************************************************************************
 * @brief ButtonInput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void ButtonInput (void  *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
    	Button1_Sample();
    	Button0_Sample();

        OSTimeDly(OS_TIME_DELAY_MS, OS_OPT_TIME_DLY, &err);
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
}
/******************************************************************************
 * @brief SliderInput
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void SliderInput (void  *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
		CAPSENSE_Sense();
		Slider_Sample();

        OSTimeDly(OS_TIME_DELAY_MS, OS_OPT_TIME_DLY, &err);
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
}
/******************************************************************************
 * @brief IdleTask
 * @param p_arg - Argument passed from task creation. Unused, in this case.
 * @return none
 *****************************************************************************/
static void IdleTask (void  *p_arg)
{
    PP_UNUSED_PARAM(p_arg);

    while (DEF_ON) {
    	EMU_EnterEM1();
    }
}
