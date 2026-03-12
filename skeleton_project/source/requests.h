#pragma once
#include "elevator_state.h"

void poll_buttons(void);
void update_cab_buttons_pressed(struct state *s, int floor_pressed);
void update_hall_button_pressed(struct state *s, ButtonType button, int floor);
