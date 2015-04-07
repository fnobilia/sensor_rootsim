/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <ROOT-Sim.h>

#include "application.h"
#include "init.h"

/**
* Function to print the current situation of local sequence number
*/
static void print_array(int *array,int me){
	int i=0;
	printf("SENSOR[%d] \t [ ",me);
	for(i=0;i<NUMBER_PROCESS;i++){
		printf("%d ",array[i]);
	}
	printf("]\n");
}


void ProcessEvent(unsigned int me, simtime_t now, unsigned int event, event_t *content, unsigned int size, sensor_t *sensor) {
	event_t new_event;
	simtime_t timestamp;

	switch(event) {
		
		// Initialize process
		case INIT: {
			
			//Init data-structure of each sensor
			sensor =  (sensor_t *)malloc(sizeof(sensor_t));
			sensor->me = me;
	 		sensor->first = NULL;
	 		sensor->array_sn = (int *)malloc(NUMBER_PROCESS * sizeof(int));
	 		bzero(sensor->array_sn,NUMBER_PROCESS * sizeof(int));
	 		
	 		SetState(sensor);

			// Parsing topology file
	 		init_my_state(me, sensor);

			if(me != MASTER){
				// If I am not the master, I will schedule the first event
				timestamp = (simtime_t)(20 * Random());
				ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
			}
			
			break;
		}

		// Send data
		case PACKET: {

			// Updating master information
			if(me == MASTER){
				// If the packet is new, update the value of local sequence number
				if(sensor->array_sn[content->pid_sensor] < content->sequence_number)
					sensor->array_sn[content->pid_sensor] = content->sequence_number;
				
				sensor->array_sn[me]++;

				if(content == NULL)
					printf("NULL (size %d)\n",size);
				else if(content->pid_sensor == MASTER)
					printf("NOW size %d)\n",size);

			}
			else{

				// If the packet is new
				if(sensor->array_sn[content->pid_sensor] < content->sequence_number){
					sensor->array_sn[content->pid_sensor] = content->sequence_number;

					// Send message to all neighbour
					if(sensor->first != NULL){
						neighbourd_t *neigh = sensor->first;
						
						char *str;
						char *pointer;
						int flag = 1;

						while (neigh){
							
							if(neigh->sensor != content->pid_sensor){
								// Send message
								timestamp = now + Expent(DELAY);
								ScheduleNewEvent(neigh->sensor, timestamp, PACKET, content, sizeof(content));
							}

							// Next neighbour
							neigh = neigh->next;
						}
					}
				}

				// Send me the waiting event to continue the simulation
				timestamp = now + Expent(DELAY);
				ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
			}

			break;
		}

		// Busy-waiting, state reachable from all sensor exception for MASTER
		case WAITING: {

			// Generate message randomly
			if((Random() > 0.5)){

				char *str;
				char *pointer;

				if(sensor->first != NULL){
					neighbourd_t *neigh = sensor->first;
					
					// Update local information
					sensor->array_sn[me]++;
					new_event.sequence_number = sensor->array_sn[me];
					new_event.pid_sensor = me;

					if(me == MASTER){
						printf("Master sends message to master! \n");
					}

					// Send message to each neighbour
					while(neigh) {							

						// Send message
						timestamp = now + Expent(DELAY);
						ScheduleNewEvent(neigh->sensor, timestamp, PACKET, &new_event, sizeof(new_event));

						// Select next sensor
						neigh = neigh->next;
					}
				}

			}
			else{
				// Without message, the sensor continues the busy-wait
				timestamp = now + Expent(DELAY);
				ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
			}

			break;
		}
		
	}
}


bool OnGVT(unsigned int me, sensor_t *snapshot) {

	// Exit if the sensor sent at least PACKETS message 
	if (snapshot->array_sn[me] < PACKETS){
		return false;
	}

	printf("SENSOR[%d] *** STOP *** \n",me );
	printf("\n");

	return true;
}
