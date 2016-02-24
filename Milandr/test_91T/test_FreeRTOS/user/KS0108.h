//-------------------------------------------------------------------------------------------------
// Graphic LCD C library
// http://glcd-libc.dxp.pl
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#define KS0108_SCREEN_WIDTH		128
#define KS0108_SCREEN_HEIGHT	64


#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
  #define ON	0x01
  #define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

#define LCD_DATA_PORT      MDR_PORTA

#define LCD_DATA_BUS_8     (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | \
                            PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7)
							
#define KS0108_PORT  MDR_PORTA

#define KS0108_RS    PORT_Pin_11//MDR_PORTE
#define KS0108_RW    PORT_Pin_2 //MDR_PORTC
#define KS0108_EN    PORT_Pin_7//MDR_PORTC

#define KS0108_RES    PORT_Pin_9//MDR_PORTC

#define KS0108_CS2   PORT_Pin_4//MDR_PORTE
#define KS0108_CS1   PORT_Pin_5//MDR_PORTE							
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void);
void GLCD_WriteData(unsigned char);
void GLCD_ClearScreen(void);
void GLCD_TextGoTo(unsigned char, unsigned char);
void GLCD_GoTo(unsigned char, unsigned char);
void GLCD_WriteString(char *);
unsigned char GLCD_ReadByteFromROMMemory(char *);
void GLCD_Reset(void);
void GLCD_PutString(char * stringToPut);
//-------------------------------------------------------------------------------------------------
// End of file KS0108.h
//-------------------------------------------------------------------------------------------------



