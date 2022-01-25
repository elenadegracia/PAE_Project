/*
 * sensor.c
 *
 */

#include "msp.h"
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include"uart.h"
#include "lib_PAE.h"

// Escollim quin dels tres sensors volem llegir
int readSensor(byte ID, byte sensor)
{

    struct RxReturn returnPacket;

    byte bInstruction = 0x02;
    byte bParameterLength = 2;
    byte bParameters[16];
    bParameters[0] = sensor;
    bParameters[1] = 1;

    TxPacket(ID, bParameterLength, bInstruction, bParameters);
    returnPacket = RxPacket();

    return returnPacket.StatusPacket[5];
}


uint8_t front_s() {
    return readSensor(3, 0X1B);
}

uint8_t left_s() {
    return readSensor(3, 0x1A);
}

uint8_t right_s() {
    return readSensor(3, 0x1C);
}
