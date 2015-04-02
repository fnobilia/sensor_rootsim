/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <ROOT-Sim.h>

#include "application.h"
#include "init.h"


static void print_array(int *array,int me){
	int i=0;
	printf("[ARRAY PROCESS %d]",me);
	for(i=0;i<11;i++){
		printf("%d | ",array[i]);
	}
	printf("\n");
}

void ProcessEvent(unsigned int me, simtime_t now, unsigned int event, event_t *content, unsigned int size, sensor_t *sensor) {
	event_t new_event;
	simtime_t timestamp;

	switch(event) {
		
		//Inizializza il processo
		case INIT: {// must be ALWAYS implemented
			if(me==0)
				if (DEBUG_INIT)
					printf("Start init \n");
			
			sensor =  (sensor_t *)malloc(sizeof(sensor_t));
			sensor->me = me;
	 		sensor->time_send = 0;
	 		sensor->packet_count = 0;
	 		sensor->first = NULL;
	 		sensor->process = (int *)malloc(NUMBER_PROCESS * sizeof(int));
	 		bzero(sensor->process,NUMBER_PROCESS * sizeof(int));
	 		
	 		SetState(sensor);

	 		init_my_state(me, sensor);

			// SetState(state); Serve per mappare l'indirizzo di memoria e associarlo al processo
			timestamp = (simtime_t)(20 * Random());
			if(me!=0)
				ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
			
			if(me==0)
				if (DEBUG_INIT)
					printf("Finisch init \n");
			
			break;
		}

		case PACKET: {
			

			if(me==0){
				if(sensor->process[content->pid_sensor] < content->sequential_number)
					sensor->process[content->pid_sensor] = content->sequential_number;
				sensor->process[me]++;
				printf("[RESULT] Arrive MEX to principal sensor from \n");
				print_array(sensor->process,me);

			}

			else{

				//printf("[PROCESS %d] Before check sequential_number\n",me);

				if(sensor->process[content->pid_sensor] < content->sequential_number){
					sensor->process[content->pid_sensor] = content->sequential_number;

					if(sensor->first != NULL){
						neighbourd_t *neigh = sensor->first;
						
						while (neigh){
							if (DEBUG_PACKET)
								printf("[SENSOR = %d] Receive from: %d and neight->sensor: %d\n",me,content->pid_sensor,neigh->sensor);
							
							if(neigh->sensor!=content->pid_sensor){
								sensor->packet_count++;
								timestamp = now + Expent(DELAY);
								
								if (DEBUG_PACKET)
									printf("the sensor %d send mex[%d] to %d\n",me,sensor->process[content->pid_sensor],neigh->sensor);
								


								ScheduleNewEvent(neigh->sensor, timestamp, PACKET, content, sizeof(content));
								
								ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);

							}
							neigh = neigh->next;
						}
						print_array(sensor->process,me);
					}
				}
			}

			break;
		}

		case WAITING: {
			if(Random() > 0.5){

				if (DEBUG)
					printf("[DEBUG %d] Send MEX\n",me);

				if(sensor->first != NULL){
					neighbourd_t *neigh = sensor->first;
					
					sensor->process[me]++;
					new_event.sequential_number = sensor->process[me];
					new_event.pid_sensor = me;

					while(neigh) {							
						
						sensor->packet_count++;
						timestamp = now + Expent(DELAY);
			
						if (DEBUG_PACKET)
							printf("[WAITING] The sensor %d send mex[%d] to %d\n",me,new_event.sequential_number,neigh->sensor);
								
						ScheduleNewEvent(neigh->sensor, timestamp, PACKET, &new_event, sizeof(new_event));

						neigh = neigh->next;
					}

					print_array(sensor->process,me);
				}

			}
			else{
				timestamp = now + Expent(DELAY);
				ScheduleNewEvent(me, timestamp, WAITING, NULL, 0);
			}
			break;
		}
		
	}
}


bool OnGVT(unsigned int me, sensor_t *snapshot) {
	
	if(DEBUG_FINISH){
		printf("[CHECK FINISH] SENSOR = %d PACKETS[%d] \n",me,snapshot->process[me]);
		print_array(snapshot->process,me);
	}

	if (snapshot->process[me] < PACKETS){
		return false;
	}
	printf("[CHECK FINISH] SENSOR = %d STOP \n",me );
	return true;
}
