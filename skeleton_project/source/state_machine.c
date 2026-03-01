
#include "state_machine.h"
#include "driver/elevio.h"
// #include <algorithm>
// #include <cstdio>
#include <threads.h>
#include "buttons.h"
#include "motor.h"

bool stop_signal = false;
struct state current_state = {-1,
                             DIRN_STOP,
                             DIRN_STOP,
                             {-1,-1,-1,-1}, 
                             {0, 0, 0},
                             {0, 0, 0}};
void sm_init() {
    int floor;
    int j = 0;
    elevio_motorDirection(DIRN_STOP);
    while(elevio_obstruction() != 0){
        j = 1;
        continue;
    }
    if (j == 1){
        for (int i = 0; i < 1000; i++){
            if(elevio_stopButton()){
                stop_rutine();
            }
            usleep(3000);
        }
    }
    elevio_doorOpenLamp(0); 
    elevio_stopLamp(0);

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
    for (int i = 0; i < 4; i++){
        elevio_buttonLamp(i, 2, 0);
    }

    for (int floor = 0; floor < N_FLOORS - 1; floor++) {
        elevio_buttonLamp(floor, 0, 0);
    }

    for (int floor = 1; floor < N_FLOORS; floor++) {
        elevio_buttonLamp(floor, 1, 0);
    }

    if (current_state.current_floor != -1){
        current_state.current_floor = elevio_floorSensor();
        printf("ferdig\n%d", elevio_floorSensor());
        return;
    };
    
    sm_init();
    
    return;
};



void poll_buttons(){
    for (int floor = 0; floor < N_FLOORS; floor++){
        if (elevio_callButton(floor, 2) != 0){
            update_cab_buttons_pressed(&current_state, floor);
            printf("Pressed: %d, \t", floor);
        };
    };

    for (int floor = 0; floor < N_FLOORS - 1; floor++) {
        if (elevio_callButton(floor, 0)) {
            update_hall_button_pressed(&current_state, 0, floor);
        }
    }

    for (int floor = 1; floor < N_FLOORS; floor++) {
        if (elevio_callButton(floor, 1)) {
            update_hall_button_pressed(&current_state, 1, floor);
        }
    }
}

void floor_reached(){
    current_state.last_motor_dir = current_state.motor_dir;
    elevio_motorDirection(DIRN_STOP);
    current_state.motor_dir = DIRN_STOP;

    for (int i = 0; i <= 2; i++){
        current_state.cab_buttons_pressed[i] = current_state.cab_buttons_pressed[i+1];
    };

    current_state.cab_buttons_pressed[3] = -1;
    elevio_buttonLamp(current_state.current_floor, 2, 0);

    if (current_state.current_floor == 3){
        elevio_buttonLamp(current_state.current_floor, 1, 0);
    } else if (current_state.current_floor == 0){
        elevio_buttonLamp(current_state.current_floor, 0, 0);
    } else {
         elevio_buttonLamp(current_state.current_floor, 0, 0);
         elevio_buttonLamp(current_state.current_floor, 1, 0);
    }

    current_state.button_hall_down_pressed[current_state.current_floor-1] = 0;
    current_state.button_hall_up_pressed[current_state.current_floor] = 0;

    elevio_doorOpenLamp(1);
    for (int i = 0; i < 1000; i++){
        if(elevio_stopButton()){
            stop_rutine();
        }
        poll_buttons();
        usleep(3000);
    }
    int j = 0;
    while(elevio_obstruction() != 0){
        j = 1;
        continue;
    }
    if (j == 1){
        for (int i = 0; i < 1000; i++){
            if(elevio_stopButton()){
                stop_rutine();
            }
            poll_buttons();
            usleep(3000);
        }
    }
    elevio_doorOpenLamp(0);
    printf("floor; %d, \t", current_state.current_floor);
};

void stop_rutine(){
    set_motor_dir(&current_state,  DIRN_STOP);
    current_state.current_floor = elevio_floorSensor();
    elevio_stopLamp(1);
    for (int i = 0; i < N_FLOORS; i++){
        current_state.cab_buttons_pressed[i] = -1;
        elevio_buttonLamp(i, 2, 0);
    }
    for (int floor = 0; floor < N_FLOORS-1; floor++){
        elevio_buttonLamp(floor, 0, 0);
        elevio_buttonLamp(floor+1, 1, 0);
        
    }

    if (elevio_floorSensor() != -1){
        elevio_doorOpenLamp(1);
    }
    while (elevio_stopButton()){
        continue;
    }
    elevio_stopLamp(0);
    for (int i = 0; i < 1000; i++){
        if(elevio_stopButton()){
            stop_rutine();
        }
        usleep(3000);
    }
    int j = 0;
     while(elevio_obstruction() != 0){
        j = 1;
        continue;
    }
    if (j == 1){
        for (int i = 0; i < 1000; i++){
            if(elevio_stopButton()){
                stop_rutine();
            }
            usleep(3000);
        }
    }
    
    elevio_doorOpenLamp(0);

    return;
}
void running(){
    printf("run\n");
    while (1){
        if(elevio_stopButton()){
            stop_rutine();
        } 
    
        
        poll_buttons();

        if (elevio_floorSensor() != -1){
            current_state.current_floor = elevio_floorSensor();
            elevio_floorIndicator(current_state.current_floor);
        }
        if (current_state.cab_buttons_pressed[0] == -1 && current_state.current_floor != -1){
            set_motor_dir(&current_state,  DIRN_STOP);

        // } else if (current_state.cab_buttons_pressed[0] == 0 && current_state.current_floor == -1){
        //     while (current_state.current_floor == -1){
        //         elevio_motorDirection(DIRN_DOWN);
        //         current_state.motor_dir = DIRN_DOWN;
        //         current_state.current_floor = elevio_floorSensor();
        //     };
        //     elevio_motorDirection(DIRN_STOP);
        //     current_state.motor_dir = DIRN_STOP;

        // update_goal(&current_state);
    } else if (current_state.cab_buttons_pressed[0] != -1){
        int current_floor = current_state.current_floor;
        int current_dir = current_state.motor_dir;
        int goal = current_state.cab_buttons_pressed[0];
            int j = 0;
            while(elevio_obstruction() != 0){
                j = 1;
                continue;
            }
            if (j == 1){
                for (int i = 0; i < 1000; i++){
                    if(elevio_stopButton()){
                        stop_rutine();
                    }
                    usleep(3000);
                }
            }
            if (current_floor < goal){
                set_motor_dir(&current_state, DIRN_UP);
                int j = 0;
                while(elevio_obstruction() != 0){
                    j = 1;
                    continue;
                }
                if (j == 1){
                    for (int i = 0; i < 1000; i++){
                        if(elevio_stopButton()){
                            stop_rutine();
                        }
                        usleep(3000);
                    }
                }
                elevio_doorOpenLamp(0);
            }
            else if (current_floor > goal){
                set_motor_dir(&current_state, DIRN_DOWN);
                  int j = 0;
                while(elevio_obstruction() != 0){
                    j = 1;
                    continue;
                }
                if (j == 1){
                    for (int i = 0; i < 1000; i++){
                        if(elevio_stopButton()){
                            stop_rutine();
                        }
                        usleep(3000);
                    }
                }
                elevio_doorOpenLamp(0);
            }
            else if (current_floor == goal){
                floor_reached();
            }
        }
    };
};