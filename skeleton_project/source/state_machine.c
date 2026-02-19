
#include "state_machine.h"
#include "driver/elevio.h"
#include <threads.h>

bool stop_signal = false;
struct state current_state;
bool sm_init() {
    int floor;
    elevio_motorDirection(0);
    current_state.motor_dir = 0;
    current_state.current_floor = elevio_floorSensor();
    elevio_doorOpenLamp(0); 

    while (current_state.current_floor == -1 && elevio_stopButton() == 0){ 
        elevio_motorDirection(-1);
        current_state.current_floor = elevio_floorSensor();
        current_state.motor_dir = -1;    
        floor = elevio_floorSensor();
        printf("utenfor floor\n");
    };
    printf("floor \n");
    elevio_motorDirection(0);
    current_state.motor_dir = 0;


    if (current_state.current_floor != -1){
        current_state.current_floor = elevio_floorSensor();
        printf("ferdig\n");
        return true;
    };
    
    sm_init();
    return false;
};