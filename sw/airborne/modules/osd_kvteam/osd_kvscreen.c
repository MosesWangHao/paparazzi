#include "std.h"
#include "string.h"
#include "osd_kvconfig.h"
#include "osd_kvscreen.h"

//================================ Display Settings ========================================

// POSITION OF EACH CHARACTER OR LOGO IN THE MAX7456
const char speedUnitAdd[2]       = { 0xa5,0xa6} ; // [0][0] and [0][1] = Km/h   [1][0] and [1][1] = Mph
const char temperatureUnitAdd[2] = { 0x0e,0x0d};
const char disarmed_text[]       = "DISARMED";
const char armed_text[]          = " ARMED";
const char MultiWiiLogoL1Add[17] = {
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0};
const char MultiWiiLogoL2Add[17] = {
  0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0};
const char MultiWiiLogoL3Add[17] = {
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0};
// For Heading ICON
const char headGraph[] = {
  0x1a,0x1d,0x1c,0x1d,0x19,0x1d,0x1c,0x1d,0x1b,0x1d,0x1c,0x1d,0x18,0x1d,0x1c,0x1d,0x1a,0x1d,0x1c,0x1d,0x19,0x1d,0x1c,0x1d,0x1b};
const unsigned char MwAltitudeAdd[2]         ={0xa7,0xa8};
const unsigned char MwClimbRateAdd[2]        ={0x9f,0x99};
const unsigned char GPS_distanceToHomeAdd[2] ={0xbb,0xb9};
const unsigned char MwGPSAltPositionAdd[2]   ={0xa7,0xa8};
const char KVTeamVersionPosition = 35;

//========================For Intro==================================
const char message0[]  = "KV_OSD_TEAM_R370";
const char message1[]  = "VIDEO SIGNAL NTSC";
const char message2[]  = "VIDEO SIGNAL PAL ";
const char message5[]  = "VERSION:";

	
	
void MAX7456_WriteString(const char *string, int Adresse);
void MAX7456_WriteString_P(const char *string, int Adresse);
uint16_t getPosition(uint8_t pos);
uint8_t Settings[EEPROM_SETTINGS];



extern char screen[];
extern char screenBuffer[];	


char *ItoaPadded(int val, char *str, uint8_t bytes, uint8_t decimalpos)  {
  uint8_t neg = 0;
  if(val < 0) {
    neg = 1;
    val = -val;
  }

  str[bytes] = 0;
  for(;;) {
    if(bytes == decimalpos) {
      str[--bytes] = DECIMAL;
      decimalpos = 0;
    }
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    if(bytes == 0 || (decimalpos == 0 && val == 0))
      break;
  }

  if(neg && bytes > 0)
    str[--bytes] = '-';

  while(bytes != 0)
    str[--bytes] = ' ';
  return str;
}

char *FormatGPSCoord(int32_t val, char *str, uint8_t p, char pos, char neg) {
  if(val < 0) {
    pos = neg;
    val = -val;
  }

  uint8_t bytes = p+8;

  str[bytes] = 0;
  str[--bytes] = pos;
  for(;;) {
    if(bytes == p) {
      str[--bytes] = DECIMAL;
      continue;
    }
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    if(bytes < 3 && val == 0)
       break;
   }

   while(bytes != 0)
     str[--bytes] = ' ';

   return str;
}

// Take time in Seconds and format it as 'MM:SS'
// Alternately Take time in Minutes and format it as 'HH:MM'
// If hhmmss is 1, display as HH:MM:SS
char *formatTime(uint16_t val, char *str, uint8_t hhmmss) {
  int8_t bytes = 5;
  if(hhmmss)
    bytes = 8;
  str[bytes] = 0;
  do {
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
    str[--bytes] = '0' + (val % 6);
    val = val / 6;
    str[--bytes] = ':';
  } while(hhmmss-- != 0);
  do {
    str[--bytes] = '0' + (val % 10);
    val = val / 10;
  } while(val != 0 && bytes != 0);

  while(bytes != 0)
     str[--bytes] = ' ';

  return str;
}

uint8_t FindNull(void)
{
  uint8_t xx;
  for(xx=0;screenBuffer[xx]!=0;xx++)
    ;
  return xx;
}

