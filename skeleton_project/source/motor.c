#include "driver/elevio.h"
#include <threads.h>

void motor_run_up(){
    elevio_motorDirection(1);
}