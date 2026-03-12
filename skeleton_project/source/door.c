#include "door.h"
#include "state_machine.h"
#include "requests.h"
#include "motor.h"
#include "elevator_state.h"
#include "driver/elevio.h"
#include <unistd.h>
#include <stdio.h>

// Wait ~3 seconds with door open, polling buttons and checking stop
void door_wait_timed(void) {
    for (int i = 0; i < 1000; i++) {
        if (elevio_stopButton()) {
            stop_rutine();
        }
        poll_buttons();
        usleep(3000);
    }
}

// Wait until obstruction clears; if there was one, wait timed again
void door_wait_obstruction(void) {
    int was_obstructed = 0;
    while (elevio_obstruction()) {
        poll_buttons();
        was_obstructed = 1;
    }
    if (was_obstructed) {
        door_wait_timed();
    }
}

void floor_reached(void) {
    current_state.last_motor_dir = current_state.motor_dir;
    set_motor_dir(&current_state, DIRN_STOP);

    // Remove current floor from cab queue
    int j = 0;
    for (int i = 0; i < 4; i++) {
        if (current_state.cab_buttons_pressed[i] != current_state.current_floor) {
            current_state.cab_buttons_pressed[j++] = current_state.cab_buttons_pressed[i];
        }
    }
    while (j < 4) current_state.cab_buttons_pressed[j++] = -1;

    // Clear button lamps at this floor
    elevio_buttonLamp(current_state.current_floor, BUTTON_CAB, 0);
    if (current_state.current_floor == N_FLOORS - 1) {
        elevio_buttonLamp(current_state.current_floor, BUTTON_HALL_DOWN, 0);
    } else if (current_state.current_floor == 0) {
        elevio_buttonLamp(current_state.current_floor, BUTTON_HALL_UP, 0);
    } else {
        elevio_buttonLamp(current_state.current_floor, BUTTON_HALL_UP, 0);
        elevio_buttonLamp(current_state.current_floor, BUTTON_HALL_DOWN, 0);
    }

    // Clear hall requests at this floor
    if (current_state.current_floor > 0)
        current_state.button_hall_down_pressed[current_state.current_floor - 1] = 0;
    if (current_state.current_floor < N_FLOORS - 1)
        current_state.button_hall_up_pressed[current_state.current_floor] = 0;

    elevio_doorOpenLamp(1);
    door_wait_timed();
    door_wait_obstruction();
    elevio_doorOpenLamp(0);

    printf("floor: %d\n", current_state.current_floor);
}