void displayTemperature(int16_t temperature)        // WILL WORK ONLY WITH V1.2
{
  int xxx;
  if (Settings[S_UNITSYSTEM])
    xxx = temperature*1.8+32;       //Fahrenheit conversion for imperial system.
  else
    xxx = temperature;

  //if(xxx > temperMAX)
  //  temperMAX = xxx;

  itoa(xxx,screenBuffer,10);	//sprintf(screenBuffer, "%d", xxx);
  uint8_t xx = FindNull();   // find the NULL
  screenBuffer[xx++]=temperatureUnitAdd[Settings[S_UNITSYSTEM]];
  screenBuffer[xx]=0;  // Restore the NULL
  MAX7456_WriteString(screenBuffer,getPosition(temperaturePosition));
}

void displayMode(void)
{
  //if(Settings[S_MODEICON])
		{
    // Put sensor symbold (was displaySensors)
    //screenBuffer[0] = (MwSensorPresent&ACCELEROMETER) ? SYM_ACC : ' ';
    //screenBuffer[1] = (MwSensorPresent&BAROMETER) ? SYM_BAR : ' ';
    //screenBuffer[2] = (MwSensorPresent&MAGNETOMETER) ? SYM_MAG : ' ';
    //screenBuffer[3] = (MwSensorPresent&GPSSENSOR) ? SYM_GPS : ' ';
    screenBuffer[0] = SYM_ACC;
    screenBuffer[1] = SYM_BAR;
    screenBuffer[2] = SYM_MAG;
    screenBuffer[3] = SYM_GPS;
  
    //if(MwSensorActive&mode_stable)
    {
      screenBuffer[4]=SYM_STABLE;
      screenBuffer[5]=SYM_STABLE1;
    }
    //else
    //{
    //  screenBuffer[4]=SYM_ACRO;
    //  screenBuffer[5]=SYM_ACRO1;
    //}
    screenBuffer[6]=' ';
    //if(MwSensorActive&mode_gpshome)
      screenBuffer[7]=SYM_G_HOME;
    //else if(MwSensorActive&mode_gpshold)
    //  screenBuffer[7]=SYM_HOLD;
    //else if(GPS_fix)
    //  screenBuffer[7]=SYM_3DFIX;
    //else
    //  screenBuffer[7]=' ';
  
    screenBuffer[8]=0;
    MAX7456_WriteString(screenBuffer,getPosition(sensorPosition));
  
    // Put ON indicator under sensor symbol
    //screenBuffer[0] = (MwSensorActive&mode_stable) ? SYM_CHECK : ' ';
    //screenBuffer[1] = (MwSensorActive&mode_baro) ? SYM_CHECK : ' ';
    //screenBuffer[2] = (MwSensorActive&mode_mag) ? SYM_CHECK : ' ';
    //screenBuffer[3] = (MwSensorActive&(mode_gpshome|mode_gpshold)) ? SYM_CHECK : ' ';
    //screenBuffer[0] = (MwSensorActive&mode_stable) ? SYM_CHECK : ' ';
    screenBuffer[1] = SYM_CHECK;
    screenBuffer[2] = SYM_CHECK;
    screenBuffer[3] = SYM_CHECK;
    screenBuffer[4] = 0;
    MAX7456_WriteString(screenBuffer,getPosition(sensorPosition)+LINE);
    }
}

void displayArmed(uint16_t status)
{
	if(status == 0) {
		screenBuffer[0] = 0;
    MAX7456_WriteString_P(screenBuffer, getPosition(motorArmedPosition));
	}
	else if(status == 1) {
		MAX7456_WriteString_P(disarmed_text, getPosition(motorArmedPosition));
	}
	else
		MAX7456_WriteString_P(armed_text, getPosition(motorArmedPosition));
}

