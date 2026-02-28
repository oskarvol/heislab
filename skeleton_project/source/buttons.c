#include "buttons.h"

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
    if (button == 1){
        s->button_hall_down_pressed[floor-1] = 1;
    };
    if (button == 2){
        s->button_hall_up_pressed[floor] = 1;
    }
}

void update_goal(struct state *s){
    if (s->last_motor_dir)
}