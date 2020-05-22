#include "app.h"
/******************************************************************************
 * @brief app_peripheral_setup - Application layer initialization
 * @param none
 * @return none
 *****************************************************************************/
void app_peripheral_setup(void){
	cmu_open();
	GPIO_Open();
	SysTick_Setup();
//	CAPSENSE_Init();
#if (LAB2_USE_INTERRUPT)
	GPIO_Interrupt_Setup();
#endif
}
