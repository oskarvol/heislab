#include <threads.h>
#include "motor.h"
#include "driver/elevio.h"
#include "state_machine.h"


// void set_motor_dir(MotorDirection dirn){
//     elevio_motorDirection(dirn);
//     current_state.motor_dir = dirn;
// }