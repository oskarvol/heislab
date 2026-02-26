
#include "state_machine.h"
#include "driver/elevio.h"
// #include <algorithm>
// #include <cstdio>
#include <threads.h>

bool stop_signal = false;
struct state current_state = {-1,
                             DIRN_STOP,
                             DIRN_STOP,
                             {-1,-1,-1,-1}, 
                             {0, 0, 0},
                             {0, 0, 0}};
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
    for (int i = 0; i < 4; i++){
        elevio_buttonLamp(i, 2, 0);
    }

    if (current_state.current_floor != -1){
        current_state.current_floor = elevio_floorSensor();
        printf("ferdig\n%d", elevio_floorSensor());
        return;
    };
    
    sm_init();
    
    return;
};

void update_cab_buttons_pressed(int floor_pressed){
    int i = 0;
    elevio_buttonLamp(floor_pressed, 2, 1);
    while (current_state.cab_buttons_pressed[i] != -1){
        i++;
        if (i >= 3){
            break;
        };
    };

    for (int i = 0; i < 4; i++){
        if (current_state.cab_buttons_pressed[i] == floor_pressed){
            return;  
        }
        if (current_state.cab_buttons_pressed[i] == -1){
            current_state.cab_buttons_pressed[i] = floor_pressed;
            return;
        }
    }

    for (int i = 0; i < 4; i++){
        printf("%d", current_state.cab_buttons_pressed[i]);
    }
    printf("\n");
}

void update_button_hall(ButtonType button, int floor){
    int i = 0;
    elevio_buttonLamp(floor, button, 1);
    if (button == 1){
        current_state.button_hall_down_pressed[floor] = 1;
    };
    if (button == 2){
        current_state.button_hall_up_pressed[floor] = 1;
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
    elevio_doorOpenLamp(1);
    for (int i = 0; i < 1000; i++){
        if(elevio_stopButton()){
            stop_rutine();
        }
        usleep(3000);
    }
    elevio_doorOpenLamp(0);
    printf("floor; %d, \t", current_state.current_floor);
};

void sort_cab_buttons_pressed(){
    int last_motor_dir = current_state.motor_dir;

};
void stop_rutine(){
    elevio_motorDirection(DIRN_STOP);
    current_state.motor_dir = DIRN_STOP;
    elevio_stopLamp(1);
    for (int i = 0; i < N_FLOORS; i++){
        current_state.cab_buttons_pressed[i] = -1;
        elevio_buttonLamp(i, 2, 0);
    }
    if (elevio_floorSensor() != -1){
        elevio_doorOpenLamp(1);
    }
}
void running(){
    printf("run\n");
    while (1){
        if(elevio_stopButton()){
            stop_rutine();
        }
        elevio_stopLamp(0);
        for (int i = 0; i < N_FLOORS; i++){
            if (elevio_callButton(i, 2) != 0){
                update_cab_buttons_pressed(i);
                printf("Pressed: %d, \t", i);
            };
        };
        if (elevio_floorSensor() != -1){
            current_state.current_floor = elevio_floorSensor();
            for(int i = 0; i < 4; i++){
                if (i == current_state.current_floor){
                    elevio_floorIndicator(i);
                }
            }
        }
        if (current_state.cab_buttons_pressed[0] == -1 && current_state.current_floor != -1){
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

        } else if (current_state.cab_buttons_pressed[0] != -1){
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