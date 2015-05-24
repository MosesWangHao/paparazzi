/*
 * Copyright (C) 2015 Moses
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "std.h"
#ifndef OSDKV_SCREEN_H
#define OSDKV_SCREEN_H

/* temperature unit c                      */
void displayTemperature(int16_t temperature);
/* Disaplay Mode ACC BAR MAG GPS STABLEMODE */
void displayMode(void);
/* 0 no display 1 unarmed 2 armed */
void displayArmed(uint16_t status);
/* not sure */
void displayCallsign(void);
/* roll = rollangle * 0.1d; pitch = pitchangle*0.1d*/
void displayHorizon(int rollAngle, int pitchAngle);
/* voltage unit 0.1V*/
void displayVoltage(int voltage);
/* throttlepercent unit percent */
void displayCurrentThrottle(uint16_t throttlepercent);
/* flyTime unit s */
void displayTime(int flyTime);
/* amperage unit 0.1A */
void displayAmperage(int amperage);
/* pMeterSum unit mAh */
void displaypMeterSum(int pMeterSum);
/* rssi unit percent% */
void displayRSSI(int rssi);
/* MwHeading unit degree */
void displayHeading(int MwHeading);
/* MwHeading unit degree */
void displayHeadingGraph(int MwHeading);
/* welcome info */
void displayIntro(void);
/* latitude = GPS_latitude*0.0000001
   longitude = GPS_longitude*0.0000001
	 GPS_altitude  unit m    */
void displayGPSPosition(int32_t GPS_latitude, int32_t GPS_longitude, int32_t GPS_altitude);
/* GPS_numSat nmber of captured sat */
void displayNumberOfSat(int GPS_numSat);
/* GPS_speed unit cm/s  but display as km/h */
void displayGPS_speed(int GPS_speed);
/* MwAltitude unit m */
void displayAltitude(int MwAltitude);
/* climb rate unit 0.01m/s */
void displayClimbRate(int MwVario);
/* distatnce to Home unit m */
void displayDistanceToHome(int GPS_distanceToHome);
/* direction to home GPS_directionToHome unit degree, GPS_distanceToHome unit m */
void displayAngleToHome(int GPS_directionToHome, int GPS_distanceToHome);
/* direction to home GPS_directionToHome&MwHeading unit degree, GPS_distanceToHome unit m */
void displayDirectionToHome(int GPS_directionToHome, int GPS_distanceToHome, int MwHeading);



char *ItoaPadded(int val, char *str, uint8_t bytes, uint8_t decimalpos);
char *FormatGPSCoord(int32_t val, char *str, uint8_t p, char pos, char neg);
char *formatTime(uint16_t val, char *str, uint8_t hhmmss);
uint8_t FindNull(void); 




#endif
