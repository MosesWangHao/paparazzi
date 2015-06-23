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

/**
 * @file modules/osd_kvteam/osd_kvteam.c
 * Maxim MAX7456 single-channel monochrome on-screen display driver.
 * Port part from kvteam
 */

#include "std.h"

#include "mcu_periph/sys_time.h"
#include "mcu_periph/gpio.h"
#include "mcu_periph/spi.h"

#include "generated/flight_plan.h"
#include "generated/airframe.h"
#include "autopilot.h"
#include "subsystems/electrical.h"
#include "state.h"
#include "subsystems/gps.h"
#include "subsystems/imu.h"

// for GetPosAlt, include correct header until we have unified API
#ifdef AP
#include "subsystems/navigation/nav.h"
#else
#include "firmwares/rotorcraft/navigation.h"
#endif

// Peripherials
#include "osd_kvteam.h"
#include "max7456_regs.h"
#include "osd_kvscreen.h"

#define OSD_STRING_SIZE     31
#define OSD_SCRREN_MAX      (480+1) //the last char must 0xff
#define OSD_LINE_MAX        20

//Fonts used to upgrade EEPROM of MAX7456
extern const unsigned char osdKVFonts[];

//Screen buffer that will be writen to the screen at 20hz
char screen[OSD_SCRREN_MAX];
//ScreenBuffer(line) is an intermietary buffer to created Strings to send to Screen buffer 
char screenBuffer[OSD_LINE_MAX];

struct spi_transaction max7456_trans;
uint8_t osd_spi_tx_buffer[2];
uint8_t osd_spi_rx_buffer[2];

uint16_t osd_eeprom_address = 0;
uint16_t eepromcount = 0;

enum max7456_osd_status_codes {
  OSD_UNINIT,
  OSD_INIT1,
  OSD_INIT2,
  OSD_INIT3,
  OSD_INIT4,
  OSD_INIT5,
  OSD_IDLE,
  OSD_S_REFRESH_SCREEN,
  OSD_S_REFRESH_SCREEN0,
  OSD_S_REFRESH_SCREEN1,
  OSD_S_REFRESH_SCREEN2,
  OSD_FONT_UPDATE,
  OSD_FONT_UPDATE0,
  OSD_FONT_UPDATE1,
  OSD_FONT_UPDATE2,
  OSD_FONT_UPDATE3,
  OSD_FINISHED,
};

uint8_t max7456_osd_status = OSD_UNINIT;
uint8_t osd_enable = TRUE;
uint8_t osd_enable_val = OSD_IMAGE_ENABLE;
float osdkvteam_get_heading_home(void);

void osdkvteam_init(void)
{
  for(int16_t i = 0; i < OSD_SCRREN_MAX-1; i++)
    screen[i] = 0;
  screen[OSD_SCRREN_MAX-1] = 0XFF;

  max7456_trans.slave_idx     = MAX7456_SLAVE_IDX;
  max7456_trans.select        = SPISelectUnselect;
  max7456_trans.cpol          = SPICpolIdleLow;
  max7456_trans.cpha          = SPICphaEdge1;
  max7456_trans.dss           = SPIDss8bit;
  max7456_trans.bitorder      = SPIMSBFirst;
  max7456_trans.cdiv          = SPIDiv256;
  max7456_trans.output_length = sizeof(osd_spi_tx_buffer);
  max7456_trans.output_buf    = (uint8_t *) osd_spi_tx_buffer;
  max7456_trans.input_length  = 0;
  max7456_trans.input_buf     = (uint8_t *)osd_spi_rx_buffer;
  max7456_trans.before_cb     = NULL;
  max7456_trans.after_cb      = NULL;
  osd_enable = 1;
  osd_enable_val = OSD_IMAGE_ENABLE;
  max7456_osd_status = OSD_UNINIT;
  return;
}

