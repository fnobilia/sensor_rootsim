/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#pragma once
#define _TRAFFIC_INIT_H

// Input definitions
#define FILENAME	"topology.txt"
#define LINE_LENGTH	128

// In a segment line, which one is the length one? (first element is 1!!!!)
#define LENGTH_ARG	3

// Radius of antenna
#define RADIUS_ANTENNA 6.0

// Number of process
#define NUMBER_PROCESS 10

// Set TRUE if debug
#define DEBUG true
#define DEBUG_INIT false
#define DEBUG_PACKET false
#define DEBUG_LIST_NEIGH false

#include "init.c"
extern void init_my_state(unsigned int me, sensor_t *sensor);
