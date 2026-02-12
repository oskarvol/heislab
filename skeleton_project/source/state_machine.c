#include "state_machine.h"
#include "driver/elevio.h"
#include <threads.h>

bool stop_signal = false;
struct state current_state;

bool sm_init(bool motor_running, int door_open, bool at_floor) {
    
    if (door_open) {
        elevio_doorOpenLamp(0);
    }
    current_state.door_open = false;

    while (!at_floor && elevio_stopButton() == 0){
        elevio_motorDirection(-1);
    };

    elevio_motorDirection(0);
    current_state.current_floor = elevio_floorSensor();
    return true;
};