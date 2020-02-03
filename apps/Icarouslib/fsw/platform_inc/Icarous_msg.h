/**
 * @file icarous_msg.h
 * @brief Icarous common message definitions
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cfe.h"

#define MAX_WAYPOINTS 50
#define MAX_VERTICES 100
#define ACID_SIZE 20
#define MAX_FIX_NAME_SIZE 20
#define MAX_GEOFENCES 50

/**
 * @defgroup ICAROUS_COMMON_MESSAGES
 * @brief Messages generated by the ardupilot application.
 * @ingroup ICAROUS_COMMON
 * @ingroup MESSAGES
 * @{
 */

/**
 * @enum icarousControlMode_e
 * @brief Defines the mode of operation of Icarous
 */
typedef enum {
	_PASSIVE_,     ///< Passive mode, Icarous only monitors and logs data
	_ACTIVE_,      ///< Active mode, Icarous will interfere if conflicts are about to be violated.
	_INACTIVE_     ///< Inactive mode.
} icarousControlMode_e;

/**
 * @enum severity
 * @brief Severity values
 */
typedef enum {
   SEVERITY_EMERGENCY=0, /* System is unusable. This is a "panic" condition. | */
   SEVERITY_ALERT=1, /* Action should be taken immediately. Indicates error in non-critical systems. | */
   SEVERITY_CRITICAL=2, /* Action must be taken immediately. Indicates failure in a primary system. | */
   SEVERITY_ERROR=3, /* Indicates an error in secondary/redundant systems. | */
   SEVERITY_WARNING=4, /* Indicates about a possible future error if this is not resolved within a given timeframe. Example would be a low battery warning. | */
   SEVERITY_NOTICE=5, /* An unusual event has occured, though not an error condition. This should be investigated for the root cause. | */
   SEVERITY_INFO=6, /* Normal operational messages. Useful for logging. No action is required for these messages. | */
   SEVERITY_DEBUG=7, /* Useful non-operational messages that can assist in debugging. These should not occur during normal operation. | */
}severity_e;


/**
 * @enum  commands
 * @brief Commands that are set to the autopilot app from other applications
 */
typedef enum {
	_ARM_,           ///< Arm motors : will be deprecated
	_TAKEOFF_,       ///< Start the takeoff sequence (used in quadcopters). 1 parameter (takeoff altitude [m])
	_LAND_,          ///< Start the landing sequence
	_GOTOWP_,        ///< Goto waypoint. 1 parameter (waypoint index)
	_SETPOS_,        ///< Set position. 3 parameters (lat [deg], lon [deg], alt [m])
	_SETVEL_,        ///< Set velocity. 3 parameters (Vn [m/s], Ve [m/s], Vu [m/s])
	_SETYAW_,        ///< Set yaw. 4 parameters (target angle [deg], angular rate [deg/s], direction (clk,anit-clk) [1,-1], type (1/0) [relative/absolute] )
	_SETSPEED_,      ///< Set speed. 1 parameter (speed [m/s])
	_SETMODE_,       ///< Set mode. will be deprecated
	_STATUS_,        ///< Status command
	_DITCH_,         ///< Start ditching status
	_RADAR_,         ///< Start radar
	_GUIDANCE_,      ///< Commands for guidance applications
	_TRAFFIC_RES_    ///< Traffic resolution
} commandName_e;


/**
 * @enum geofenceType_e
 * @brief geofence type
 */
typedef enum{
    _KEEPIN_,  ///< Keep in fence
    _KEEPOUT_  ///< Keep out fence
}geofenceType_e;

/**
 * @enum objectType_e
 * @brief object type
 */
typedef enum{
    _TRAFFIC_SIM_,   ///< traffic data from simulation
    _TRAFFIC_ADSB_,  ///< traffic data from ADSB
    _TRAFFIC_RADAR_, ///< traffic data from radar
    _OBSTACLE_       ///< obstacle
}objectType_e;


/**
 * @enum wp_metric_e
 * @brief waypoint metrix
 */
