/*
Name: globals.h
Created: 1/10/2016 1:09:10 PM
Author:	Aftab
*/

#ifndef _GLOBALS_h
#define _GLOBALS_h

#define CONTROLLER_NAME				"Arduino Helicopter Controller"
#define FIRMWARE_VERSION			1.03

#define MAX_NUM_CHANNELS			2

#define INT_LINE_SIZE_MAX			255
#define INT_PARAMETER_COUNT_MAX		3
#define INT_PARAMETER_LENGTH_MAX	20
#define INT_DESCRIPTION_MAX			60

// TODO: These need to be changed once hardware is finalized

#define ENCODER_CHA_PIN				4
#define ENCODER_CHB_PIN				5
// Can't use pin 13 (on-board LED) since it's tied to SPI SCK
// Source: http://playground.arduino.cc/Code/Spi
#define HEARTBEAT_LED_PIN			6

#define FREQUENCY_OUTPUT_PIN		7
#define FREQUENCY_DIRECTION_PIN		8

#define ADC_CHIP_SELECT_PIN			9
#define DAC_CHIP_SELECT_PIN			10

extern char gParameters[INT_PARAMETER_COUNT_MAX][INT_PARAMETER_LENGTH_MAX + 1];

#endif
