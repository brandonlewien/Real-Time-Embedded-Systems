///**************************************************************************//**
// * @file main.c
// * @brief GPIO, Timers, and Interrupts!
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
//
//#include <stdint.h>
//#include <stdbool.h>
//#include "em_device.h"
//#include "em_chip.h"
//#include "em_emu.h"
//#include "bsp.h"
//#include "main.h"
//#include "app.h"
//#include "capsense.h"
///******************************************************************************
// * @brief EM_Init - Configure Energy Modes
// * @param none
// * @return none
// *****************************************************************************/
//static void EM_Init(void)
//{
//	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
//
//	/* Initialize DCDC. Always start in low-noise mode. */
//	EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
//	EMU_DCDCInit(&dcdcInit);
//	em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
//	EMU_EM23Init(&em23Init);
//}
//int main(void)
//{
//    /* Chip errata */
//	CHIP_Init();
//	/* Energy Mode Initialization */
//	EM_Init();
//	/* Call application program to open / initialize all required peripheral */
//	app_peripheral_setup();
//
//#if !(LAB2_USE_INTERRUPT)
//	while(1) {
//		Delay(100);
//		CAPSENSE_Sense();
//		Slider_Sample();
//		Button1_Sample();
//		Button0_Sample();
//		Drive_LEDs();
//	}
//#else
//	while (1) {
//		EMU_EnterEM1();
//	}
//#endif
//}
