#pragma once
#include <stdint.h>
#define SFRAME 0x5b
#define EFRAME 0x5d
enum comm_state {
  COMM_WAIT,
  COMM_RECV,
  COMM_COMPLETE,
  COMM_VALID,
  COMM_INVALID };
typedef struct{
uint8_t steer;
uint8_t power;
uint16_t cksum;
} packet_t;
