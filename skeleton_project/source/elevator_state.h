#pragma once
#include "driver/elevio.h"

struct state {
    int current_floor;
    int motor_dir;
    int last_motor_dir;
    int cab_buttons_pressed[4];
    int button_hall_up_pressed[3];
    int button_hall_down_pressed[3];
};

extern struct state current_state;
