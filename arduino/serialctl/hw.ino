#include "hw.h"
#include "packet.h"
#include "globals.h"
void print_data(){
     SerComm.print("terrain cf=");
     SerComm.println(cf);
}
