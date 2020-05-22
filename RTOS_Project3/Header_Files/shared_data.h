/**************************************************************************//**
 * @file shared_data.h
 * @brief Different data needed
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

typedef struct {
    uint32_t up_count;
    uint32_t down_count;
    float cur_speed;
}VehicleSpeedData_t;

typedef struct {
    uint32_t left_count;
    uint32_t right_count;
    uint32_t cur_direction_ms;
    int8_t   cur_direction;
}VehicleDirectionData_t;

typedef struct {
    int8_t cur_direction;
    float cur_speed;
} DisplayData_t;
