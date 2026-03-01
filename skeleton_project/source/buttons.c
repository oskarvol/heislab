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
}

// Returnerer neste etasjemål.
// Cab-knapper bestemmer reiseretning og har prioritet.
// På vei mot en cab-destinasjon stoppes det ved hall-knapper i samme retning.
// Uten cab-knapper brukes SCAN på hall-knapper alene.
int update_goal(struct state *s) {
    int floor = s->current_floor;
    int dir   = s->last_motor_dir;

    // Sjekk nåværende etasje først
    for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
        if (s->cab_buttons_pressed[i] == floor) return floor;
    }
    if (floor < N_FLOORS - 1 && s->button_hall_up_pressed[floor])     return floor;
    if (floor > 0             && s->button_hall_down_pressed[floor-1]) return floor;

    // Finn om det finnes cab-knapper over/under nåværende etasje
    int has_cab_above = 0, has_cab_below = 0;
    for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
        if (s->cab_buttons_pressed[i] > floor) has_cab_above = 1;
        if (s->cab_buttons_pressed[i] < floor) has_cab_below = 1;
    }
    int has_cab = has_cab_above || has_cab_below;

    // Cab-knapper bestemmer retning; ved uavgjort brukes siste retning
    int go_up;
    if      (has_cab_above && !has_cab_below) go_up = 1;
    else if (has_cab_below && !has_cab_above) go_up = 0;
    else                                      go_up = (dir != DIRN_DOWN);

    if (go_up) {
        if (has_cab) {
            // Kjører opp mot cab: stopp ved cab ELLER hall-opp underveis
            for (int f = floor + 1; f < N_FLOORS; f++) {
                for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++)
                    if (s->cab_buttons_pressed[i] == f) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f]) return f;
            }
            // Ingenting opp — snu og søk ned (cab og hall-ned)
            for (int f = floor - 1; f >= 0; f--) {
                for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++)
                    if (s->cab_buttons_pressed[i] == f) return f;
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
            }
        } else {
            // Ingen cab — SCAN opp: stopp ved alle hall-knapper
            for (int f = floor + 1; f < N_FLOORS; f++) {
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
            }
            // Snu og søk ned
            for (int f = floor - 1; f >= 0; f--) {
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
            }
        }
    } else {
        if (has_cab) {
            // Kjører ned mot cab: stopp ved cab ELLER hall-ned underveis
            for (int f = floor - 1; f >= 0; f--) {
                for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++)
                    if (s->cab_buttons_pressed[i] == f) return f;
                if (f > 0 && s->button_hall_down_pressed[f-1]) return f;
            }
            // Ingenting ned — snu og søk opp (cab og hall-opp)
            for (int f = floor + 1; f < N_FLOORS; f++) {
                for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++)
                    if (s->cab_buttons_pressed[i] == f) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
            }
        } else {
            // Ingen cab — SCAN ned: stopp ved alle hall-knapper
            for (int f = floor - 1; f >= 0; f--) {
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
            }
            // Snu og søk opp
            for (int f = floor + 1; f < N_FLOORS; f++) {
                if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])     return f;
                if (f > 0             && s->button_hall_down_pressed[f-1]) return f;
            }
        }
    }

    return -1;
}