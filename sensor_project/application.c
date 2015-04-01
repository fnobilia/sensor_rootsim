/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <ROOT-Sim.h>

#include "application.h"
#include "init.h"

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
	 		SetState(sensor);

	 		init_my_state(me, sensor);

			// SetState(state); Serve per mappare l'indirizzo di memoria e associarlo al processo
			timestamp = (simtime_t)(20 * Random());
			ScheduleNewEvent(me, timestamp, PACKET, NULL, 0);
			
			if(me==0)
				if (DEBUG_INIT)
					printf("Finisch init \n");
			
			break;
		}

		case PACKET: {
			//printf("Receive mex: %d\n",me);
			
			//printf("Numember of packet: %d\n",sensor->packet_count);
			
			if(me==0){
				printf("[RESULT] Arrive MEX to principal sensor from \n");
			}

			else{
				
				if(sensor->first != NULL){
					neighbourd_t *neigh = sensor->first;
					
					while(neigh) {
						if (DEBUG_PACKET)
							printf("[SENSOR = %d] Receive from: %d and neight->sensor: %d\n",me,content->pid_sensor,neigh->sensor);
						
						if(neigh->sensor!=content->pid_sensor){
							sensor->packet_count++;
							new_event.sent_at = now;
							new_event.pid_sensor = me;
							timestamp = now + Expent(DELAY);
							
							if (DEBUG_PACKET)
								printf("the sensor %d send mex to %d\n",me,neigh->sensor);
							
							ScheduleNewEvent(neigh->sensor, timestamp, PACKET, &new_event, sizeof(new_event));
							neigh = neigh->next ;
						}
						else 
							break;
					}	
				}
			}
		}
	}
}


bool OnGVT(unsigned int me, sensor_t *snapshot) {
	
	
	//printf("Position of sensor( me = %d ): X->%lf | Y-> %lf\n",me,snapshot->coord_X,snapshot->coord_Y);
	

	if (snapshot->packet_count < PACKETS)
		return false;
	return true;
}
