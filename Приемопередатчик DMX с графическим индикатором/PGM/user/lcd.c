//-------------------------------------------------------------------------------------------------
// Graphic LCD C library
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "stm8s.h"
#include "font5x8.h"
#include "stm8s_gpio.h"
#include "lcd.h"
//-------------------------------------------------------------------------------------------------
// Reference to MCU-specific external functions
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void);
void GLCD_WriteData(unsigned char);
void GLCD_WriteCommand(unsigned char, unsigned char);
void GLCD_EnableController(unsigned char controller);
void GLCD_DisableController(unsigned char controller);
void GLCD_Reset(void);
void GLCD_WriteINVChar(char );
unsigned char GLCD_ReadStatus(unsigned char);
//-------------------------------------------------------------------------------------------------
volatile unsigned char screen_x = 0, screen_y = 0; // screen coordinates
//-------------------------------------------------------------------------------------------------
// Initialization interface and LCD controller
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void)
{
    unsigned char i;
    
    GLCD_InitializePorts();
    
    //reset lcd
    GLCD_Reset();
    
    for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
      GLCD_WriteCommand((DISPLAY_ON_CMD | ON), i);
}
//-------------------------------------------------------------------------------------------------
// Sets screen coordinates and address 
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
    unsigned char i;
    
    screen_x = x;
    screen_y = y;

    for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
      {
      GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
      GLCD_WriteCommand(DISPLAY_SET_X | y,i);
      GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
      }
    GLCD_WriteCommand((DISPLAY_SET_Y | (x % 64)), (x / 64));
    GLCD_WriteCommand((DISPLAY_SET_X | y), (x / 64));
}
//-------------------------------------------------------------------------------------------------
// Sets text coordinates and address 
//-------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
    GLCD_GoTo(x*6,y);
}
//-------------------------------------------------------------------------------------------------
// Clears screen
//-------------------------------------------------------------------------------------------------
void GLCD_ClearScreen(void)
{
    unsigned char i, j;
    
    for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
      {
      GLCD_GoTo(0,j);
      for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
        	GLCD_WriteData(0x00);
      }
}
//-------------------------------------------------------------------------------------------------
// Schow Border
//-------------------------------------------------------------------------------------------------
void GLCD_DrawBrd(void)
{
    unsigned char i, j;
    
	GLCD_GoTo(0,0);
	GLCD_WriteData(0xFF);
	GLCD_WriteData(0x1);
	GLCD_WriteData(0xFD);
	
	for(i = 3; i < KS0108_SCREEN_WIDTH-3; i++)
        GLCD_WriteData(0x5);
	
	GLCD_WriteData(0xFD);
	GLCD_WriteData(0x1);
	GLCD_WriteData(0xFF);
	
	for(j = 1; j < KS0108_SCREEN_HEIGHT/8-1; j++)
    {	
		GLCD_GoTo(0,j);
		GLCD_WriteData(0xFF);
		GLCD_WriteData(0x00);
		GLCD_WriteData(0xFF);
		GLCD_GoTo(KS0108_SCREEN_WIDTH-3,j);
		GLCD_WriteData(0xFF);
		GLCD_WriteData(0x00);
		GLCD_WriteData(0xFF);
	}
	
	GLCD_GoTo(0,KS0108_SCREEN_HEIGHT/8-1);
	GLCD_WriteData(0xFF);
	GLCD_WriteData(0x80);
	GLCD_WriteData(0xBF);
	
	for(i = 3; i < KS0108_SCREEN_WIDTH-3; i++)
        GLCD_WriteData(0xA0);
	
	GLCD_WriteData(0xBF);
	GLCD_WriteData(0x80);
	GLCD_WriteData(0xFF);
	
}
//-------------------------------------------------------------------------------------------------
// Schow Battery_CHG
//-------------------------------------------------------------------------------------------------
void GLCD_bat_chg(unsigned char prc)
{
    unsigned char i, j;
    
	GLCD_GoTo(KS0108_SCREEN_WIDTH-15,0);
	
	if(prc > 75)
		i = 0;
	else
		if(prc > 50 && prc <= 75)
			i = 15;
		else
			if(prc > 25 && prc <= 50)
				i = 30;
			else
				if(prc <= 25)
					i = 45;
	
	j = i + 12;
	
	for(; i <  j; i++)
        GLCD_WriteData(battery[i]);
	
	GLCD_WriteData((battery[i]|0xC0));
	GLCD_WriteData(battery[i+1]);
	GLCD_WriteData((battery[i+2]|0xC0));
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
  return *(ptr);
}