void displayCallsign(void)
{
  uint16_t position = getPosition(callSignPosition);
  //if(Settings[S_DISPLAY_CS])
	{
      for(int X=0; X<10; X++) {
          screenBuffer[X] = (char)(Settings[S_CS0 + X]);
     }   
       screenBuffer[10] = 0;
       MAX7456_WriteString(screenBuffer, getPosition(callSignPosition)); 
  }
}
void displayHorizon(int rollAngle, int pitchAngle)
{
  //if(!fieldIsVisible(horizonPosition))
  //  return;

  uint16_t position = getPosition(horizonPosition);
  rollAngle =  10*rollAngle;
  pitchAngle = -10*pitchAngle;

  if(pitchAngle>200) pitchAngle=200;
  if(pitchAngle<-250) pitchAngle=-250;
  if(rollAngle>400) rollAngle=400;
  if(rollAngle<-400) rollAngle=-400;

  for(int X=0; X<=8; X++) {
    int Y = (rollAngle * (4-X)) / 64;
    Y -= pitchAngle / 8;
    Y += 41;
    if(Y >= 0 && Y <= 81) {
      uint16_t pos = position + LINE*(Y/9) + 2 - 2*LINE + X;
      screen[pos] = SYM_AH_BAR9_0+(Y%9);
      if(Y>=9 && (Y%9) == 0)
        screen[pos-LINE] = SYM_AH_BAR9_9;
    }
  }

  //Draw center screen
  screen[position+2*LINE+6-1] = SYM_AH_CENTER_LINE;
  screen[position+2*LINE+6+1] = SYM_AH_CENTER_LINE;
  screen[position+2*LINE+6] =   SYM_AH_CENTER;

  // Draw AH sides
  screen[position+2*LINE+1] =   SYM_AH_LEFT;
  screen[position+2*LINE+11] =  SYM_AH_RIGHT;
  screen[position+0*LINE] =     SYM_AH_DECORATION_LEFT;
  screen[position+1*LINE] =     SYM_AH_DECORATION_LEFT;
  screen[position+2*LINE] =     SYM_AH_DECORATION_LEFT;
  screen[position+3*LINE] =     SYM_AH_DECORATION_LEFT;
  screen[position+4*LINE] =     SYM_AH_DECORATION_LEFT;
  screen[position+0*LINE+12] =  SYM_AH_DECORATION_RIGHT;
  screen[position+1*LINE+12] =  SYM_AH_DECORATION_RIGHT;
  screen[position+2*LINE+12] =  SYM_AH_DECORATION_RIGHT;
  screen[position+3*LINE+12] =  SYM_AH_DECORATION_RIGHT;
  screen[position+4*LINE+12] = SYM_AH_DECORATION_RIGHT;
}

void displayVoltage(int voltage)
{

  ItoaPadded(voltage, screenBuffer, 4, 3);
  screenBuffer[4] = SYM_VOLT;
  screenBuffer[5] = 0;
  MAX7456_WriteString(screenBuffer,getPosition(voltagePosition));

  if (Settings[S_SHOWBATLEVELEVOLUTION]){
    // For battery evolution display
    int BATTEV1 =Settings[S_BATCELLS] * 35;
    int BATTEV2 =Settings[S_BATCELLS] * 36;
    int BATTEV3 =Settings[S_BATCELLS] * 37;
    int BATTEV4 =Settings[S_BATCELLS] * 38;
    int BATTEV5 =Settings[S_BATCELLS] * 40;
    int BATTEV6 = Settings[S_BATCELLS] * 41;

    if (voltage < BATTEV1) screenBuffer[0]=SYM_BATT_EMPTY;
    else if (voltage < BATTEV2) screenBuffer[0]=SYM_BATT_1;
    else if (voltage < BATTEV3) screenBuffer[0]=SYM_BATT_2;
    else if (voltage < BATTEV4) screenBuffer[0]=SYM_BATT_3;
    else if (voltage < BATTEV5) screenBuffer[0]=SYM_BATT_4;
    else if (voltage < BATTEV6) screenBuffer[0]=SYM_BATT_5;
    else screenBuffer[0]=SYM_BATT_FULL;                              // Max charge icon
  }
  else {
    screenBuffer[0]=SYM_MAIN_BATT;
  }
  screenBuffer[1]=0;
  MAX7456_WriteString(screenBuffer,getPosition(voltagePosition)-1);

//  if (Settings[S_VIDVOLTAGE]){
//    ItoaPadded(vidvoltage, screenBuffer, 4, 3);
//    screenBuffer[4]=SYM_VOLT;
//    screenBuffer[5]=0;
//    MAX7456_WriteString(screenBuffer,getPosition(vidvoltagePosition));
//    screenBuffer[0]=SYM_VID_BAT;
//    screenBuffer[1]=0;
//    MAX7456_WriteString(screenBuffer,getPosition(vidvoltagePosition)-1);
//  }
}

