#pragma once

/**
 * @file elevio.h
 * @brief Elevator I/O interface for communicating with the simulator.
 *
 * This module provides a small hardware abstraction used by the lab
 * elevator controller. Communication is performed over a TCP socket to
 * the simulator process.
 */

#define N_FLOORS 4 /**< Number of floors in the elevator system */

/**
 * @enum MotorDirection
 * @brief Direction commands for the elevator motor.
 */
typedef enum { 
    DIRN_DOWN   = -1, /**< Move elevator down */
    DIRN_STOP   = 0,  /**< Stop elevator */
    DIRN_UP     = 1   /**< Move elevator up */
} MotorDirection;


#define N_BUTTONS 3 /**< Number of distinct button types */

/**
 * @enum ButtonType
 * @brief Types of buttons available on each floor / in the cab.
 */
typedef enum { 
    BUTTON_HALL_UP      = 0, /**< Hall call: request to go up */
    BUTTON_HALL_DOWN    = 1, /**< Hall call: request to go down */
    BUTTON_CAB          = 2  /**< Cab (inside elevator) button */
} ButtonType;


/**
 * @brief Initialise the elevio module and connect to the simulator.
 *
 * Reads configuration from `source/driver/elevio.con` and opens a TCP
 * connection to the elevator simulator. Must be called before other
 * `elevio_*` functions.
 */
void elevio_init(void);

/**
 * @brief Set motor direction.
 * @param dirn Direction command (one of `DIRN_UP`, `DIRN_DOWN`, `DIRN_STOP`).
 */
void elevio_motorDirection(MotorDirection dirn);

/**
 * @brief Set or clear a button lamp.
 * @param floor Floor index (0 .. N_FLOORS-1).
 * @param button Button type (`BUTTON_HALL_UP`, `BUTTON_HALL_DOWN`, `BUTTON_CAB`).
 * @param value Non-zero to turn the lamp on, zero to turn it off.
 */
void elevio_buttonLamp(int floor, ButtonType button, int value);

/**
 * @brief Update the floor indicator shown in the cab.
 * @param floor Floor index (0 .. N_FLOORS-1).
 */
void elevio_floorIndicator(int floor);

/**
 * @brief Control the door-open lamp.
 * @param value Non-zero to turn the lamp on, zero to turn it off.
 */
void elevio_doorOpenLamp(int value);

/**
 * @brief Control the stop lamp.
 * @param value Non-zero to turn the lamp on, zero to turn it off.
 */
void elevio_stopLamp(int value);

/**
 * @brief Query whether a call button is pressed.
 * @param floor Floor index (0 .. N_FLOORS-1).
 * @param button Button type to query.
 * @return Non-zero if the button is pressed, otherwise zero.
 */
int elevio_callButton(int floor, ButtonType button);

/**
 * @brief Read the floor sensor.
 * @return Current floor index (0 .. N_FLOORS-1) if the elevator is at a floor,
 * otherwise -1.
 */
int elevio_floorSensor(void);

/**
 * @brief Query the stop button state.
 * @return Non-zero if the stop button is pressed, otherwise zero.
 */
int elevio_stopButton(void);

/**
 * @brief Query the obstruction sensor state.
 * @return Non-zero if obstruction is active, otherwise zero.
 */
int elevio_obstruction(void);

