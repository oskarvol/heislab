
#include "state_machine.h"
#include "driver/elevio.h"
// #include <algorithm>
// #include <cstdio>
#include <threads.h>

bool stop_signal = false;
struct state current_state = {-1,
                             DIRN_STOP,
                             DIRN_STOP,
                             {0,0,0,0}};
void sm_init() {
    int floor;
    elevio_motorDirection(DIRN_STOP);
    elevio_doorOpenLamp(0); 

    while (current_state.current_floor == -1 && elevio_stopButton() == 0){ 
        elevio_motorDirection(DIRN_DOWN);
        current_state.current_floor = elevio_floorSensor();
        current_state.motor_dir = DIRN_DOWN;    
        floor = elevio_floorSensor();
        printf("utenfor floor\n");
    };
    printf("floor \n");
    elevio_motorDirection(DIRN_STOP);
    current_state.motor_dir = DIRN_STOP;

    if (current_state.current_floor != -1){
        current_state.current_floor = elevio_floorSensor();
        printf("ferdig\n");
        return;
    };
    
    sm_init();
    return;
};

void update_cab_buttons_pressed(int floor_pressed){
    int i = 0;
    while (current_state.cab_buttons_pressed[i] != 0){
        i++;
        if (i >= 4){
            break;
        };
    };
    current_state.cab_buttons_pressed[i] = floor_pressed;
}

void floor_reached(){
    for (int i = 0; i <= 2; i++){
        current_state.cab_buttons_pressed[i] = current_state.cab_buttons_pressed[i+1];
    };
    current_state.cab_buttons_pressed[3] = 0;
};

void sort_cab_buttons_pressed(){
    int last_motor_dir = current_state.motor_dir;
};

void running(){
    printf("run\n");
    while (elevio_stopButton() == 0){
        for (int i = 0; i < 3; i++){
            if (elevio_callButton(i, 2) != 0){
                update_cab_buttons_pressed(i);
                printf("%d", i);
            };
        };
        if (current_state.cab_buttons_pressed[0] == 0 && current_state.current_floor != -1){
            elevio_motorDirection(DIRN_STOP);
            current_state.motor_dir = DIRN_STOP;

        // } else if (current_state.cab_buttons_pressed[0] == 0 && current_state.current_floor == -1){
        //     while (current_state.current_floor == -1){
        //         elevio_motorDirection(DIRN_DOWN);
        //         current_state.motor_dir = DIRN_DOWN;
        //         current_state.current_floor = elevio_floorSensor();
        //     };
        //     elevio_motorDirection(DIRN_STOP);
        //     current_state.motor_dir = DIRN_STOP;

        } else if (current_state.cab_buttons_pressed[0] != 0){
            int current_floor = current_state.current_floor;
            int goal = current_state.cab_buttons_pressed[0];
            int current_dir = current_state.motor_dir;

            if (current_floor < goal){
                elevio_motorDirection(DIRN_UP);
                current_state.motor_dir = DIRN_UP;
            }
            else if (current_floor > goal){
                elevio_motorDirection(DIRN_DOWN);
                current_state.motor_dir = DIRN_DOWN;
            }
            else if (current_floor == goal){
                floor_reached();
            }
        }
    };
};