/**
 * @file trajectory_msg.h
 * @brief definition of messages generated by the trajectory messages
 */
#ifndef ICAROUS_CFS_TRAJECTORY_MSG_H
#define ICAROUS_CFS_TRAJECTORY_MSG_H

#include "cfe.h"

/**
 * @defgroup TRAJECTORY_MESSAGES
 * @brief Messages generated by the trajectory monitoring application
 * @ingroup TRAJECTORY_MONITOR
 * @ingroup MESSAGES
 * @{
 */

 /**
 * @enum algorithm_e
 * @brief search algorithm type
 */
typedef enum {
    _GRID,              ///< A simple grid based Astar algorithm (Requires a keep in geofence to bound the search space)
    _ASTAR,             ///< A pseudo-motion primitive type Astar algorithm
    _RRT,               ///< Rapidly exploring random tree based algorithm (Requires a keep in geofence to bound the search space)
    _SPLINES            ///< Bsplines based planner (experimental planner)
}algorithm_e;

/**
 * @struct trajectory_request_t
 * @brief Request computation of a trajectory
 */
typedef struct{
   uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
   algorithm_e algorithm;                  /**< algorithm to use */
   double initialPosition[3];              /**< initial position, lat (degree), lon (degree), alt (m) */
   double initialVelocity[3];              /**< initial velocity track (degree), ground speed (m/s), vertical speed (m/s) */
   double finalPosition[3];                /**< final position  (lat, lon, alt )*/
}trajectory_request_t;

/**
 * @struct flightplan_monitor_t
 * @brief Information regarding the mission flight plan
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
    int nextWP;                             /**< Next waypoint */
    double allowedXtrackError;              /**< allowed cross track deviaiton (m)*/
    double dist2NextWP;                     /**< distance to next WP (m)*/
    double crossTrackDeviation;             /**< cross track deviation (m) left(+), right(-)*/
    double interceptManeuver[3];            /**< intercept maneuver (track (degree), ground speed (m/s), vertical speed (m/s)*/
    double interceptHeadingToPlan;          /**< intercept heading to plan */
    double resolutionSpeed;                 /**< flight plan resolution speed (m/s); */
    algorithm_e searchType;                 /**< preferred search algorithm to be used for trajectory generation*/
}flightplan_monitor_t;

/**@}*/
#endif //ICAROUS_CFS_TRAJECTORY_MSG_H