typedef enum {
	WP_METRIC_NONE      = 0,///< None
	WP_METRIC_ETA       = 1,///< Estimated time of arrival (s) at the next waypoint
	WP_METRIC_SPEED     = 2,///< Speed(m/s) en route to next waypoint
	WP_METRIC_ALTITUDE  = 3 ///< Altitude
}wp_metric_e;


/**
 * @struct waypoint_t
 * @brief waypoint data.
 */
typedef struct __attribute__((__packed__))
{
	uint16  index;                          /**< waypoint index */
	char    name[MAX_FIX_NAME_SIZE];        /**< waypoint name */
	double  latitude; 					    /**< latitude in degrees */
	double  longitude; 					    /**< longitude in degrees */
	double  altitude;                       /**< altitude Meters */
	int     wp_metric;                      /**< see @see wp_metric_e */
	double  value_to_next_wp;               /**< wp_metric value to next waypoint. */
}waypoint_t;

/**
 * @struct flightplan_t
 * @brief message encoding flight plan information
 */
typedef struct __attribute__((__packed__))
{
	uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	char     id[ACID_SIZE];                  /**< identifier */
	int      num_waypoints;                  /**< total waypoints. Cannot be greater than max. */
	double   scenario_time;                  /**< flight plan time */
	waypoint_t waypoints[MAX_WAYPOINTS];     /**< waypoint data */
}flightplan_t;


/**
 * @struct missionItemReached_t
 * @brief message indicating a specific waypoint has been reached.
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	char planID[ACID_SIZE];                 /**< index of flight plan */
	uint8_t reachedwaypoint;                /**< waypoint index that was reached */
    bool feedback;                          /**< this should be set to true if this is data from the autopilot */
}missionItemReached_t;

/**
 * @struct geofence_t
 * @brief message encoding geofence vertex information.
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	uint8_t type;                           /**< geofence type: see geofence_type_t */
    uint16_t index;                         /**< geofence index */
	uint16_t totalvertices;                 /**< total vertices in this geofence */
    double vertices[MAX_VERTICES][2];       /**< lat,lon (deg,deg) */
	double floor;                           /**< floor of geofence (m) */
	double ceiling;                         /**< roof of geofence (m) */
}geofence_t;

/**
 * @struct object_t
 * @brief message to represent information about an static/dynamic object
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
	uint8_t type;                             /**< object type: see object_type_t */
	uint32_t index;                           /**< id of object */
	char callsign[25];                        /**< call sign */
	double latitude;                          /**< latitude (degrees) */
	double longitude;                         /**< longitude (degrees) */
	double altitude;                          /**< altitude (degrees) */
	double ve;                                /**< velocity East component */
	double vn;                                /**< velocity North component */
	double vd;                                /**< velocity Up component */
}object_t;

/**
 * @struct position_t
 * @brief position information of aircraft.
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
    uint32_t aircraft_id;                     /**< aircraft id */
	char call_sign[25];                       /**< aircraft call sign */
	double time_gps;                          /**< gps time */
    double time_boot;                         /**< boot time of onboard autopilot */
	double latitude;                          /**< latitude (degrees) */
	double longitude;                         /**< longitude (degrees) */
	double altitude_abs;                      /**< absolution altitude, ASL (m)  */
	double altitude_rel;                      /**< relative altitude, AGL (m) */
	double vn;                                /**< velocity North component (m/s)*/
	double ve;                                /**< velocity East component (m/s)*/
	double vd;                                /**< velocity Down component (m/s)*/
    double hdg;                               /**< heading in degrees */
	uint16_t hdop;                            /**< GPS Horizontal Dilution of Precision */
	uint16_t vdop;                            /**< GPS Vertical Dilution of Precision */
	int numSats;                              /**< Total number of satellites being used for localization */
}position_t;


