
#ifndef SRC_HEADER_FILES_VEHICLE_H_
#define SRC_HEADER_FILES_VEHICLE_H_

struct vehicle_info_struct {
	char VehicleName[16];	// not null terminated
	uint16_t Mass;		// kg
	uint16_t MaxPower;		// kW
	uint16_t TurnRadius;		// cm, curb-to-curb
	uint16_t VehicleWidth;	// cm

	// Viewing Info, for 3-D POV
	uint16_t EyeballHeightAboveGround;	// cm
	uint16_t HorizontalAngleOfView;		// degrees
	uint16_t ViewInclination;			// degrees (positive=up)

	// Vehicle characteristics for slippage
	uint16_t DragArea;				// Cd*CrossSectionalArea as: Cd * Ft^2 * 100
	uint16_t TireType;				// 0:truck, 1:tourism, 2:highperformance
};

struct RoadLayout			{
	char Name[15];					// not Null delimited
	uint16_t RoadWidth;				// cm
//	uint16_t MaxBank;				// degrees
//	uint16_t CompensatedSpeed	;	// m/s.  The speed at which the road should be banked for
									// optimum driving, up to the MaxBank.  (See Info section)
	uint16_t NumWaypoints;			//
	struct {
		uint16_t x;					// m
		uint16_t y;					// m
//		uint16_t z;					// cm
		}* Waypoints;				// Array of waypoints.  First and last 2 are not to be driven
};


#endif /* SRC_HEADER_FILES_VEHICLE_H_ */
