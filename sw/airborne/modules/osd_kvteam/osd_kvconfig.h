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


#ifndef OSDKV_CONFIG_H
#define OSDKV_CONFIG_H

#define DECIMAL '.'   // Decimal point character, change to what suits you best (.) (,)

//=======================================================================================
#define LINE      30
#define LINE01    0
#define LINE02    30
#define LINE03    60
#define LINE04    90
#define LINE05    120
#define LINE06    150
#define LINE07    180
#define LINE08    210
#define LINE09    240
#define LINE10    270
#define LINE11    300
#define LINE12    330
#define LINE13    360
#define LINE14    390
#define LINE15    420
#define LINE16    450

//=====================================FONT ADDRESS DEFINE HERE==========================
//----------------Satellite Graphics--------------
#define SYM_SAT_L 0X1E
#define SYM_SAT_R 0X1F
//#define SYM_SAT 0X0F  // Not used
//----Degrees Icon for HEADING/DIRECTION HOME-----
#define SYM_DEGREES 0XBD

//-------------------Direction arrows-------------
#define SYM_ARROW_SOUTH 0X60
#define SYM_ARROW_2 0X61
#define SYM_ARROW_3 0X62
#define SYM_ARROW_4 0X63
#define SYM_ARROW_EAST 0X64
#define SYM_ARROW_6 0X65
#define SYM_ARROW_7 0X66
#define SYM_ARROW_8 0X67
#define SYM_ARROW_NORTH 0X68
#define SYM_ARROW_10 0X69
#define SYM_ARROW_11 0X6A
#define SYM_ARROW_12 0X6B
#define SYM_ARROW_WEST 0X6C
#define SYM_ARROW_14 0X6D
#define SYM_ARROW_15 0X6E
#define SYM_ARROW_16 0X6F

//-------Heading Graphics-------------------------
#define SYM_HEADING_N 0X18
#define SYM_HEADING_S 0X19
#define SYM_HEADING_E 0X1A
#define SYM_HEADING_W 0X1B
#define SYM_HEADING_DIVIDED_LINE 0X1C
#define SYM_HEADING_LINE 0X1D

//-------GPS Coordinates and Altitude-------------
#define SYM_LAT 0xCA
#define SYM_LON 0XCB
#define SYM_ALT 0XCC

//-------GPS Mode and Autopilot-------------------
#define SYM_3DFIX  0XDF
#define SYM_HOLD   0XEF
#define SYM_G_HOME 0XFE

//-------Sensor Presence--------------------------
#define SYM_ACC   0XA0
#define SYM_MAG   0XA1
#define SYM_BAR   0XA2
#define SYM_GPS   0XA3
#define SYM_CHECK 0XBE

//-------AH Center screen Graphics----------------
#define SYM_AH_CENTER 0X01
#define SYM_AH_RIGHT  0X02
#define SYM_AH_LEFT   0X03
/*attation if use KVTEAM FONT THIS SHOULD 0x00
i moved a char to 0x10 so must change here */
#define SYM_AH_CENTER_LINE 0X10
#define SYM_AH_DECORATION_RIGHT 0XC6
#define SYM_AH_DECORATION_LEFT 0XC7

//-------AH Bars----------------------------------
#define SYM_AH_BAR9_0 0x80
#define SYM_AH_BAR9_9 0x89


//-------Climb Rate Arrows------------------------
#define SYM_POS_CLIMB 0XB0
#define SYM_POS_CLIMB1 0XB1
#define SYM_POS_CLIMB2 0XB2
#define SYM_POS_CLIMB3 0XB3
#define SYM_NEG_CLIMB3 0XB4
#define SYM_NEG_CLIMB2 0XB5
#define SYM_NEG_CLIMB1 0XB6
#define SYM_NEG_CLIMB 0XB7
#define SYM_ZERO_CLIMB 0XB8

//-------Temperature------------------------------
#define SYM_TEMP_F 0X0D
#define SYM_TEMP_C 0X0E

//-------Batt evolution---------------------------
#define SYM_BATT_FULL 0X90
#define SYM_BATT_5 0X91
#define SYM_BATT_4 0X92
#define SYM_BATT_3 0X93
#define SYM_BATT_2 0X94
#define SYM_BATT_1 0X95
#define SYM_BATT_EMPTY 0X96

//-------Batt Icons-------------------------------
#define SYM_MAIN_BATT 0X97
#define SYM_VID_BAT 0XBF

