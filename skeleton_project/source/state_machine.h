#include <stdio.h>
#include <stdbool.h>
#include "driver/elevio.h"

struct state {
    int current_floor;
    int motor_dir;
    int last_motor_dir;   
    int cab_buttons_pressed [4];
};

void sm_init(void);

void update_cab_buttons_pressed(int floor_pressed);

void floor_reached(void);

void sort_cab_buttons_pressed();

void run();