void displayCurrentThrottle(uint16_t throttlepercent)
{

//  if (MwRcData[THROTTLESTICK] > HighT) HighT = MwRcData[THROTTLESTICK] -5;
//  if (MwRcData[THROTTLESTICK] < LowT) LowT = MwRcData[THROTTLESTICK];      // Calibrate high and low throttle settings  --defaults set in GlobalVariables.h 1100-1900
  screenBuffer[0]=SYM_THR;
  screenBuffer[1]=0;
  MAX7456_WriteString(screenBuffer,getPosition(CurrentThrottlePosition));
  if(!1 /*armed*/) {
    screenBuffer[0]=' ';
    screenBuffer[1]=' ';
    screenBuffer[2]='-';
    screenBuffer[3]='-';
    screenBuffer[4]=0;
    MAX7456_WriteString(screenBuffer,getPosition(CurrentThrottlePosition)+2);
  }
  else
  {
    //int CurThrottle = map(MwRcData[THROTTLESTICK],LowT,HighT,0,100);
    ItoaPadded(throttlepercent,screenBuffer,3,0);
    screenBuffer[3]='%';
    screenBuffer[4]=0;
    MAX7456_WriteString(screenBuffer,getPosition(CurrentThrottlePosition)+2);
  }
}

void displayTime(int flyTime)
{ 
  if(flyTime < 3600) {
    screenBuffer[0] = SYM_FLY_M;
    formatTime(flyTime, screenBuffer+1, 0);
  }
  else {
    screenBuffer[0] = SYM_FLY_H;
    formatTime(flyTime/60, screenBuffer+1, 0);
  }
  MAX7456_WriteString(screenBuffer,getPosition(flyTimePosition));

//  if (armed) return ;
//  uint16_t position = getPosition(onTimePosition);
//  if(onTime < 3600) {
//    screenBuffer[0] = SYM_ON_M;
//    formatTime(onTime, screenBuffer+1, 0);
//  }
//  else {
//    screenBuffer[0] = SYM_ON_H;
//    formatTime(onTime/60, screenBuffer+1, 0);
//  }
//  MAX7456_WriteString(screenBuffer,getPosition(onTimePosition));
}

void displayAmperage(int amperage)
{
  // Real Ampere is ampere / 10
  ItoaPadded(amperage, screenBuffer, 4, 3);     // 99.9 ampere max!
  screenBuffer[4] = SYM_AMP;
  screenBuffer[5] = 0;
  MAX7456_WriteString(screenBuffer,getPosition(amperagePosition));
}

void displaypMeterSum(int pMeterSum)
{
  screenBuffer[0]=SYM_MAH;
  itoa(pMeterSum,screenBuffer+1,10);	//sprintf((char *)(screenBuffer+1),"%d", pMeterSum);
  MAX7456_WriteString(screenBuffer,getPosition(pMeterSumPosition));
}

void displayRSSI(int rssi)
{
  screenBuffer[0] = SYM_RSSI;
  // Calcul et affichage du Rssi
  itoa(rssi,screenBuffer+1,10); //sprintf((char*)(screenBuffer+1),"%d", rssi);
  uint8_t xx = FindNull();
  screenBuffer[xx++] = '%';
  screenBuffer[xx] = 0;
  MAX7456_WriteString(screenBuffer,getPosition(rssiPosition));
}

void displayHeading(int MwHeading)
{
 //if (Settings[S_SHOWHEADING]) 
	 {  
      int16_t heading = MwHeading;
      if (1 /*Settings[S_HEADING360]*/) {
        if(heading < 0)
          heading += 360;
        ItoaPadded(heading,screenBuffer,3,0);
        screenBuffer[3]=SYM_DEGREES;
        screenBuffer[4]=0;
      }
      else {
        ItoaPadded(heading,screenBuffer,4,0);
        screenBuffer[4]=SYM_DEGREES;
        screenBuffer[5]=0;
      }
      MAX7456_WriteString(screenBuffer,getPosition(MwHeadingPosition));
  }  
}

void displayHeadingGraph(int MwHeading)
{
  int xx;
  xx = MwHeading * 4;
  xx = xx + 720 + 45;
  xx = xx / 90;

  uint16_t pos = getPosition(MwHeadingGraphPosition);
  memcpy(screen+pos, headGraph+xx, 10);
}

