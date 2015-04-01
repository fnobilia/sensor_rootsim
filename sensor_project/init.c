/**
*
* @author Nazzareno Marziale, Francesco Nobilia
* @date 01-04-2015
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static void find_me(sensor_t *sensor, FILE *f) {
	char *source;
	char *token;
	char line[LINE_LENGTH];

	while(fgets(line, LINE_LENGTH, f) != NULL) {
		
		// Skip empty lines
		if(line[0] == '\0') // The only \n was overwritten with a \0 // FIXME: what if there are leftover spaces?
			continue;
		// Skip comments
		if(line[0] == '#')
			continue;	

		line[strlen(line) - 1] = '\0'; // Remove trailing '\n'
		
		source = line;	
		token = strtok(source, ", \t");
			
		if(token == NULL) {
			fprintf(stderr, "Error: entry: %s\n", line);
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		if(parseInt(token) == sensor->me){
			token = strtok(NULL, ", \t");
			sensor->coord_X = parseDouble(token);
			token = strtok(NULL, ", \t");
			sensor->coord_Y = parseDouble(token);
			break;
		}

		if(source == line) {
			source = NULL; // To continue tokenizing the string
		}

		bzero(line, LINE_LENGTH);
	}
}

static void list_neighbourd(sensor_t *sensor, FILE *f){
	char *source;
	char *token;
	char line[LINE_LENGTH];
	double coord_X, coord_Y;
	unsigned int pid_sensor;
	neighbourd_t *first=NULL;
	neighbourd_t *next=NULL;


	fprintf(stderr, "Io sono: %d position coord_X: %lf & coord_Y: %lf\n", sensor->me,sensor->coord_X,sensor->coord_Y);

	int counter_process = 0;
	while(fgets(line, LINE_LENGTH, f) != NULL) {
		// Skip empty lines
		if(line[0] == '\0') // The only \n was overwritten with a \0 // FIXME: what if there are leftover spaces?
			continue;
		// Skip comments
		if(line[0] == '#')
			continue;	

		line[strlen(line) - 1] = '\0'; // Remove trailing '\n'
		source = line;	
		token = strtok(source, ", \t");
			
		if(token == NULL) {
			fprintf(stderr, "Error: entry: %s\n", line);
			fflush(stderr);
			exit(EXIT_FAILURE);
		}

		pid_sensor = parseInt(token);
		token = strtok(NULL, ", \t");
		coord_X = parseDouble(token);
		token = strtok(NULL, ", \t");
		coord_Y = parseDouble(token);

		if(pid_sensor != sensor->me && sqrt( pow((sensor->coord_X-coord_X),2) + pow((sensor->coord_Y-coord_Y),2) ) < RADIUS_ANTENNA ){		
			if (DEBUG_LIST_NEIGH)
				fprintf(stderr, "neighbour of: %d -- pid = %d\n", sensor->me,pid_sensor);

			first = (neighbourd_t *)malloc(sizeof(neighbourd_t));
      		first->sensor = pid_sensor;
			first->next = next;
      		next = first;
		}

		if(source == line) {
			source = NULL; // To continue tokenizing the string
		}
		bzero(line, LINE_LENGTH);
		counter_process++;

	}
	sensor->first = next;

	if (DEBUG_LIST_NEIGH){
		first = next;
		int count = 0;
		while(first){
			count++;
			first = first->next ;
		}
		fprintf(stderr, "Number of neighbour = %d\n",count);
	}
}

void init_my_state(unsigned int me, sensor_t *sensor){
	int i;
	char line[LINE_LENGTH];

	// Open the file
	FILE *f = fopen(FILENAME, "r");
	if(f == NULL) {
		perror(FILENAME);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	//Understand where I am inside the file
	fseek(f, 0L, SEEK_SET);
	find_me(sensor, f);

	// Create the list of neighbour 
	fseek(f, 0L, SEEK_SET);
	list_neighbourd(sensor, f);

	fclose(f);
}
