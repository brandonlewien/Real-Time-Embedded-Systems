/**************************************************************************//**
 * @file vehicle.h
 * @brief Vehicle Structures
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

#ifndef SRC_HEADER_FILES_VEHICLE_H_
#define SRC_HEADER_FILES_VEHICLE_H_

typedef struct {
	char VehicleName[16];	// not null terminated
	uint16_t Mass;		    // kg
	uint16_t MaxPower;		// kW
	uint16_t TurnRadius;    // cm, curb-to-curb
	uint16_t VehicleWidth;	// cm

	// Vehicle characteristics for slippage
	uint16_t DragArea;		// Cd*CrossSectionalArea as: Cd * Ft^2 * 100
	uint16_t TireType;		// 0:truck, 1:tourism, 2:highperformance
}vehicle_info;

typedef struct {
	char Name[15];					// not Null delimited
	uint16_t RoadWidth;				// cm
	uint16_t NumWaypoints;			//
	struct {
		uint16_t x;					// m
		uint16_t y;					// m
	}* Waypoints;				// Array of waypoints.  First and last 2 are not to be driven
}RoadLayout;


#endif /* SRC_HEADER_FILES_VEHICLE_H_ */