void displayIntro(void)
{

  MAX7456_WriteString_P(message0, KVTeamVersionPosition);

  if (Settings[S_VIDEOSIGNALTYPE])
    MAX7456_WriteString_P(message2, KVTeamVersionPosition+30);
  else
    MAX7456_WriteString_P(message1, KVTeamVersionPosition+30);

  MAX7456_WriteString_P(MultiWiiLogoL1Add, KVTeamVersionPosition+120);
  MAX7456_WriteString_P(MultiWiiLogoL2Add, KVTeamVersionPosition+120+LINE);
  MAX7456_WriteString_P(MultiWiiLogoL3Add, KVTeamVersionPosition+120+LINE+LINE);

  MAX7456_WriteString_P(message5, KVTeamVersionPosition+120+LINE+LINE+LINE);
	sprintf((char*)(screenBuffer),"%d", 23/*VER*/);
  MAX7456_WriteString(screenBuffer,KVTeamVersionPosition+131+LINE+LINE+LINE);

  //MAX7456_WriteString_P(message6, KVTeamVersionPosition+120+LINE+LINE+LINE+LINE+LINE);
  //MAX7456_WriteString_P(message7, KVTeamVersionPosition+125+LINE+LINE+LINE+LINE+LINE+LINE);
  //MAX7456_WriteString_P(message8, KVTeamVersionPosition+125+LINE+LINE+LINE+LINE+LINE+LINE+LINE);
  //MAX7456_WriteString_P(message9, KVTeamVersionPosition+120+LINE+LINE+LINE+LINE+LINE+LINE+LINE+LINE);
}

void displayGPSPosition(int32_t GPS_latitude, int32_t GPS_longitude, int32_t GPS_altitude)
{
  //if(!GPS_fix)
  //  return;

  //if(Settings[S_COORDINATES])
	{
    //if(fieldIsVisible(MwGPSLatPosition)) 
		{
      screenBuffer[0] = SYM_LAT;
      FormatGPSCoord(GPS_latitude,screenBuffer+1,3,'N','S');
      //if(!Settings[S_GPSCOORDTOP])
      //  MAX7456_WriteString(screenBuffer,getPosition(MwGPSLatPosition));
      //else
        MAX7456_WriteString(screenBuffer,getPosition(MwGPSLatPositionTop));  
    }

    //if(fieldIsVisible(MwGPSLatPosition)) 
		{
      screenBuffer[0] = SYM_LON;
      FormatGPSCoord(GPS_longitude,screenBuffer+1,3,'E','W');
      //if(!Settings[S_GPSCOORDTOP])
      //  MAX7456_WriteString(screenBuffer,getPosition(MwGPSLonPosition));
      //else
        MAX7456_WriteString(screenBuffer,getPosition(MwGPSLonPositionTop));          
    }
  }
  
  //if(Settings[S_GPSALTITUDE])
		{
      screenBuffer[0] = MwGPSAltPositionAdd[Settings[S_UNITSYSTEM]];
      uint16_t xx;
      if(Settings[S_UNITSYSTEM])
        xx = GPS_altitude * 3.2808; // Mt to Feet
      else
        xx = GPS_altitude;          // Mt
      itoa(xx,screenBuffer+1,10); //sprintf((char*)(screenBuffer+1),"%d", xx);
      MAX7456_WriteString(screenBuffer,getPosition(MwGPSAltPosition));
      }
}

void displayNumberOfSat(int GPS_numSat)
{
  screenBuffer[0] = SYM_SAT_L;
  screenBuffer[1] = SYM_SAT_R;
  itoa(GPS_numSat,screenBuffer+2,10);	//sprintf((char*)(screenBuffer+2),"%d", GPS_numSat);
  if(!Settings[S_GPSCOORDTOP])
    MAX7456_WriteString(screenBuffer,getPosition(GPS_numSatPosition));
  else 
    MAX7456_WriteString(screenBuffer,getPosition(GPS_numSatPositionTop)); 
}


void displayGPS_speed(int GPS_speed)
{

  //if(!GPS_fix) return;
  //if(!armed) GPS_speed=0;

  int xx;
  if(!Settings[S_UNITSYSTEM])
    xx = GPS_speed * 0.036;           // From MWii cm/sec to Km/h
  else
    xx = GPS_speed * 0.02236932;      // (0.036*0.62137)  From MWii cm/sec to mph

  //if(xx > speedMAX)
  //  speedMAX = xx;
    
  screenBuffer[0]=speedUnitAdd[Settings[S_UNITSYSTEM]];
  itoa(xx,screenBuffer+1,10); //sprintf((char*)(screenBuffer+1),"%d", xx);
  MAX7456_WriteString(screenBuffer,getPosition(speedPosition));
}

