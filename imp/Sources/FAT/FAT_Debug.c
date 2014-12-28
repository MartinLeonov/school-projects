/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2011
*  ALL RIGHTS RESERVED
*
*  File Name:   FAT_Debug.c
*                                                                          
*  Description: Simple programs for FAT driver debug
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
#include "FAT_Debug.h"

/* Global Variables */
UINT8 gau8Minicom[MINICOM_BUFFER_SIZE];
extern UINT8 gu8IntFlags;


/***********************************************************************/
void MiniCom(void)
{
    UINT8   *pu8DataPointer;
    UINT8   u8Selection;
    UINT16  u16Counter;
    
    Terminal_Send_String((UINT8*)"\r\n\r\nDemo FAT in S08\r\n");
    Terminal_Send_String((UINT8*)"\r\n1.-Read File\n\r2.-Write File\n\r3.-Modify File");
    pu8DataPointer=gau8Minicom;
    
    while(!cdc_kbhit());
    u8Selection=cdc_getch();

    Terminal_Send_String((UINT8*)"\r\nName: ");
    
    do
    {
        while(!cdc_kbhit());
        *pu8DataPointer=cdc_getch();
        pu8DataPointer++;
        Terminal_Send_Byte((*(pu8DataPointer-1)));
        cdc_process();
    }while((*(pu8DataPointer-1))!=13);

    Terminal_Send_String("\n\r");
    
    /* Read File */
    if(u8Selection=='1')
    {
        if(!FAT_FileOpen(gau8Minicom,READ))
            File_Read();
    }
    
    /* Write File */
    if(u8Selection=='2')
    {
        (void)FAT_FileOpen(gau8Minicom,CREATE);
        pu8DataPointer=gau8Minicom;
        u16Counter=0;
        Terminal_Send_String((UINT8*)"\r\n");
        do
        {
            while(!cdc_kbhit());
            *pu8DataPointer=cdc_getch();
            //pu8DataPointer++;
            u16Counter++;
            Terminal_Send_Byte(*pu8DataPointer);
            cdc_process();
        }while(*(pu8DataPointer++)!=0x1A);
        
        FAT_FileWrite(gau8Minicom,u16Counter-1);
        FAT_FileClose();
    }

    /* Modify File */
    if(u8Selection=='3')
    {
        (void)FAT_FileOpen(gau8Minicom,MODIFY);

        pu8DataPointer=gau8Minicom;
        u16Counter=0;
        Terminal_Send_String((UINT8*)"\r\n");
        do
        {
            while(!cdc_kbhit());
            *pu8DataPointer=cdc_getch();
            pu8DataPointer++;
            u16Counter++;
            Terminal_Send_Byte((*(pu8DataPointer-1)));
            cdc_process();
        }while((*(pu8DataPointer-1))!=0x1A);
        
        FAT_FileWrite(gau8Minicom,u16Counter-1);
        FAT_FileClose();
        
    
    }
}

/***********************************************************************/
void File_Read(void)
{
    UINT16 u16BufferSize;
    do{
        u16BufferSize=FAT_FileRead(gau8Minicom);
        Terminal_Send_Buffer(gau8Minicom,u16BufferSize);
    }while(u16BufferSize);
}

/***********************************************************************/
void ErrorDisplay(UINT8 u8Error)
{
    Terminal_Send_String((UINT8*)"\r\nError: ");
    Terminal_Send_Byte(u8Error+0x30);
}