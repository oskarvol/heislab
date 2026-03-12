#include "state_machine.h"
#include "requests.h"
#include "scheduler.h"
#include "door.h"
#include "motor.h"
#include "driver/elevio.h"

struct state current_state = {
    -1, DIRN_STOP, DIRN_STOP,
    {-1, -1, -1, -1},
    {0, 0, 0},
    {0, 0, 0}
};

void sm_init(void) {
    elevio_motorDirection(DIRN_STOP);
    door_wait_obstruction();

    elevio_doorOpenLamp(0);
    elevio_stopLamp(0);

    while (current_state.current_floor == -1 && !elevio_stopButton()) {
        elevio_motorDirection(DIRN_DOWN);
        current_state.motor_dir = DIRN_DOWN;
        current_state.current_floor = elevio_floorSensor();
    }

    elevio_motorDirection(DIRN_STOP);
    current_state.motor_dir = DIRN_STOP;

    for (int i = 0; i < N_FLOORS; i++)         elevio_buttonLamp(i, BUTTON_CAB, 0);
    for (int f = 0; f < N_FLOORS - 1; f++)     elevio_buttonLamp(f, BUTTON_HALL_UP, 0);
    for (int f = 1; f < N_FLOORS; f++)         elevio_buttonLamp(f, BUTTON_HALL_DOWN, 0);

    if (current_state.current_floor == -1) {
        sm_init();
    }
}

void stop_rutine(void) {
    set_motor_dir(&current_state, DIRN_STOP);
    if (elevio_floorSensor() != -1) {
        current_state.current_floor = elevio_floorSensor();
    }

    elevio_stopLamp(1);

    for (int i = 0; i < N_FLOORS; i++) {
        current_state.cab_buttons_pressed[i] = -1;
        elevio_buttonLamp(i, BUTTON_CAB, 0);
    }
    for (int i = 0; i < N_FLOORS - 1; i++) {
        current_state.button_hall_up_pressed[i] = 0;
        current_state.button_hall_down_pressed[i] = 0;
        elevio_buttonLamp(i, BUTTON_HALL_UP, 0);
        elevio_buttonLamp(i + 1, BUTTON_HALL_DOWN, 0);
    }

    if (elevio_floorSensor() != -1) {
        elevio_doorOpenLamp(1);
    }

    while (elevio_stopButton()) {}

    elevio_stopLamp(0);
    door_wait_timed();
    door_wait_obstruction();
    elevio_doorOpenLamp(0);
}

void running(void) {
    printf("run\n");
    while (1) {
        if (elevio_stopButton()) {
            stop_rutine();
        }
        while (elevio_stopButton()) {}

        poll_buttons();

        if (elevio_floorSensor() != -1) {
            current_state.current_floor = elevio_floorSensor();
            elevio_floorIndicator(current_state.current_floor);
        }

        int goal = update_goal(&current_state);
        int current_floor = current_state.current_floor;

        if (goal == -1) {
            set_motor_dir(&current_state, DIRN_STOP);
        } else if (current_floor < goal) {
            set_motor_dir(&current_state, DIRN_UP);
            elevio_doorOpenLamp(0);
        } else if (current_floor > goal) {
            set_motor_dir(&current_state, DIRN_DOWN);
            elevio_doorOpenLamp(0);
        } else {
            if (elevio_floorSensor() == current_floor) {
                floor_reached();
            } else if (current_state.motor_dir == DIRN_STOP) {
                MotorDirection dir = (current_state.last_motor_dir != DIRN_STOP)
                                     ? current_state.last_motor_dir : DIRN_DOWN;
                set_motor_dir(&current_state, dir);
            }
        }
    }
}
