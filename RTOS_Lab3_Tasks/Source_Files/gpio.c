#include "gpio.h"
#include "cmu.h"
#include "capsense.h"

static volatile bool pushButton1State   = 0;
static volatile bool pushButton0State   = 0;
static volatile int32_t sliderPosition = -1;
#if (LAB2_USE_INTERRUPT)
static volatile bool isB1FallingEdge    = false;
static volatile bool isB0FallingEdge    = false;
#endif

/******************************************************************************
 * @brief Button1_Sample - Sample Button 1 on the Pearl Gecko
 * @param none
 * @return none
 *****************************************************************************/
void Button1_Sample(void)
{
	pushButton1State = GPIO_PinInGet(BUT1_port, BUT1_pin);
}
/******************************************************************************
 * @brief Button0_Sample - Sample Button 0 on the Pearl Gecko
 * @param none
 * @return none
 *****************************************************************************/
void Button0_Sample(void)
{
	pushButton0State = GPIO_PinInGet(BUT0_port, BUT0_pin);
}
/******************************************************************************
 * @brief Slider_Sample - Sample the slider by left and right areas and update
 *                        global variable
 * @param none
 * @return none
 *****************************************************************************/
void Slider_Sample(void)
{
	if (((CAPSENSE_getPressed(SLIDER_RIGHT) == true)
			|| (CAPSENSE_getPressed(SLIDER_MID_RIGHT) == true))
			&& !((CAPSENSE_getPressed(SLIDER_LEFT) == true)
			|| (CAPSENSE_getPressed(SLIDER_MID_LEFT) == true)))
	{
		sliderPosition = SLIDER_RIGHT;
	}
	else if (!((CAPSENSE_getPressed(SLIDER_RIGHT) == true)
			|| (CAPSENSE_getPressed(SLIDER_MID_RIGHT) == true))
			&& ((CAPSENSE_getPressed(SLIDER_LEFT) == true)
			|| (CAPSENSE_getPressed(SLIDER_MID_LEFT) == true)))
	{
		sliderPosition = SLIDER_LEFT;
	}
	else
	{
		sliderPosition = SLIDER_NONE;
	}
}
/******************************************************************************
 * @brief Drive_LEDs - Configure the LED transitions through logic.
 * @param none
 * @return none
 *****************************************************************************/
void Drive_LEDs(void)
{
	// If both are on don't do anything
	if ((pushButton1State == true) && (pushButton0State == true))
	{
		GPIO_PinOutClear(LED0_port, LED0_pin);
		GPIO_PinOutClear(LED1_port, LED1_pin);
		// Case where if you have BUT1 && BUT0 Pressed and are using the capsense
		if (sliderPosition == SLIDER_LEFT)
		{
		    GPIO_PinOutSet(LED0_port, LED0_pin);
		}
		if (sliderPosition == SLIDER_RIGHT)
		{
		    GPIO_PinOutSet(LED1_port, LED1_pin);
		}
	    return;
	}
	// Nominal cases
	if (sliderPosition == SLIDER_LEFT)
	{
	    GPIO_PinOutSet(LED0_port, LED0_pin);
	}
	if (sliderPosition == SLIDER_RIGHT)
	{
	    GPIO_PinOutSet(LED1_port, LED1_pin);
	}
	if ((pushButton0State == true))
	{
	    GPIO_PinOutSet(LED0_port, LED0_pin);
	}
	if ((pushButton1State == true))
	{
	    GPIO_PinOutSet(LED1_port, LED1_pin);
	}
	// Cornercases
	if ((sliderPosition == SLIDER_NONE) && (pushButton0State == false))
	{
		GPIO_PinOutClear(LED0_port, LED0_pin);
	}
	if ((sliderPosition == SLIDER_NONE) && (pushButton1State == false))
	{
		GPIO_PinOutClear(LED1_port, LED1_pin);
	}
	if ((sliderPosition == SLIDER_LEFT) && (pushButton1State == false))
	{
		GPIO_PinOutClear(LED1_port, LED1_pin);
	}
	if ((sliderPosition == SLIDER_RIGHT) && (pushButton0State == false))
	{
		GPIO_PinOutClear(LED0_port, LED0_pin);
	}
}
/******************************************************************************
 * @brief GPIO_Interrupt_Setup - Configure Interrupts for Buttons
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_Interrupt_Setup(void)
{
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

#if (LAB2_USE_INTERRUPT)
/******************************************************************************
 * @brief GPIO_ODD_IRQHandler - Odd GPIO IRQHandler
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	// Clear Interrupt
	GPIO_IntClear(1 << BUT1_pin);
	isB1FallingEdge = !isB1FallingEdge;
	Button1_Sample();
	// If falling edge turn off LED
	pushButton1State = ((isB1FallingEdge == true) ? LED_OFF : LED_ON);

}
/******************************************************************************
 * @brief GPIO_EVEN_IRQHandler - Even GPIO IRQHandler
 * @param none
 * @return none
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
	// Clear Interrupt
	GPIO_IntClear(1 << BUT0_pin);

	isB0FallingEdge = !isB0FallingEdge;
	Button0_Sample();
	// If falling edge turn off LED
	pushButton0State = ((isB0FallingEdge == true) ? LED_OFF : LED_ON);
}
#endif