/**
 * @struct local_position_t
 * @brief aircraft local position information.
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
    double time_boot_ms;                      /**< boot time of onboard autopilot (ms) */
    double x;                                 /**< x position (m) */
    double y;                                 /**< y position (m) */
    double z;                                 /**< z position (m) */
    double vx;                                /**< x speed (m/s) */
    double vy;                                /**< y speed (m/s) */
    double vz;                                /**< z speed (m/s) */
}local_position_t;


/**
 * @struct attitude_t
 * @brief aircraft attitude information.
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];     /**< cFS header information */
    double time_boot;							/**< boot time of onboard autopilot (ms) */
	double roll;                                /**< roll angle (degree) */
	double pitch;                               /**< pitch angle (degree) */
	double yaw;                                 /**< yaw angle (degree) */
	double rollspeed;							/**< roll speed (degree/second) */
	double pitchspeed;							/**< pitch speed (degree/second) */
	double yawspeed;							/**< yaw speed (degree/second) */
}attitude_t;

/**
 * @struct noArgsCmd_t
 * @brief Command without arguments
 *
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
	uint64_t name;                           /**< command name */
}noArgsCmd_t;

/**
 * @struct argsCmd_t
 * @brief Command with arguments
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
	uint64_t name;                           /**< command name */
	double param1,param2;                    /**< command arguments */
	double param3,param4;                    /**< command arguments */
	double param5,param6;                    /**< command arguments */
	double param7,param8;                    /**< command arguments */
	char buffer[50];                         /**< command arguments */
}argsCmd_t;

/**
 * @struct cmdAck_t
 * @brief Command acknowledgement
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
	uint64_t name;                           /**< command name */
	int result;                              /**< result */
}cmdAck_t;


/**
 * @struct status_t
 * @brief Message to provide status information
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
    char buffer[250];                         /**< status message */
    int8_t severity;                          /**< severity - see severity_t */
}status_t;

/**
 * @struct vfrHud_t
 * @brief Message containing information to be displayed on a hud
 */
typedef struct{
	uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
    double airspeed;                          /**< airspeed in m/s */
	double groundspeed;                       /**< groundspeed in m/s */
	int16_t heading;                          /**< heading [0,360] 0 = north */
	uint16_t throttle;                        /**< throttle % */
	double alt;                               /**< altitude in m */
	double climb;                             /**< climb rate in m/s */
	uint8_t modeAP;                           /**< pixhawk mode */
	uint8_t modeIcarous;                      /**< icarous mode */
	uint8_t modeFlagAP;                       /**< pixhawk mode flag */
	uint16_t waypointCurrent;                 /**< current waypoint */
	uint32_t home_latitude;                   /**< latitude in degE7 */
    uint32_t home_longitude;                  /**< longitude in degE7 */
    uint32_t home_altitude;                   /**< altitude in mm */
}vfrhud_t;

/**
 * @struct battery_status_t
 * @brief Message containing battery status
 */
typedef struct{
	uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
	uint8_t id;		                          /**< battery id*/
	uint8_t battery_function;                 /**< function of the battery */
	uint8_t type;                          	  /**< type (chemistry) of battery */
	int16_t temperature;                      /**< Temperature of the battery in deg C, INT16_MAX for unknown */
	uint16_t voltages[10];                    /**< voltage of battery cells in mV, cells above cell count = UINT16_MAX*/
	int16_t current_battery;                  /**< battery current in cA, -1 means autopilot does not measure current */
	int32_t current_consumed;                 /**< consumed charge in mAh, -1 means autopilot does not provide */
	int32_t energy_consumed;                  /**< consumed energy in hJ, -1 means autopilot does not provide */
	int8_t battery_remaining;                 /**< remaining energy, [0-100]%, -1 means autopilot does not provide */
}battery_status_t;

/**
 * @struct rc_channels_t
 * @brief Message containing rc channel information
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
    uint32_t time_boot_ms;                                        /**< timestamp */
    uint8_t chancount;                                            /**< number of rc channels being received */
    uint16_t chan[20];                                            /**< RC channel values */
    uint8_t rssi;                                                 /**< receive signal strength */
}rc_channels_t;



/**@}*/

#endif //COMMON_DEFS_H_

