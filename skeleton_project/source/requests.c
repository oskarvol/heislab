#include "requests.h"
#include "driver/elevio.h"
#include <stdio.h>

void update_cab_buttons_pressed(struct state *s, int floor_pressed) {
    elevio_buttonLamp(floor_pressed, 2, 1);
    for (int i = 0; i < 4; i++) {
        if (s->cab_buttons_pressed[i] == floor_pressed) {
            return;
        }
        if (s->cab_buttons_pressed[i] == -1) {
            s->cab_buttons_pressed[i] = floor_pressed;
            return;
        }
    }
}

void update_hall_button_pressed(struct state *s, ButtonType button, int floor) {
    elevio_buttonLamp(floor, button, 1);
    if (button == BUTTON_HALL_UP) {
        s->button_hall_up_pressed[floor] = 1;
    }
    if (button == BUTTON_HALL_DOWN) {
        s->button_hall_down_pressed[floor - 1] = 1;
    }
}

void poll_buttons(void) {
    for (int floor = 0; floor < N_FLOORS; floor++) {
        if (elevio_callButton(floor, BUTTON_CAB)) {
            update_cab_buttons_pressed(&current_state, floor);
        }
    }
    for (int floor = 0; floor < N_FLOORS - 1; floor++) {
        if (elevio_callButton(floor, BUTTON_HALL_UP)) {
            update_hall_button_pressed(&current_state, BUTTON_HALL_UP, floor);
        }
    }
    for (int floor = 1; floor < N_FLOORS; floor++) {
        if (elevio_callButton(floor, BUTTON_HALL_DOWN)) {
            update_hall_button_pressed(&current_state, BUTTON_HALL_DOWN, floor);
        }
    }
}
