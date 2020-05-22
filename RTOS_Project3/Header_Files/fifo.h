/**************************************************************************//**
 * @file fifo.h
 * @brief First in first out header
 * @author Brandon Lewien
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Brandon Lewien
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

#include <stdint.h>
#include <stdbool.h>

#define FIFO_DEPTH 10

typedef uint8_t FifoIndex_t;
typedef int8_t InputValue_t;

typedef struct {
    FifoIndex_t  head;
    FifoIndex_t  tail;
    InputValue_t input[FIFO_DEPTH];
} InputFifo_t;

void Fifo_Init(void);
void InputFifo_Put(InputFifo_t *p_Fifo, InputValue_t value);
bool InputFifo_Get(InputFifo_t *p_Fifo, InputValue_t *p_value);
