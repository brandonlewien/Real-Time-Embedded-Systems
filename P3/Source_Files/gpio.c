#include "gpio.h"
#include "cmu.h"
#include "capsense.h"
#include "fifo.h"

extern volatile bool pushButton1State;
extern volatile bool pushButton0State;

extern InputFifo_t * FifoBuffer;
extern OS_SEM buttonSemaphore;
extern OS_FLAGS LEDFlag;
extern OS_FLAG_GRP VehicleCtrl;


extern volatile bool led1;
extern volatile bool led0;


#include  <../../platform/emlib/inc/em_csen.h>
#include  "em_cmu.h"


#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#if (LAB2_USE_INTERRUPT)
//static volatile bool isB1FallingEdge    = false;
//static volatile bool isB0FallingEdge    = false;
#endif

/******************************************************************************
 * @brief Button1_Sample - Sample Button 1 on the Pearl Gecko
 * @param none
 * @return none
 *****************************************************************************/
int Button1_Sample(void)
{
	pushButton1State = GPIO_PinInGet(BUT1_port, BUT1_pin);
	return !pushButton1State;
}
/******************************************************************************
 * @brief Button0_Sample - Sample Button 0 on the Pearl Gecko
 * @param none
 * @return none
 *****************************************************************************/
int Button0_Sample(void)
{
	pushButton0State = GPIO_PinInGet(BUT0_port, BUT0_pin);
	return !pushButton0State;
}
/******************************************************************************
 * @brief GPIO_Interrupt_Setup - Configure Interrupts for Buttons
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_Interrupt_Setup(void)
{
	sliderSemaphore.Type = OS_OBJ_TYPE_SEM;
	// Clear, Enable, Config
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	GPIO_IntConfig(BUT0_port, BUT0_pin, true, true, BUT0_default);
	GPIO_IntConfig(BUT1_port, BUT1_pin, true, true, BUT1_default);

}
void GPIO_Open(void){

	// Set LED ports to be standard output drive with default off (cleared)
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_DriveStrengthSet(BUT0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(BUT0_port, BUT0_pin, gpioModeInput, BUT0_default);

//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(BUT1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(BUT1_port, BUT1_pin, gpioModeInput, BUT1_default);
}
/******************************************************************************
 * @brief LED_Critical - Handles editing the critical global variables
 * @param inputLED0 - Handles led0 critical section
 * 		  inputLED1 - Handles led1 critical section
 * @return none
 *****************************************************************************/
void LED_Critical(int16_t inputLED0, int16_t inputLED1)
{
	RTOS_ERR err;
	if(inputLED0 != NOT_HANDLED)
	{
		if (led0 != inputLED0)
		{
			led0 = inputLED0;
    		OSFlagPost(&VehicleCtrl, (OS_FLAGS)LEDFlag, OS_OPT_POST_FLAG_SET, &err);
		}
	}
	if(inputLED1 != NOT_HANDLED)
	{
		if (led1 != inputLED1)
		{
			led1 = inputLED1;
    		OSFlagPost(&VehicleCtrl, (OS_FLAGS)LEDFlag, OS_OPT_POST_FLAG_SET, &err);
		}
	}
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
}
#if (LAB2_USE_INTERRUPT)
/******************************************************************************
 * @brief GPIO_ODD_IRQHandler - Odd GPIO IRQHandler
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	RTOS_ERR err;
	GPIO_IntClear(1 << BUT1_pin);
	OSSemPost(&buttonSemaphore, OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED, &err);

	InputFifo_Put(FifoBuffer, Button1_Sample() + BUTTON1_ID);
}
/******************************************************************************
 * @brief GPIO_EVEN_IRQHandler - Even GPIO IRQHandler
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{

	// Clear Interrupt
	RTOS_ERR err;
	GPIO_IntClear(1 << BUT0_pin);
	OSSemPost(&buttonSemaphore, OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED, &err);

	InputFifo_Put(FifoBuffer, Button0_Sample());
}
#endif
