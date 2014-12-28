/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2011
*  ALL RIGHTS RESERVED
*
*  File Name:   Main.c
*                                                                          
*  Description: USB CDC, FAT lite and SD card Demo implementation 
*                                                                                     
*  Assembler:   Codewarrior for HC(S)08 V6.1
*                                            
*  Version:     1.1                                                         
*                                                                                                                                                         
*  Author:      Jose Ruiz (SSE Americas)
*                                                                                       
*  Location:    Guadalajara,Mexico                                              
*                                                                                                                  
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2008.02.18  Jose Ruiz     Initial version
* 1.1   2011.09.28  Carlos C.     Updated version for using FAT.h v1.2 and 
*                                 FAT.c v1.3 files 
* 
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/

/* Include Files */
#include <hidef.h>          
#include "usb.h"
#include "target.h"
#include "usb_cdc.h"
#include "FslTypes.h"
#include "Fat.h"
#include "SD.h"
#include "FAT_Debug.h"


/************************************************************************************************/
void main(void)
{
  int cdc_in;
  int uart_in; 
  volatile UINT8 u8Error;
  
  PTBPE_PTBPE5 = 1;                     // DEMOFLEXISJMSD board 
  PTBDD_PTBDD5 = _IN;                     // Initialize Button in 

  hw_init();                            /* MCU Initialization */
  SPI_Init();                           /* SPI Module Init */
  u8Error=SD_Init();                    /* SD Card Init */
  FAT_Read_Master_Block();              /* FAT Driver Init */
  
  cdc_in=uart_in=0xff+1;                /* USB CMX Init */
  usb_cfg_init();
  cdc_init();


  while(PTBD_PTBD5);                    /* Wait until button clic */  
  CDC_Send_String("\r\n\r\n\tSD Card Terminal\r\n");  
  

  while(1)
  {
    cdc_process();                      /* USB CMX CDC process */
    FAT_LS();                           /* Simple function that shows the content*/
    MiniCom();                          /* User Interface call */
  }
}
