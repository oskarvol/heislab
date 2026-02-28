#include <threads.h>
#include "motor.h"
#include "driver/elevio.h"


void set_motor_dir(struct state *s, MotorDirection dirn){
    elevio_motorDirection(dirn);
    s->motor_dir = dirn;
}