/*
 * motor.h
 *
 *  Created on: 26 Apr 2021
 *      Author: joan Badia
 */

#ifndef MOTOR_H_
#define MOTOR_H_
typedef uint8_t byte;
void forward(unsigned int speed);
void move(byte ID, bool rotation, unsigned int speed);
void backward(unsigned int speed);
void stop(void);
void mode_right(unsigned int speed);
void turnLeft(unsigned int speed);
void turnRight(unsigned int speed);
void turnRightD(unsigned int degree);
void turnSlightlyRight(unsigned int speed);
void turnSlightlyLeft(unsigned int speed);
#endif /* MOTOR_H_ */
