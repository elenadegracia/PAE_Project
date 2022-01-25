/*
 * sensor.h
 *
 *  Created on: 26 Apr 2021
 *      Author: joan Badia
 */

#ifndef SENSOR_H_
#define SENSOR_H_

int readSensor(byte ID, byte sensor);

uint8_t front_s();
uint8_t left_s();
uint8_t right_s();

#endif /* SENSOR_H_ */