//-------Unit Icons (Metric)----------------------
#define SYM_MS 0X9F
#define SYM_KMH 0XA5
#define SYM_ALTM 0XA7
#define SYM_DISTHOME_M 0XBB

//-------Unit Icons (Imperial)--------------------
#define SYM_FTS 0X99
#define SYM_MPH 0XA6
#define SYM_ALTFT 0XA8
#define SYM_DISTHOME_FT 0XB9

//-------Voltage and amperage---------------------
#define SYM_VOLT 0XA9
#define SYM_AMP 0X9A
#define SYM_MAH 0XA4

//-------Flying Mode------------------------------
#define SYM_ACRO 0XAE
#define SYM_ACRO1 0XAF
#define SYM_STABLE 0XAC
#define SYM_STABLE1 0XAD
#define SYM_TARGET_LOCK 0XAA
#define SYM_TARGET_LOCK1 0XAB

//-------Time-------------------------------------
#define SYM_ON_M 0X9B
#define SYM_FLY_M 0X9C
#define SYM_ON_H 0X70
#define SYM_FLY_H 0X71

//-------Throttle Position (%)--------------------
#define SYM_THR 0XC8
#define SYM_THR1 0XC9

//-------RSSI-------------------------------------
#define SYM_RSSI 0XBA

//=======================================================================================
// Settings Locations
enum Setting_ {
  S_CHECK_,		// used for check
  S_RSSIMIN,
  S_RSSIMAX,
  S_RSSI_ALARM,
  S_DISPLAYRSSI,
  S_MWRSSI,
  S_PWMRSSI,
  S_DISPLAYVOLTAGE,
  S_VOLTAGEMIN,
  S_BATCELLS,
  S_DIVIDERRATIO,
  S_MAINVOLTAGE_VBAT,
  S_AMPERAGE,
  S_AMPER_HOUR,
  S_VIDVOLTAGE,
  S_VIDDIVIDERRATIO,
  S_VIDVOLTAGE_VBAT,
  S_DISPLAYTEMPERATURE,
  S_TEMPERATUREMAX,
  S_BOARDTYPE,
  S_DISPLAYGPS,
  S_COORDINATES,
  S_GPSCOORDTOP,
  S_GPSALTITUDE,
  S_ANGLETOHOME,
  S_SHOWHEADING,
  S_HEADING360,
  S_UNITSYSTEM,
  S_VIDEOSIGNALTYPE,
  S_THROTTLEPOSITION,
  S_DISPLAY_HORIZON_BR,
  S_WITHDECORATION,
  S_SHOWBATLEVELEVOLUTION,
  S_RESETSTATISTICS,
  S_ENABLEADC,
  S_USE_BOXNAMES,
  S_MODEICON,
  S_DISPLAY_CS,
  S_CS0,
  S_CS1,
  S_CS2,
  S_CS3,
  S_CS4,
  S_CS5,
  S_CS6,
  S_CS7,
  S_CS8,
  S_CS9,
  // EEPROM_SETTINGS must be last!
  EEPROM_SETTINGS
};

enum Positions {
  GPS_numSatPosition,
  GPS_numSatPositionTop,
  GPS_directionToHomePosition,
  GPS_distanceToHomePosition,
  speedPosition,
  GPS_angleToHomePosition,
  MwGPSAltPosition,
  sensorPosition,
  MwHeadingPosition,
  MwHeadingGraphPosition,
  MwAltitudePosition,
  MwClimbRatePosition,
  CurrentThrottlePosition,
  flyTimePosition,
  onTimePosition,
  motorArmedPosition,
  MwGPSLatPosition,
  MwGPSLonPosition,
  MwGPSLatPositionTop,
  MwGPSLonPositionTop,
  rssiPosition,
  temperaturePosition,
  voltagePosition,
  vidvoltagePosition,
  amperagePosition,
  pMeterSumPosition,
  horizonPosition,
  callSignPosition
};

//=======================================================================================
#define POS_MASK        0x01FF
#define PAL_MASK        0x0003
#define PAL_SHFT             9
#define DISPLAY_MASK    0xC000
#define DISPLAY_ALWAYS  0xC000
#define DISPLAY_NEVER   0x0000
#define DISPLAY_COND    0x4000
#define DISPLAY_CONDR   0x8000
#define POS(pos, pal_off, disp)  (((pos)&POS_MASK)|((pal_off)<<PAL_SHFT)|(disp))

#endif
