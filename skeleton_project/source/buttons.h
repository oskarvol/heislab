#include <stdio.h>
#include <stdbool.h>
#include "driver/elevio.h"
#include "state_machine.h"

void update_hall_button_pressed(struct state *s, ButtonType button, int floor);
void update_cab_buttons_pressed(struct state *s, int floor_pressed);

void update_goal(struct state *s);
