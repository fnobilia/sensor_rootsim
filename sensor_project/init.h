/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#pragma once
#define _TRAFFIC_INIT_H

// Input definitions
#define FILENAME	"topology_2.txt"
//#define FILENAME	"topology_small.txt"
#define LINE_LENGTH	128

// In a segment line, which one is the length one? (first element is 1!!!!)
#define LENGTH_ARG	3

// Radius of antenna
#define RADIUS_ANTENNA 6.0

// Number of process
#define NUMBER_PROCESS 11
//#define NUMBER_PROCESS 4

// Set TRUE if debug
#define DEBUG true
#define DEBUG_INIT false
#define DEBUG_PACKET true
#define DEBUG_LIST_NEIGH false
#define DEBUG_FINISH false
#define DEBUG_WAITING false
#define DEBUG_ARRAY false
#define DEBUG_LABLE false
#define DEBUG_POSITION false

// Central Node
#define MASTER 0


#include "init.c"
extern void init_my_state(unsigned int me, sensor_t *sensor);