void osdkvteam_periodic(void)
{
  int16_t BodyHeading, HomeHeading, HomeDistance; 
  static int16_t i = 0;
  //This code is executed always and checks if the "osd_enable" var has been changed by telemetry.
  //If yes then it commands a reset but this time turns on or off the osd overlay, not the video.
  if (max7456_osd_status == OSD_IDLE) {
    if (osd_enable > 1) {
      osd_enable = 1;
    }
    if ((osd_enable << 3) !=  osd_enable_val) {
      osd_enable_val = (osd_enable << 3);
      max7456_osd_status = OSD_UNINIT;
    }
  }

  //--------------------------------INITIALIZATION OF THE OSD-------------------------------------
  if (max7456_osd_status == OSD_UNINIT) {
    //reset max7456
    max7456_trans.status = SPITransDone;
    max7456_trans.output_buf[0] = OSD_VM0_REG;
    //This operation needs at least 100us but when the periodic function will be invoked again
    //sufficient time will have elapsed even with at a periodic frequency of 1000 Hz
    max7456_trans.output_buf[1] = OSD_RESET;
    #ifdef KVFONT_UPDATE
      max7456_osd_status = OSD_FONT_UPDATE;
    #else
      max7456_osd_status = OSD_INIT1;
    #endif
    spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
  }
  else if(max7456_osd_status == OSD_FONT_UPDATE0) {    
    //Updata EEPROM, need at least 1000/20=50ms*256 ~ 13s
    if(eepromcount < 256) {
      osd_eeprom_address = eepromcount;
      max7456_trans.output_length = 2;
      max7456_trans.input_length = 0;
      max7456_trans.output_buf[0] = OSD_VM0_REG;
      max7456_trans.output_buf[1] = 0; 	     //reset value, display off
      spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
      max7456_osd_status = OSD_FONT_UPDATE1; //write eeprom one by one
      eepromcount++; 
    }
    else
      max7456_osd_status = OSD_INIT2;
  } 
  else if(max7456_osd_status == OSD_INIT2) {
    //Init MAX7456 after reset READ OSDBL register
    max7456_trans.output_length = 1;
    max7456_trans.input_length = 1;
    max7456_trans.output_buf[0] = OSD_OSDBL_REG_R;
    max7456_osd_status = OSD_INIT3;
    spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
  }
  else if (max7456_osd_status == OSD_IDLE && osd_enable > 0) { 
    // DRAW THE OSD SCREEN
    //=============clean screen buffer=====================
    for(i = 0; i < OSD_SCRREN_MAX; i++)
    	screen[i] = 0;
    screen[OSD_SCRREN_MAX-1] = 0XFF;

    //=============Electrical==============================
    displayVoltage(electrical.vsupply);
    //displayAmperage(electrical.current/100);
    //displaypMeterSum(electrical.consumed);

    //=============Heading=================================
    BodyHeading = DegOfRad(stateGetNedToBodyEulers_f()->psi);
    displayHeading(BodyHeading);
    displayHeadingGraph(BodyHeading);

    //=============Speed===================================
    displayGPS_speed(state.h_speed_norm_f);
    displayClimbRate(stateGetSpeedEnu_f()->z*100);

    //=============Alt=====================================
    displayAltitude(GetPosAlt());

    //=============GPS=====================================
    displayNumberOfSat(gps.num_sv);
    //displayGPSPosition(gps.lla_pos.lat, gps.lla_pos.lon, gps.lla_pos.alt);

    //=============IMU=====================================
    displayHorizon(DegOfRad(stateGetNedToBodyEulers_f()->phi), DegOfRad(stateGetNedToBodyEulers_f()->theta));
    
    //=============Home direction and distance=============
    HomeDistance = sqrt(dist2_to_home);
    HomeHeading = DegOfRad(osdkvteam_get_heading_home());
    displayDistanceToHome(HomeDistance);
    displayAngleToHome(HomeHeading, HomeDistance);
    displayDirectionToHome(HomeHeading, HomeDistance, BodyHeading);

    //FOLLOW IS SOME DISPLAY INFO HAVEN'T FIND DATA SOURCE
    //displayMode();
    //displayArmed(2);
    //displayCallsign();
    //displayCurrentThrottle(100);
    //displayTime(3000);
    //displayRSSI(80);
    //displayTemperature(??);
    //displayIntro();

    //TRIGGER THE SPI TRANSFERS. The rest of the spi transfers occur in the "max7456_event" function.
    max7456_trans.output_length = 2;
    max7456_trans.output_buf[0] = OSD_DMAH_REG;
    max7456_trans.output_buf[1] = 0;
    max7456_osd_status = OSD_S_REFRESH_SCREEN;
    spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
  }
  else{}
  return;
}