void displayAltitude(int MwAltitude)
{
  int16_t altitude;
  if(Settings[S_UNITSYSTEM])
    altitude = MwAltitude*3.2808;    // cm to feet
  else
    altitude = MwAltitude;

  //if(armed && allSec>5 && altitude > altitudeMAX)
  //  altitudeMAX = altitude;

  screenBuffer[0]=MwAltitudeAdd[Settings[S_UNITSYSTEM]];
  itoa(altitude,screenBuffer+1,10); //sprintf((char*)(screenBuffer+1),"%d", altitude);
  MAX7456_WriteString(screenBuffer,getPosition(MwAltitudePosition));
}

void displayClimbRate(int MwVario)
{

  if(MwVario > 70)       screenBuffer[0] = SYM_POS_CLIMB3;
  else if(MwVario > 50)  screenBuffer[0] = SYM_POS_CLIMB2;
  else if(MwVario > 30)  screenBuffer[0] = SYM_POS_CLIMB1;
  else if(MwVario > 20)  screenBuffer[0] = SYM_POS_CLIMB;
  else if(MwVario < -70) screenBuffer[0] = SYM_NEG_CLIMB3;
  else if(MwVario < -50) screenBuffer[0] = SYM_NEG_CLIMB2;
  else if(MwVario < -30) screenBuffer[0] = SYM_NEG_CLIMB1;
  else if(MwVario < -20) screenBuffer[0] = SYM_NEG_CLIMB;
  else                   screenBuffer[0] = SYM_ZERO_CLIMB;

  screenBuffer[1] = MwClimbRateAdd[Settings[S_UNITSYSTEM]];
  int16_t vario;
  if(Settings[S_UNITSYSTEM])
    vario = MwVario * 0.032808;       // cm/sec ----> ft/sec
  else
    vario = MwVario / 100;            // cm/sec ----> mt/sec
	
  itoa(vario, screenBuffer+2, 10); //sprintf((char*)(screenBuffer+2),"%d", vario);

  MAX7456_WriteString(screenBuffer,getPosition(MwClimbRatePosition));
}

void displayDistanceToHome(int GPS_distanceToHome)
{
  //if(!GPS_fix)
  //  return;

  int16_t dist;
  if(Settings[S_UNITSYSTEM])
    dist = GPS_distanceToHome * 3.2808;           // mt to feet
  else
    dist = GPS_distanceToHome;                    // Mt

  //if(dist > distanceMAX)
  //  distanceMAX = dist;

  screenBuffer[0] = GPS_distanceToHomeAdd[Settings[S_UNITSYSTEM]];
  itoa(dist, screenBuffer+1, 10); //sprintf((char*)(screenBuffer+1),"%d", dist);
  MAX7456_WriteString(screenBuffer,getPosition(GPS_distanceToHomePosition));
}

void displayAngleToHome(int GPS_directionToHome, int GPS_distanceToHome)
{
  //if(!GPS_fix)
  //    return;
      
  //if(Settings[S_ANGLETOHOME])
	{
    if(GPS_distanceToHome <= 2 /*&& Blink2hz*/)
      return;
  
    ItoaPadded(GPS_directionToHome,screenBuffer,3,0);
    screenBuffer[3] = SYM_DEGREES;
    screenBuffer[4] = 0;
    MAX7456_WriteString(screenBuffer,getPosition(GPS_angleToHomePosition));
    }
}

void displayDirectionToHome(int GPS_directionToHome, int GPS_distanceToHome, int MwHeading)
{
  //if(!GPS_fix)
  //  return;
  if(GPS_distanceToHome <= 2 /*&& Blink2hz*/)
    return;
  
  int16_t d = MwHeading + 180 + 360 - GPS_directionToHome;
  d *= 4;
  d += 45;
  d = (d/90)%16;

  screenBuffer[0] = SYM_ARROW_SOUTH + d;
  screenBuffer[1] = 0;
  MAX7456_WriteString(screenBuffer,getPosition(GPS_directionToHomePosition));
}




