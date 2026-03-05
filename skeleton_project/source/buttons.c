#include "buttons.h"
#include "driver/elevio.h"

void update_cab_buttons_pressed(struct state *s, int floor_pressed){
    int i = 0;
    elevio_buttonLamp(floor_pressed, 2, 1);
    while (s->cab_buttons_pressed[i] != -1){
        i++;
        if (i >= 3){
            break;
        };
    };

    for (int i = 0; i < 4; i++){
        if (s->cab_buttons_pressed[i] == floor_pressed){
            return;  
        }
        if (s->cab_buttons_pressed[i] == -1){
            s->cab_buttons_pressed[i] = floor_pressed;
            return;
        }
    }

    for (int i = 0; i < 4; i++){
        printf("%d", s->cab_buttons_pressed[i]);
    }
    printf("\n");
}

void update_hall_button_pressed(struct state *s, ButtonType button, int floor){
    elevio_buttonLamp(floor, button, 1);
    if (button == 0){
        s->button_hall_up_pressed[floor] = 1;
    }
    if (button == 1){
        s->button_hall_down_pressed[floor-1] = 1;
    }

    char event[64];
    snprintf(event, sizeof(event), "HALL BUTTON PRESSED: floor=%d %s",
             floor, (button == BUTTON_HALL_UP) ? "UP" : "DOWN");
    // debug_log_hall_buttons(s, event);
}

int update_goal(struct state *s){
    int floor = s->current_floor;
    int dir = (s->motor_dir != DIRN_STOP) ? s->motor_dir : s->last_motor_dir;

    // Sjekk nåværende etasje
    for (int i = 0; i < 4; i++){
        if (s->cab_buttons_pressed[i] == -1){
            break;
        }
        if (s->cab_buttons_pressed[i] == floor){
            return floor;
        }
    }
    if (s->motor_dir != DIRN_DOWN && floor < N_FLOORS - 1 && s->button_hall_up_pressed[floor]){
        return floor;
    }
    if (s->motor_dir != DIRN_UP && floor > 0 && s->button_hall_down_pressed[floor - 1]){
        return floor;
    }

    // Finn retning
    int has_cab_above = 0;
    int has_cab_below = 0;
    for (int i = 0; i < 4; i++){
        if (s->cab_buttons_pressed[i] == -1){
            break;
        }
        if (s->cab_buttons_pressed[i] > floor){
            has_cab_above = 1;
        }
        if (s->cab_buttons_pressed[i] < floor){
            has_cab_below = 1;
        }
    }
    int has_cab = has_cab_above || has_cab_below;

    int go_up;
    if (has_cab_above && !has_cab_below){
        go_up = 1;
    } else if (has_cab_below && !has_cab_above){
        go_up = 0;
    } else {
        go_up = (dir != DIRN_DOWN);
    }

    if (go_up){
        if (has_cab){
            // Opp mot cab: stopp ved cab eller hall-opp underveis
            for (int f = floor + 1; f < N_FLOORS; f++){
                for (int i = 0; i < 4; i++){
                    if (s->cab_buttons_pressed[i] == -1){
                        break;
                    }
                    if (s->cab_buttons_pressed[i] == f){
                        return f;
                    }
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
            }
            // Ingenting opp, søk ned
            for (int f = floor - 1; f >= 0; f--){
                for (int i = 0; i < 4; i++){
                    if (s->cab_buttons_pressed[i] == -1){
                        break;
                    }
                    if (s->cab_buttons_pressed[i] == f){
                        return f;
                    }
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
            }
        } else {
            // Ingen cab, SCAN opp
            for (int f = floor + 1; f < N_FLOORS; f++){
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
            }
            // Snu og søk ned
            for (int f = floor - 1; f >= 0; f--){
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
            }
        }
    } else {
        if (has_cab){
            // Ned mot cab: stopp ved cab eller hall-ned underveis
            for (int f = floor - 1; f >= 0; f--){
                for (int i = 0; i < 4; i++){
                    if (s->cab_buttons_pressed[i] == -1){
                        break;
                    }
                    if (s->cab_buttons_pressed[i] == f){
                        return f;
                    }
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
            }
            // Ingenting ned, søk opp
            for (int f = floor + 1; f < N_FLOORS; f++){
                for (int i = 0; i < 4; i++){
                    if (s->cab_buttons_pressed[i] == -1){
                        break;
                    }
                    if (s->cab_buttons_pressed[i] == f){
                        return f;
                    }
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
            }
        } else {
            // Ingen cab, SCAN ned
            for (int f = floor - 1; f >= 0; f--){
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
            }
            // Snu og søk opp
            for (int f = floor + 1; f < N_FLOORS; f++){
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]){
                    return f;
                }
                if (f > 0 && s->button_hall_down_pressed[f - 1]){
                    return f;
                }
            }
        }
    }

    return -1;
}