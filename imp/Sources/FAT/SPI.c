/**************************************
 Zmeny provedl dne 7. 10. 2012:
 Roman Mazur
 Login: xmazur01
 Email: xmazur01@stud.fit.vutbr.cz
 Odhad podilu zmen vzhledem k puvodnimu obsahu: 20%
**************************************/

#include "SPI.h"

/************************************************/
void SPI_Init(void)
{
  
  //SOPT2 = SOPT2_SPI1PS_MASK;  // Drive PTE as SPI port
  
  SPI_SS = 1;
  _SPI_SS= 1;
  
  SPI1BR = 0x24; // 375 Khz     		
  SPI1C2 = 0x00;     
  SPI1C1 = SPI1C1_SPE_MASK | SPI1C1_MSTR_MASK;
}

/************************************************/
void SPI_Send_byte(UINT8 u8Data)
{
	(void)SPI1S;
	SPI1DL=u8Data;
	while(!SPI1S_SPTEF);
}

/************************************************/
UINT8 SPI_Receive_byte(void)
{
	(void)SPI1DL;
	SPI1DL=0xFF;
	while(!SPI1S_SPRF);	
	return(SPI1DL);
}

/************************************************/
void SPI_High_rate(void)
{
  SPI1C1 = 0x00;
  SPI1BR = 0x11; // 375 Khz     		
  SPI1C1 = SPI1C1_SPE_MASK | SPI1C1_MSTR_MASK;

}
