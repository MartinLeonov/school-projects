#ifndef __FAT_Debug__
#define __FAT_Debug__

/* Includes */
#include "FslTypes.h"
#include "FAT.h"
#include "USB_CDC.h"    // SCI Port


#define MINICOM_BUFFER_SIZE 512
extern UINT8 gu8IntFlags;


/* function Prototypes */
void MiniCom(void);
void ErrorDisplay(UINT8);
void File_Read(void);


/* Terminal HAL */
#define Terminal_Send_String(A)     (void)CDC_Send_String(A);
#define Terminal_Send_Byte(A)       (void)cdc_putch(A);
#define Terminal_Send_Buffer(A,B)   (void)CDC_Send_Buffer(A,B);


#endif /* __FAT_Debug__ */