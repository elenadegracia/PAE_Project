/*
 * motor.c
 *
 *  Created on: 26 Apr 2021
 *      Author: joan Badia
 */

#include "msp.h"
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include"motor.h"
#include"uart.h"
#include "lib_PAE.h"



#define RIGHT_WHEEL 2
#define LEFT_WHEEL 1
#define MAX_SPEED 1023//0x3FF

#define LEFT 0
#define RIGHT 1
#define CW_ADRESS 0x06
#define CCW_ADRESS 0x08
#define DIRECTION_ADRESS 0x21


void move(byte ID, bool rotation, unsigned int speed)
{
    struct RxReturn returnPacket;
    byte speed_H, speed_L;
    speed_L = speed;
    byte bParameters[16];
    if (speed < MAX_SPEED) // Max Speed
    {
        // Move right if not left
        byte speed_H = ((rotation << 2) & 0x04) | ((speed >> 8) & 0x03);

        byte bInstruction = 0X03;
        byte bParameterLength = 3;


        // Comença direccio 32
        bParameters[0] = 0X20;


        bParameters[1] = speed_L; // Write speed
        bParameters[2] = speed_H; // Write direction

        TxPacket(ID, bParameterLength, bInstruction, bParameters);
        returnPacket = RxPacket();

    }
}
void forward(unsigned int speed)
{
    //Move forward mateixa velovitat en les dues rodes
    if (speed < MAX_SPEED)
    {
        move(RIGHT_WHEEL, RIGHT, speed);
        move(LEFT_WHEEL, LEFT, speed);
    }
}
//no es fa servir
void backward(unsigned int speed)
{
    // Move backward mateixa velocitat en les dues rodes
    if (speed < MAX_SPEED)
    {
        move(RIGHT_WHEEL, LEFT, speed);
        move(LEFT_WHEEL, RIGHT, speed);
    }
}
//no es fa servir
void stop(void)
{
    // Velocitat a 0
    move(RIGHT_WHEEL, 0, 0);
    move(LEFT_WHEEL, 0, 0);
}
void turnLeft(unsigned int speed)
{
    // Pivot left
    if (speed < MAX_SPEED)
    {
        // Left wheel velocitat a 0
        move(RIGHT_WHEEL, RIGHT, speed);
        move(LEFT_WHEEL, LEFT, 0);
    }
}
void turnRight(unsigned int speed)
{
   // Pivot Right
    if(speed < MAX_SPEED){
        // Right wheel velocitat a 0
        move(RIGHT_WHEEL, RIGHT, 0);
        move(LEFT_WHEEL, LEFT, speed);
    }
}

void turnSlightlyRight(unsigned int speed)
{
    int speed2 = speed * 0.95;
    if(speed < MAX_SPEED){
        move(RIGHT_WHEEL, RIGHT, speed2);
        move(LEFT_WHEEL, LEFT, 800);
    }
}
void turnSlightlyLeft(unsigned int speed)
{
    int speed2 = speed * 0.95;
   // Pivot Right
    if(speed < MAX_SPEED){
        // Right wheel velocitat a 0.9 times speed
        move(RIGHT_WHEEL, RIGHT, speed);
        move(LEFT_WHEEL, LEFT, speed2);
    }
}




