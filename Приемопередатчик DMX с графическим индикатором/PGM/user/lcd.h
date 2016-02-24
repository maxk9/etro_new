//-------------------------------------------------------------------------------------------------
// Graphic LCD C library
// http://glcd-libc.dxp.pl
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------




#define KS0108_SCREEN_WIDTH	 128
#define KS0108_SCREEN_HEIGHT 64


#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
  #define ON	0x01
  #define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

#define LCD_DATA_PORT      GPIOB

#define LCD_DATA_BUS_8     (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
							
#define KS0108_PORT  GPIOB

#define KS0108_RS    GPIO_PIN_6
#define KS0108_RW    GPIO_PIN_5 
#define KS0108_EN    GPIO_PIN_7

#define KS0108_RES    GPIO_PIN_3

#define KS0108_CS2   GPIO_PIN_2
#define KS0108_CS1   GPIO_PIN_1							
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void);
void GLCD_WriteData(unsigned char);
void GLCD_ClearScreen(void);
void GLCD_TextGoTo(unsigned char, unsigned char);
void GLCD_GoTo(unsigned char, unsigned char);
void GLCD_WriteString(char *);
void GLCD_WriteStringBlack(char *);
unsigned char GLCD_ReadByteFromROMMemory(char *);
void GLCD_Reset(void);
void GLCD_PutString(char * stringToPut);
void GLCD_DrawBrd(void);
void GLCD_bat_chg(unsigned char prc);
//-------------------------------------------------------------------------------------------------
// End of file KS0108.h
//-------------------------------------------------------------------------------------------------



