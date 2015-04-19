 //    serialctl
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include "packet.h"
#include "hw.h"
#include "zserio.h"
#include <Servo.h>
#include "globals.h"
Servo steer, esc;
packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif

void setup() {
  #ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT 
  wdt_reset();             //watchdog timer reset 
  #endif
  //Initialize safe to safe values!!
  safe.power = 127;
  safe.steer = 127;
  safe.cksum = 0b1000000010001011;
  SerComm.begin(57600);
  comm_init();
  pinMode(13,OUTPUT);
  steer.attach(5);
  esc.attach(6);
  steer.write(90);
  esc.writeMicroseconds(1500);
  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));
}
void loop(){
  //Every line sent to the computer gets us a new state
  wdt_reset();
  print_data();
  comm_parse();
  esc.writeMicroseconds(map(astate->power,0,255,1400,1600));
  steer.write(map(astate->steer,0,255,50,130));
  //limits data rate
  delay(50);
}
