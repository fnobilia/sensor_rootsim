/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <ROOT-Sim.h>

#define PACKET 1 // Event definition
#define WAITING 2 // Event definition
#define DELAY 120
#define PACKETS 3 // Termination condition

typedef struct neighbourd_t
{
	int sensor;
	struct neighbourd_t *next;

}neighbourd_t;

typedef struct sensor_t{
	unsigned int me;
	int time_send;
	int packet_count;
	double coord_X;
	double coord_Y;
	neighbourd_t *first;
	int *process;

}sensor_t;

typedef struct event_content_t {
	simtime_t sent_at;
	int sequential_number;
	int pid_sensor;
} event_t;


