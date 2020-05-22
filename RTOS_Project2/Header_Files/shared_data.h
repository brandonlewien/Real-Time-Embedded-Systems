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
