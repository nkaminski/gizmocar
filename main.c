#include "serio.h"
#include "crc16.h"
#include "packet.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

void packet_crc(packet_t *p){
    p->cksum = htons(compute_crc((char *)p,sizeof(packet_t)-sizeof(uint16_t)));
    }

int main(int argc, char ** argv){
        char msg[RECVBUF];
        if(argc != 2){
            printf("Usage: ./serialctl <serial port>\n");
            return 3;
            }
        short loop=1;
        connection_t c;
        packet_t ctl;
        if(serio_init(&c, argv[1]))
            return 2;
        while(loop){
          if(serio_recv(&c, msg) < 0){
            printf("Error reading data!\n");
            return 2;
            }
        //update data here
	ctl.power=10;
	ctl.steer=10;
	packet_crc(&ctl); 
        if(serio_send(&c, &ctl, sizeof(packet_t)) < 0){
            printf("Unable to send data!\n");
            return 2;
            }
        printf("power: %i, steering: %i, CRC: %i, Resp: %s\n", ctl.power, ctl.steer, ctl.cksum, msg); 
        }
        return 0;
}
