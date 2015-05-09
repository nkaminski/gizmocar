#include <stdint.h>
#include <pthread.h>
extern "C"{
extern pthread_mutex_t lck;
extern uint8_t s_power;
extern uint8_t s_steer;
int comm_start(char * serial_port);
void comm_stop();
}
