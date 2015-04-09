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

int broken = 0;

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
	 		sensor->final_time = Expent(TIME_END);
	 		sensor->current_time = 0;

	 		if(Random()>0.8)
	 			sensor->failure_time = Expent(TIME_FAILURE);
	 		else
	 			sensor->failure_time = sensor->final_time + 1;

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

			sensor->current_time = now;

			if(sensor->current_time < sensor->failure_time){

				// Updating master information
				if(me == MASTER){
					// If the packet is new, update the value of local sequence number
					if(sensor->array_sn[content->pid_sensor] < content->sequence_number)
						sensor->array_sn[content->pid_sensor] = content->sequence_number;
					
					sensor->array_sn[me]++;
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

			}
			else{
				if(!broken){
					printf("SENSOR[%d] ***BROKEN*** \n\n",me);
					broken = 1;
				}
			}

			break;
		}

		// Busy-waiting, state reachable from all sensor exception for MASTER
		case WAITING: {

			sensor->current_time = now;

			// Generate message randomly
			if((Random() > 0.5) && (sensor->current_time < sensor->failure_time)){

				char *str;
				char *pointer;

				if(sensor->first != NULL){
					neighbourd_t *neigh = sensor->first;
					
					// Update local information
					sensor->array_sn[me]++;
					new_event.sequence_number = sensor->array_sn[me];
					new_event.pid_sensor = me;

					// Send message to each neighbour
					while(neigh) {							

						// Send message
						timestamp = now + Expent(DELAY);
						ScheduleNewEvent(neigh->sensor, timestamp, PACKET, &new_event, sizeof(new_event));

						// Select next sensor
						neigh = neigh->next;
					}

					// Start busy-waiting
					timestamp = now + Expent(DELAY);
					ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
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
	/*if (snapshot->array_sn[me] < PACKETS){
		return false;
	}*/

	if (snapshot->current_time < snapshot->final_time){
		//printf("SENSOR[%d] Current=%lf Final=%lf \n",me,snapshot->current_time,snapshot->final_time);
		return false;
	}	

	printf("SENSOR[%d] *** STOP *** \n\n",me );

	return true;
}
