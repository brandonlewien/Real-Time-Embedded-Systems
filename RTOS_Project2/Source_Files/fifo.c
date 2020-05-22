/*******************************************************************************
 * @file
 * @brief FIFO implementation
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <common/include/rtos_utils.h>

#include "fifo.h"

extern InputFifo_t * FifoBuffer;
static uint32_t Fifo_Size = 0;

void Fifo_Init(void)
{
	FifoBuffer = (InputFifo_t *)malloc(sizeof(InputFifo_t));
    for (int i = 0; i < FIFO_DEPTH; ++i)
    {
    	FifoBuffer->input[i] = -1;
    }
    FifoBuffer->head = 0;
    FifoBuffer->tail = 0;
}
/***************************************************************************//**
 * @brief
 *   Add a value to the FIFO
 *
 * @param value
 *   Value to add to the FIFO
 ******************************************************************************/
void InputFifo_Put(InputFifo_t *p_Fifo, InputValue_t value)
{
	if (p_Fifo->tail == FIFO_DEPTH)
	{
		p_Fifo->tail = 0;
	}
	if (Fifo_Size == 10)
	{
		// Overflow!
		return;
	}
    p_Fifo->input[p_Fifo->tail] = value;
	(p_Fifo->tail)++;
	Fifo_Size++;

    return;
}

/***************************************************************************//**
 * @brief
 *   Get the next value from the FIFO
 *
 * @return
 *   Next value in the FIFO
 ******************************************************************************/
bool InputFifo_Get(InputFifo_t *p_Fifo, InputValue_t *p_value)
{
	if (p_Fifo->head == FIFO_DEPTH)
	{
		p_Fifo->head = 0;
	}
	if (Fifo_Size == 0)
	{
		return false;
	}
	*p_value = p_Fifo->input[p_Fifo->head];
	p_Fifo->input[p_Fifo->head] = 0;
	p_Fifo->head++;
	Fifo_Size--;
    return true;
}




