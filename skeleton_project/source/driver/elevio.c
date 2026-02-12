#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>

#include "elevio.h"
#include "con_load.h"

/**
 * @file elevio.c
 * @brief Implementation of the elevator I/O functions which communicate
 * with the simulator over a TCP socket.
 */

static int sockfd;
static pthread_mutex_t sockmtx;

/**
 * @brief Initialise the elevio module and open connection to simulator.
 */
void elevio_init(void){
    char ip[16] = "localhost";
    char port[8] = "15657";
    con_load("source/driver/elevio.con",
        con_val("com_ip",   ip,   "%s")
        con_val("com_port", port, "%s")
    )
    
    pthread_mutex_init(&sockmtx, NULL);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1 && "Unable to set up socket");
    
    struct addrinfo hints = {
        .ai_family      = AF_INET, 
        .ai_socktype    = SOCK_STREAM, 
        .ai_protocol    = IPPROTO_TCP,
    };
    struct addrinfo* res;
    getaddrinfo(ip, port, &hints, &res);
    
    int fail = connect(sockfd, res->ai_addr, res->ai_addrlen);
    assert(fail == 0 && "Unable to connect to elevator server");
    
    freeaddrinfo(res);
    
    send(sockfd, (char[4]){0}, 4, 0);
}




/**
 * @brief Send motor direction command to the simulator.
 * @param dirn Direction (DIRN_UP, DIRN_DOWN or DIRN_STOP).
 */
void elevio_motorDirection(MotorDirection dirn){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){1, dirn}, 4, 0);
    pthread_mutex_unlock(&sockmtx);
}


/**
 * @brief Set or clear a button lamp in the simulator.
 * @param floor Floor index (0 .. N_FLOORS-1).
 * @param button Button type.
 * @param value Non-zero to turn lamp on, zero to turn it off.
 */
void elevio_buttonLamp(int floor, ButtonType button, int value){
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(button >= 0);
    assert(button < N_BUTTONS);

    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){2, button, floor, value}, 4, 0);
    pthread_mutex_unlock(&sockmtx);
}


/**
 * @brief Update the floor indicator shown in the cab.
 * @param floor Floor index (0 .. N_FLOORS-1).
 */
void elevio_floorIndicator(int floor){
    assert(floor >= 0);
    assert(floor < N_FLOORS);

    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){3, floor}, 4, 0);
    pthread_mutex_unlock(&sockmtx);
}


/**
 * @brief Control the door-open lamp.
 * @param value Non-zero to turn the lamp on, zero to turn it off.
 */
void elevio_doorOpenLamp(int value){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){4, value}, 4, 0);
    pthread_mutex_unlock(&sockmtx);
}


/**
 * @brief Control the stop lamp.
 * @param value Non-zero to turn the lamp on, zero to turn it off.
 */
void elevio_stopLamp(int value){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){5, value}, 4, 0);
    pthread_mutex_unlock(&sockmtx);
}




/**
 * @brief Query whether a call button is pressed.
 * @param floor Floor index.
 * @param button Button type.
 * @return Non-zero if pressed, otherwise zero.
 */
int elevio_callButton(int floor, ButtonType button){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){6, button, floor}, 4, 0);
    char buf[4];
    recv(sockfd, buf, 4, 0);
    pthread_mutex_unlock(&sockmtx);
    return buf[1];
}


/**
 * @brief Read current floor sensor value from simulator.
 * @return Floor index if at floor, otherwise -1.
 */
int elevio_floorSensor(void){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){7}, 4, 0);
    char buf[4];
    recv(sockfd, buf, 4, 0);
    pthread_mutex_unlock(&sockmtx);
    return buf[1] ? buf[2] : -1;
}


/**
 * @brief Query the stop button state.
 * @return Non-zero if pressed, otherwise zero.
 */
int elevio_stopButton(void){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){8}, 4, 0);
    char buf[4];
    recv(sockfd, buf, 4, 0);
    pthread_mutex_unlock(&sockmtx);
    return buf[1];
}


/**
 * @brief Query the obstruction sensor state.
 * @return Non-zero if obstruction is active, otherwise zero.
 */
int elevio_obstruction(void){
    pthread_mutex_lock(&sockmtx);
    send(sockfd, (char[4]){9}, 4, 0);
    char buf[4];
    recv(sockfd, buf, 4, 0);
    pthread_mutex_unlock(&sockmtx);
    return buf[1];
}