void osdkvteam_event(void)
{
  static uint16_t x = 0;
  static uint16_t eeprom_bmpbitcount = 0; 
  if (max7456_trans.status == SPITransSuccess) {
    max7456_trans.status = SPITransDone;
    switch (max7456_osd_status) {
      case (OSD_FONT_UPDATE):
        max7456_osd_status = OSD_FONT_UPDATE0;
        break;
      case (OSD_FONT_UPDATE1):
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_CMAH_REG;
        max7456_trans.output_buf[1] = osd_eeprom_address;
        max7456_osd_status = OSD_FONT_UPDATE2;
        eeprom_bmpbitcount = 0;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans); 
        break; 
      case (OSD_FONT_UPDATE2): //write 54byte for per char
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        if((eeprom_bmpbitcount&0x1) == 0) {
           max7456_trans.output_buf[0] = OSD_CMAL_REG;
           max7456_trans.output_buf[1] = eeprom_bmpbitcount/2;
        }
        else {
           max7456_trans.output_buf[0] = OSD_CMDI_REG;
           max7456_trans.output_buf[1] = *(osdKVFonts+osd_eeprom_address*64+eeprom_bmpbitcount/2);
        }
        eeprom_bmpbitcount++;
        if(eeprom_bmpbitcount == 54*2)
           max7456_osd_status = OSD_FONT_UPDATE3; 
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        break;
      case (OSD_FONT_UPDATE3):
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_CMM_REG;
        max7456_trans.output_buf[1] = 0xa0; //wirite to nvm array need at least 12ms
        max7456_osd_status = OSD_FONT_UPDATE;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans); 
        break;
      case (OSD_INIT1):
        max7456_osd_status = OSD_INIT2;
        break;
      case (OSD_INIT3): //Set OSD BLACK LEVEL
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_OSDBL_REG;
        max7456_trans.output_buf[1] = max7456_trans.input_buf[0] & (~(1 << 4));
        max7456_osd_status = OSD_INIT4;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        break;
      case (OSD_INIT4): //Set OSD PAL/NTSC mode and enable OSD
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_VM0_REG;
        #if USE_PAL_FOR_OSD_VIDEO
        max7456_trans.output_buf[1] = OSD_VIDEO_MODE_PAL | osd_enable_val;
        #else
        max7456_trans.output_buf[1] = osd_enable_val;
        #endif
        max7456_osd_status = OSD_INIT5;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        break;
      case (OSD_INIT5):
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_DMM_REG;
        max7456_trans.output_buf[1] = OSD_8BIT_MODE;
        max7456_osd_status = OSD_FINISHED;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        break;
      case (OSD_S_REFRESH_SCREEN):
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_DMAL_REG;
        max7456_trans.output_buf[1] = 0;
        max7456_osd_status = OSD_S_REFRESH_SCREEN1;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        break;
      case (OSD_S_REFRESH_SCREEN1):
        max7456_trans.output_length = 2;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf[0] = OSD_DMM_REG;
        max7456_trans.output_buf[1] = OSD_8BIT_MODE|OSD_AUTO_INCREMENT_MODE;
        max7456_osd_status = OSD_S_REFRESH_SCREEN2;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        x = 0;
        break;
      case (OSD_S_REFRESH_SCREEN2):
        if (screen[x] != 0XFF) {
          max7456_trans.output_buf[0] = screen[x++];
          spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        } else {
          max7456_trans.output_buf[0] = 0xFF; //Exit the auto increment mode
          max7456_osd_status = OSD_FINISHED;
          spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);
        }
	/*Not implement, try send all data by DMA but failed
        for(int i = 0; i < OSD_SCRREN_MAX-1; i++)
		screen[i] = i&0xF;
        screen[9] = 0xff;
        max7456_trans.output_length = OSD_SCRREN_MAX;
        max7456_trans.input_length = 0;
        max7456_trans.output_buf    = (uint8_t *) screen;
        screen[OSD_SCRREN_MAX-1] = 0xff;  //end flag exit the auto increment mode
        max7456_osd_status = OSD_FINISHED;
        spi_submit(&(MAX7456_SPI_DEV), &max7456_trans);*/
        break;
      case (OSD_FINISHED):
        max7456_trans.status = SPITransDone;
        max7456_osd_status = OSD_IDLE;
        break;
      default: break;
    }
  }
  return;
}

float osdkvteam_get_heading_home(void)
{
  float x,y;
  x = WaypointX(WP_HOME);
  y = WaypointY(WP_HOME);
  struct FloatVect2 target={x,y};
  struct FloatVect2 pos_diff;
  VECT2_DIFF(pos_diff, target, *stateGetPositionEnu_f());
  float heading_f = atan2f(pos_diff.x, pos_diff.y); 
  return heading_f;
}







