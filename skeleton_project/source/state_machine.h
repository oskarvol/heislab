#include <stdio.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include <unistd.h>
#include <sys/select.h>

struct state {
    int current_floor;
    int motor_dir;
    int last_motor_dir;   
    int cab_buttons_pressed [4];
    int button_hall_up_pressed [3];
    int button_hall_down_pressed [3];
};
// struct state current_state;

void sm_init(void);

void update_cab_buttons_pressed(int floor_pressed);

void update_button_hall(ButtonType button, int floor);

void floor_reached(void);

void stop_rutine(void);

void sort_cab_buttons_pressed(void);

void running(void);