#include "scheduler.h"
#include "driver/elevio.h"

int update_goal(struct state *s) {
    int floor = s->current_floor;
    int dir = (s->motor_dir != DIRN_STOP) ? s->motor_dir : s->last_motor_dir;

    // Check current floor
    for (int i = 0; i < 4; i++) {
        if (s->cab_buttons_pressed[i] == -1) break;
        if (s->cab_buttons_pressed[i] == floor) return floor;
    }
    if (s->motor_dir != DIRN_DOWN && floor < N_FLOORS - 1 && s->button_hall_up_pressed[floor]) {
        return floor;
    }
    if (s->motor_dir != DIRN_UP && floor > 0 && s->button_hall_down_pressed[floor - 1]) {
        return floor;
    }

    // Determine preferred direction
    int has_cab_above = 0, has_cab_below = 0;
    for (int i = 0; i < 4; i++) {
        if (s->cab_buttons_pressed[i] == -1) break;
        if (s->cab_buttons_pressed[i] > floor) has_cab_above = 1;
        if (s->cab_buttons_pressed[i] < floor) has_cab_below = 1;
    }
    int has_cab = has_cab_above || has_cab_below;

    int go_up;
    if (has_cab_above && !has_cab_below) {
        go_up = 1;
    } else if (has_cab_below && !has_cab_above) {
        go_up = 0;
    } else {
        go_up = (dir != DIRN_DOWN);
    }

    if (go_up) {
        if (has_cab) {
            for (int f = floor + 1; f < N_FLOORS; f++) {
                for (int i = 0; i < 4; i++) {
                    if (s->cab_buttons_pressed[i] == -1) break;
                    if (s->cab_buttons_pressed[i] == f) return f;
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
            }
            for (int f = floor - 1; f >= 0; f--) {
                for (int i = 0; i < 4; i++) {
                    if (s->cab_buttons_pressed[i] == -1) break;
                    if (s->cab_buttons_pressed[i] == f) return f;
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
            }
        } else {
            for (int f = floor + 1; f < N_FLOORS; f++) {
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
            }
            for (int f = floor - 1; f >= 0; f--) {
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
            }
        }
    } else {
        if (has_cab) {
            for (int f = floor - 1; f >= 0; f--) {
                for (int i = 0; i < 4; i++) {
                    if (s->cab_buttons_pressed[i] == -1) break;
                    if (s->cab_buttons_pressed[i] == f) return f;
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
            }
            for (int f = floor + 1; f < N_FLOORS; f++) {
                for (int i = 0; i < 4; i++) {
                    if (s->cab_buttons_pressed[i] == -1) break;
                    if (s->cab_buttons_pressed[i] == f) return f;
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
            }
        } else {
            for (int f = floor - 1; f >= 0; f--) {
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
            }
            for (int f = floor + 1; f < N_FLOORS; f++) {
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
                if (f > 0 && s->button_hall_down_pressed[f - 1]) return f;
            }
        }
    }

    return -1;
}
