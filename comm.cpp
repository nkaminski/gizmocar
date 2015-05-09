#include "comm.hpp"
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serio.h"
#include "crc16.h"
#include "packet.h"
#include <string.h>
#include <arpa/inet.h>

pthread_t comm_th;
pthread_mutex_t lck;

int rv, comm_run;
uint8_t s_power,s_steer;

void packet_crc(packet_t *p){
    p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
    }

void* comm_thread(void * serial_port){
        char msg[RECVBUF];
        connection_t c;
        packet_t ctl;
		//TODO error handling
        serio_init(&c, (char *)serial_port);
        while(comm_run){
          if(serio_recv(&c, msg) < 0){
            printf("Error reading data!\n");
            }
        //update data here
	pthread_mutex_lock( &lck );
	ctl.power=s_power;
	ctl.steer=s_steer;
	pthread_mutex_unlock( &lck );
	packet_crc(&ctl); 
        if(serio_send(&c, &ctl, sizeof(packet_t)) < 0){
            printf("Unable to send data!\n");
            }
        printf("power: %i, steering: %i, CRC: %i, Resp: %s\n", ctl.power, ctl.steer, ctl.cksum, msg); 
        }
}
int comm_start(char * serial_port){
	comm_run=1;
	s_power=127;
	s_steer=127;
	lck = PTHREAD_MUTEX_INITIALIZER;
	if((rv=pthread_create( &comm_th, NULL, comm_thread, (void *)serial_port)))
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",rv);
	         exit(EXIT_FAILURE);
	     }
}	
void comm_stop(){
	comm_run=0;
	pthread_join(comm_th, NULL);
}
void comm_check(){
  if(!pthread_tryjoin_np(comm_th,NULL)){
    printf("FATAL: Comms failed, exiting!\n");
    exit(4);
  }
}

}
