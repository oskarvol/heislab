#include <stdio.h>
#include <stdbool.h>
#include "driver/elevio.h"

struct state {
    int current_floor;
    int motor_dir;    

};

bool sm_init(void);
void sm_update(void);


