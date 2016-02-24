//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// STM32 MCU low-level driver
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "ks0108.h"


#define KS0108_D0    0

#define DISPLAY_STATUS_BUSY	0x80

extern unsigned char screen_x;
extern unsigned char screen_y;

PORT_InitTypeDef PORT_InitStructure;

//-------------------------------------------------------------------------------------------------
// Delay function /for 8MHz/
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
  //asm("nop");asm("nop");asm("nop");asm("nop");
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : PORT_ResetBits(MDR_PORTE, KS0108_CS1); break;
	case 1 : PORT_ResetBits(MDR_PORTE, KS0108_CS2); break;
	//case 2 : PORT_ResetBits(KS0108_PORT, KS0108_CS3); break;
	default: break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : PORT_SetBits(MDR_PORTE, KS0108_CS1); break;
	case 1 : PORT_SetBits(MDR_PORTE, KS0108_CS2); break;
	//case 2 : PORT_SetBits(KS0108_PORT, KS0108_CS3); break;
	default: break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status byte from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
	unsigned char status;

	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin = 0xFF << KS0108_D0;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;
	PORT_Init(KS0108_PORT, &PORT_InitStructure);

	PORT_SetBits(MDR_PORTC, KS0108_RW);
	PORT_ResetBits(MDR_PORTE, KS0108_RS);
	GLCD_EnableController(controller);
	GLCD_Delay();
	PORT_SetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	status = ((PORT_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
	PORT_ResetBits(MDR_PORTC, KS0108_EN);
	GLCD_DisableController(controller);
return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
	while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
	
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin = 0xFF << KS0108_D0;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_Init(KS0108_PORT, &PORT_InitStructure);

	//PORT_ResetBits(KS0108_PORT, KS0108_RS | KS0108_RW);
	PORT_ResetBits(MDR_PORTE, KS0108_RS);
	PORT_ResetBits(MDR_PORTC, KS0108_RW);
	GLCD_Delay();
	GLCD_EnableController(controller);
	GLCD_Delay();
	PORT_SetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
	commandToWrite ^= 0xFF;
	PORT_ResetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
	GLCD_Delay();
	PORT_SetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	PORT_ResetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	GLCD_DisableController(controller);
}

//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
	unsigned char tmp;
	
	while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
	
	PORT_StructInit(&PORT_InitStructure);  
	PORT_InitStructure.PORT_Pin = 0xFF << KS0108_D0;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;
	PORT_Init(KS0108_PORT, &PORT_InitStructure);

	//PORT_SetBits(KS0108_PORT, KS0108_RS | KS0108_RW);
	PORT_SetBits(MDR_PORTE, KS0108_RS);
	PORT_SetBits(MDR_PORTC, KS0108_RW);
	
	GLCD_EnableController(screen_x / 64);
	GLCD_Delay();
	PORT_SetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	tmp = ((PORT_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
	PORT_ResetBits(MDR_PORTC, KS0108_EN);
	GLCD_DisableController(screen_x / 64);
	screen_x++;
	return tmp;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
	while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
   
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin = 0xFF << KS0108_D0;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_Init(KS0108_PORT, &PORT_InitStructure);

	//PORT_ResetBits(KS0108_PORT, KS0108_RW);
	PORT_ResetBits(MDR_PORTC, KS0108_RW);
	GLCD_Delay();
	//PORT_SetBits(KS0108_PORT, KS0108_RS);
	PORT_SetBits(MDR_PORTE, KS0108_RS);
	GLCD_Delay();
	PORT_SetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
	dataToWrite ^= 0xFF;
	PORT_ResetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
	GLCD_Delay();
	GLCD_EnableController(screen_x / 64);
	GLCD_Delay();
	PORT_SetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	PORT_ResetBits(MDR_PORTC, KS0108_EN);
	GLCD_Delay();
	GLCD_DisableController(screen_x / 64);
	screen_x++;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
// void GLCD_InitializePorts(void)
// {
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
// 	PORT_StructInit(&PORT_InitStructure);
// 	PORT_InitStructure.PORT_Pin   =  PORT_Pin_All;
// 	PORT_InitStructure.PORT_Speed =  PORT_Speed_2MHz;
// 	PORT_InitStructure.PORT_Mode  =  PORT_Mode_Out_PP;

// 	PORT_Init(KS0108_PORT, &PORT_InitStructure);
// 	PORT_Write(KS0108_PORT, KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | (0xFF << KS0108_D0));
// }
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
  return *(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

void GLCD_Reset(void)
{
	PORT_SetBits(MDR_PORTC, KS0108_RES);
	GLCD_Delay();
	PORT_ResetBits(MDR_PORTC, KS0108_RES);
	GLCD_Delay();
	PORT_SetBits(MDR_PORTC, KS0108_RES);
	
	while(GLCD_ReadStatus(0)&DISPLAY_STATUS_BUSY);
}




