#include "crc16.h"
// C implemtation for computer
uint16_t _crc16_update(uint16_t crc, uint8_t a){
        int i;
        crc ^= a;
        for (i = 0; i < 8; ++i)
        {
                if (crc & 1)
                        crc = (crc >> 1) ^ 0xA001;
                else
                        crc = (crc >> 1);
        }
        return crc;
}
uint16_t compute_crc(char *data, int len){
        uint16_t crc = 0xffff;
        int i;
        for(i=0; i<len; i++){
                crc=_crc16_update(crc,data[i]);
        }
        return crc;
}
