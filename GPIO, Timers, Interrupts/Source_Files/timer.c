#include "timer.h"

#if !(LAB2_USE_INTERRUPT)
static volatile uint32_t msTicks = 0;
#endif
/******************************************************************************
 * @brief SysTick_Handler - IRQ for systick
 * @param  none
 * @return none
 *****************************************************************************/
#if !(LAB2_USE_INTERRUPT)
void SysTick_Handler(void)
{
	msTicks++;
}
/***************************************************************************//**
 * @brief
 *   Delays number of msTick Systicks (typically 1 ms)
 *
 * @param dlyTicks
 *   Number of ticks (ms) to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}
#else
void SysTick_Handler(void)
{
	CAPSENSE_Sense();
	Slider_Sample();
	Drive_LEDs();
}
#endif
/******************************************************************************
 * @brief SysTick_Setup - Setup for systick
 * @param  none
 * @return none
 *****************************************************************************/
void SysTick_Setup(void)
{
#if !(LAB2_USE_INTERRUPT)
	/* Setup SysTick timer for 100 msec interrupts  */
	if (SysTick_Config(SystemCoreClockGet() / 1000))
	{
		while(1) ;
	}
#else
	/* Setup SysTick timer for 100 msec interrupts  */
	if (SysTick_Config(SystemCoreClockGet() / 10000))
	{
		while(1);
	}
#endif

}
