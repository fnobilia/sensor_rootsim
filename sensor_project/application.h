/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <ROOT-Sim.h>

#define PACKET 1 	// Event definition
#define WAITING 2 	// Event definition
#define DELAY 120
#define PACKETS 3 	// Termination condition

typedef struct neighbourd_t{
	int sensor;						// Process ID
	struct neighbourd_t *next;		

}neighbourd_t;

typedef struct sensor_t{
	unsigned int me;				// Process ID
	double coord_X;
	double coord_Y;
	neighbourd_t *first;			// Head of neighbours list
	int *array_sn;					// Array to store the local sequence number of each sensor

}sensor_t;

typedef struct event_content_t {
	simtime_t sent_at;				// Timestamp of simulation
	int sequence_number;			
	int pid_sensor;					// ID of sender
} event_t;


