extern "C" {
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t comm_th;
int rv, comm_run;

void* comm_thread(void *data){
	while(comm_run){
	printf("Comm thread\n");
	usleep(1E6);
	}
}
int comm_start(){
	comm_run=1;
	if((rv=pthread_create( &comm_th, NULL, comm_thread, (void *)NULL)))
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",rv);
	         exit(EXIT_FAILURE);
	     }
}	
void comm_stop(){
	comm_run=0;
	pthread_join(comm_th, NULL);
}

}