//============================PORT FROM scree_layout.c====================================
const uint16_t screenPosition[] = {
  POS(LINE02+2,  0, DISPLAY_ALWAYS), // GPS_numSatPosition
  POS(LINE01+13,  0, DISPLAY_ALWAYS), // GPS_numSatPositionTop      // On top of screen
  POS(LINE03+14, 0, DISPLAY_ALWAYS), // GPS_directionToHomePosition
  POS(LINE02+24, 0, DISPLAY_ALWAYS), // GPS_distanceToHomePosition
  POS(LINE03+24, 0, DISPLAY_ALWAYS), // speedPosition
  POS(LINE04+12, 0, DISPLAY_ALWAYS), // GPS_angleToHomePosition
  POS(LINE04+24, 0, DISPLAY_ALWAYS), // MwGPSAltPosition
  POS(LINE03+2,  0, DISPLAY_ALWAYS), // sensorPosition
  POS(LINE02+20, 0, DISPLAY_ALWAYS), // MwHeadingPosition
  POS(LINE02+10, 0, DISPLAY_ALWAYS), // MwHeadingGraphPosition
  POS(LINE07+2,  1, DISPLAY_ALWAYS), // MwAltitudePosition
  POS(LINE07+24, 1, DISPLAY_ALWAYS), // MwClimbRatePosition
  POS(LINE12+22, 2, DISPLAY_ALWAYS), // CurrentThrottlePosition
  POS(LINE13+22, 2, DISPLAY_ALWAYS), // flyTimePosition
  POS(LINE13+22, 2, DISPLAY_ALWAYS), // onTimePosition
  POS(LINE12+11, 2, DISPLAY_ALWAYS), // motorArmedPosition
  POS(LINE10+2,  2, DISPLAY_CONDR),  // MwGPSLatPosition
  POS(LINE10+15, 2, DISPLAY_CONDR),  // MwGPSLonPosition
  POS(LINE01+1,  0, DISPLAY_CONDR),  // MwGPSLatPositionTop      // On top of screen
  POS(LINE01+17, 0, DISPLAY_CONDR),  // MwGPSLonPositionTop      // On top of screen
  POS(LINE12+2,  2, DISPLAY_ALWAYS), // rssiPosition
  POS(LINE11+2,  0, DISPLAY_ALWAYS), // temperaturePosition
  POS(LINE13+3,  2, DISPLAY_ALWAYS), // voltagePosition
  POS(LINE11+3,  2, DISPLAY_ALWAYS), // vidvoltagePosition
  POS(LINE13+10, 2, DISPLAY_ALWAYS), // amperagePosition
  POS(LINE13+16, 2, DISPLAY_ALWAYS), // pMeterSumPosition
  POS(LINE05+8,  1, DISPLAY_CONDR),  // horizonPosition
  POS(LINE12+10, 2, DISPLAY_ALWAYS), // CallSign Position
  
};

uint16_t getPosition(uint8_t pos) {
  uint16_t val = (uint16_t)(screenPosition[pos]);
  uint16_t ret = val&POS_MASK;

  if(1 /*Settings[S_VIDEOSIGNALTYPE]*/) {
    ret += LINE * ((val >> PAL_SHFT) & PAL_MASK);
  }

  return ret;
}

uint8_t SfieldIsVisible(uint8_t pos) {
  uint16_t val = (uint16_t)(screenPosition[pos]);
  switch(val & DISPLAY_MASK) {
  case DISPLAY_ALWAYS:
    return 1;
  case DISPLAY_NEVER:
    return 0;
  //case DISPLAY_COND:
  //  return !!(MwSensorActive&mode_osd_switch);
  //case DISPLAY_CONDR:
  //  return !(MwSensorActive&mode_osd_switch);
    default:
    return 0;
  }
}



// Copy string from ram into screen buffer
void MAX7456_WriteString(const char *string, int Adresse)
{
  uint8_t xx;
  for(xx=0;string[xx]!=0;)
  {
    screen[Adresse++] = string[xx++];
  }
}

// Copy string from progmem into the screen buffer
void MAX7456_WriteString_P(const char *string, int Adresse)
{
  uint8_t xx = 0;
  char c;
  while((c = (char)(string[xx++])) != 0)
  {
    screen[Adresse++] = c;
  }
}






