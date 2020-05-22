/*******************************************************************************
 * @file
 * @brief FIFO declarations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
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
