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
        s->button_hall_down_pressed[floor-1] = 1;
    }
    if (button == 1){
        s->button_hall_up_pressed[floor] = 1;
    }
}

// Returnerer neste etasjemål basert på SCAN-algoritmen (heisalgoritmen):
//   - Søk fremover i nåværende retning (last_motor_dir)
//   - Stopp ved første etasje som har cab-knapp ELLER hall-knapp i reiseretningen
//   - Hvis ingenting finnes fremover, snu og søk bakover
//   - Returnerer -1 hvis ingen ventende ordre finnes
//
// Forslag: kall update_goal() etter floor_reached() og bruk returverdien
//           som ny cab_buttons_pressed[0] (erstatter den enkeltstående køen),
//           eller legg til et eget "goal"-felt i struct state.
int update_goal(struct state *s) {
    int floor = s->current_floor;
    int dir   = s->last_motor_dir;

    // Sjekk nåværende etasje først
    for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
        if (s->cab_buttons_pressed[i] == floor) return floor;
    }
    if (floor < N_FLOORS - 1 && s->button_hall_up_pressed[floor])    return floor;
    if (floor > 0             && s->button_hall_down_pressed[floor-1]) return floor;

    // Søk oppover hvis vi kjørte opp (eller sto stille)
    if (dir != DIRN_DOWN) {
        for (int f = floor + 1; f < N_FLOORS; f++) {
            for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
                if (s->cab_buttons_pressed[i] == f) return f;
            }
            if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])   return f;
            if (f > 0            && s->button_hall_down_pressed[f-1]) return f;
        }
        // Ingenting oppover — søk nedover
        for (int f = floor - 1; f >= 0; f--) {
            for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
                if (s->cab_buttons_pressed[i] == f) return f;
            }
            if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])   return f;
            if (f > 0            && s->button_hall_down_pressed[f-1]) return f;
        }
    } else {
        // Søk nedover
        for (int f = floor - 1; f >= 0; f--) {
            for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
                if (s->cab_buttons_pressed[i] == f) return f;
            }
            if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])   return f;
            if (f > 0            && s->button_hall_down_pressed[f-1]) return f;
        }
        // Ingenting nedover — søk oppover
        for (int f = floor + 1; f < N_FLOORS; f++) {
            for (int i = 0; i < 4 && s->cab_buttons_pressed[i] != -1; i++) {
                if (s->cab_buttons_pressed[i] == f) return f;
            }
            if (f < N_FLOORS - 1 && s->button_hall_up_pressed[f])   return f;
            if (f > 0            && s->button_hall_down_pressed[f-1]) return f;
        }
    }

    return -1; // Ingen ventende ordre
}