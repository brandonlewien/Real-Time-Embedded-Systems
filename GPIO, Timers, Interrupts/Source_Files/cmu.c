#include "cmu.h"
/******************************************************************************
 * @brief cmu_open - Clocks and Oscillator stuff
 * @param none
 * @return none
 *****************************************************************************/
void cmu_open(void)
{
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
	CMU_HFXOInit(&hfxoInit);

	/* Switch HFCLK to HFRCO and disable HFRCO */
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

//	uint32_t	temp_freq;
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO);
	CMU_ClockEnable(cmuClock_HFPER, true);

	// By default, LFRCO is enabled
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

	// Route LF clock to the LF clock tree
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// routing clock to LFA

	CMU_ClockEnable(cmuClock_LFA, true);
	CMU_ClockEnable(cmuClock_CORELE, true);

	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);
}