//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charToWrite)
{
    char i;
    
    charToWrite -= 32; 
    
    for(i = 0; i < 5; i++) 
      GLCD_WriteData(GLCD_ReadByteFromROMMemory((char *)(font5x8 + (5 * charToWrite) + i))); 
    
    GLCD_WriteData(0x00);
}
//-------------------------------------------------------------------------------------------------
// Writes INV char to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteINVChar(char charToWrite)
{
    char i;
	unsigned char t=0;
    
    charToWrite -= 32; 
    
    for(i = 0; i < 5; i++)
	{
	  t = GLCD_ReadByteFromROMMemory((char *)(font5x8 + (5 * charToWrite) + i));
	
      GLCD_WriteData(~t);
	}
    
    GLCD_WriteData(0xFF);
}
//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite)
{
  while(*stringToWrite)
    GLCD_WriteChar(*stringToWrite++);
}
//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteStringBlack(char * stringToWrite)
{
  while(*stringToWrite)
    GLCD_WriteINVChar(*stringToWrite++);
}
//-------------------------------------------------------------------------------------------------
// Sets or clears pixel at (x,y)
//-------------------------------------------------------------------------------------------------
//void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
//{
//  unsigned char tmp;
//  
//  GLCD_GoTo(x, y/8);
//  tmp = GLCD_ReadData();
//  GLCD_GoTo(x, y/8);
//  tmp = GLCD_ReadData();
//  GLCD_GoTo(x, y/8);
//  if(color)
//      tmp |= (1 << (y%8));
//  else
//      tmp &= ~(1 << (y%8));
//  GLCD_WriteData(tmp);
//}
//-------------------------------------------------------------------------------------------------
// Displays bitmap at (x,y) and (dx,dy) size
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
  unsigned char i, j;
  
  for(j = 0; j < dy / 8; j++)
    {
    GLCD_GoTo(x,y + j);
    for(i = 0; i < dx; i++) 
      GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
    }
}

//-------------------------------------------------------------------------------------------------
// Öèêëè÷åñêè ïåðåçàïèñûâàåò ñòðîêè íà äèñïëåå
//-------------------------------------------------------------------------------------------------
void GLCD_PutString(char * stringToPut)
{
    static unsigned char str=1;
    unsigned char i=0;
    
    if(str<8)
    {
      GLCD_GoTo(0,str);
      for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
              GLCD_WriteData(0x00);
      GLCD_GoTo(0,str);
      GLCD_WriteString(stringToPut);
      ++str;
    }
    else
    {
      str=1;
    }
}
//-------------------------------------------------------------------------------------------------
// Delay function /for 8MHz/
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
  //asm("nop");asm("nop");asm("nop");asm("nop");
	nop();
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : GPIO_WriteLow(GPIOC, KS0108_CS1); break;
	case 1 : GPIO_WriteLow(GPIOC, KS0108_CS2); break;
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
	case 0 : GPIO_WriteHigh(GPIOC, KS0108_CS1); break;
	case 1 : GPIO_WriteHigh(GPIOC, KS0108_CS2); break;
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

//    GPIO_StructInit(&GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = 0xFF ;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
    GPIO_Init(KS0108_PORT,GPIO_PIN_ALL,GPIO_MODE_IN_PU_NO_IT);
    
    GPIO_WriteHigh(GPIOC, KS0108_RW);
    GPIO_WriteLow(GPIOC, KS0108_RS);
    GLCD_EnableController(controller);
    GLCD_Delay();
    GPIO_WriteHigh(GPIOC, KS0108_EN);
    GLCD_Delay();
    status = ((GPIO_ReadInputData(KS0108_PORT) ) & 0xFF);
    GPIO_WriteLow(GPIOC, KS0108_EN);
    GLCD_DisableController(controller);
    return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
    while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
//    GPIO_StructInit(&GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin  = (0xFF );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

    GPIO_Init(KS0108_PORT,GPIO_PIN_ALL,GPIO_MODE_OUT_PP_LOW_FAST);
    
    GPIO_WriteLow(GPIOC, (GPIO_Pin_TypeDef)(KS0108_RS | KS0108_RW));
    GLCD_Delay();
    GLCD_EnableController(controller);
    GLCD_Delay();
	GPIO_Write(KS0108_PORT, (GPIO_Pin_TypeDef)(commandToWrite ));
    GLCD_Delay();
    GPIO_WriteHigh(GPIOC, KS0108_EN);
    GLCD_Delay();
    GPIO_WriteLow(GPIOC, KS0108_EN);
    GLCD_Delay();
    GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
    while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
       
//    GPIO_StructInit(&GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = (0xFF );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
    
    GPIO_Init(KS0108_PORT,GPIO_PIN_ALL,GPIO_MODE_OUT_PP_LOW_FAST);

    GPIO_WriteLow(GPIOC, KS0108_RW);
    GLCD_Delay();
    GPIO_WriteHigh(GPIOC, KS0108_RS);
    GLCD_Delay();

	GPIO_Write(KS0108_PORT, (GPIO_Pin_TypeDef)(dataToWrite ));
    GLCD_Delay();
	
    GLCD_EnableController(screen_x / 64);
    GLCD_Delay();
    GPIO_WriteHigh(GPIOC, KS0108_EN);
    GLCD_Delay();
    GPIO_WriteLow(GPIOC, KS0108_EN);
    GLCD_Delay();
    GLCD_DisableController(screen_x / 64);
    screen_x++;
}
void GLCD_Reset(void)
{
    GPIO_WriteHigh(GPIOC, KS0108_RES);
    GLCD_Delay();
    GPIO_WriteLow(GPIOC, KS0108_RES);
    GLCD_Delay();
    GPIO_WriteHigh(GPIOC, KS0108_RES);
    
    while(GLCD_ReadStatus(0)&DISPLAY_STATUS_BUSY);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{

  	GPIO_Init(LCD_DATA_PORT,(GPIO_Pin_TypeDef)LCD_DATA_BUS_8,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(KS0108_CS1 | KS0108_CS2 | KS0108_RS | KS0108_RW | KS0108_RES | KS0108_EN),GPIO_MODE_OUT_PP_HIGH_FAST);
    //GPIO_WriteHigh(GPIOC, KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | (0xFF << KS0108_D0));
}
//-------------------------------------------------------------------------------------------------
// End of file 
//-------------------------------------------------------------------------------------------------





