#include <stdio.h>
#include <stdbool.h>
#include "driver/elevio.h"

struct state {
    int current_floor;
    int motor_dir;    
    bool door_open;
};

bool sm_init(bool motor_running, int door_open, bool at_floor);
void sm_update(bool motor_running, bool door_open, bool at_floor);


