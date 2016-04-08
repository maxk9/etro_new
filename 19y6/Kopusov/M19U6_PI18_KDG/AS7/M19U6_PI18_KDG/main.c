


	//#include <iom8535.h>
	#include <avr/io.h>
	#include <stdint.h>
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#define F_CPU	8000000
	#include <util/delay.h>
	#include <avr/pgmspace.h>
	#include "font5x7.h"
	







#define  LCD_DIS 		( PORTB &= ~4 )
#define  LCD_EN 		( PORTB |= 4 )
#define  LCD_ALL_ROW_OFF ( PORTA &= ~0x1F )
							
							
#define  LCD_ROW1_ON 	( PORTA |= 1 )
#define  LCD_ROW2_ON 	( PORTA |= 2 )
#define  LCD_ROW3_ON 	( PORTA |= 4 )					
#define  LCD_ROW4_ON 	( PORTA |= 8 )
#define  LCD_ROW5_ON 	( PORTA |= 16 )

#define  LCD_ROW1_SW 	( PORTA ^= 1 )



const uint8_t   KHU[]  PROGMEM        = {0, 0,0 ,0 ,43,57 ,52 },
				NIOBII[] PROGMEM      = {0, 0,0 ,46,41,47 ,34  ,41 ,42 },
				NIZKAYA[] PROGMEM     = {0,0 ,0 ,46,41,40 ,43  ,33 ,64} ,
				ISOLYACIA[] PROGMEM   = {0,0 ,41,40,47,44 ,64  ,55 ,41  ,64},
				
				OTKAZ[] PROGMEM       = {0,0 ,0 ,47,51,43 ,33  ,40},
				LUKA_1[] PROGMEM      = {0,0 ,0 ,44,63,43 ,33  ,0  ,17} ,
				LUKA_2[] PROGMEM      = {0,0 ,0 ,44,63,43 ,33  ,0  ,18} ,
				LUKA_3[] PROGMEM      = {0,0 ,0 ,44,63,43 ,33  ,0  ,19} ,
				LUKA_4[] PROGMEM      = {0,0 ,0 ,44,63,43 ,33  ,0  ,20} ,
				LUKA_5[] PROGMEM      = {0,0 ,0 ,44,63,43 ,33  ,0  ,21} ,
				CHSN[] PROGMEM        = {0,0 ,0 ,0 ,57,50 ,46} ,
				LUKA_DG1[] PROGMEM    = {0,0 ,44,36,0 ,37 ,36  ,0  ,17} ,
				LUKA_DG2[] PROGMEM    = {0,0 ,44,36,0 ,37 ,36  ,0  ,18} ,
				LUKA_LR_DG1[] PROGMEM = {0,0 ,44,49,0 ,37 ,36  ,0  ,17} ,
				LUKA_LR_DG2[] PROGMEM = {0,0 ,44,49,0 ,37 ,36  ,0  ,18} ,
				LUKA_PCH1[] PROGMEM   = {0,0 ,44,63,43,33 ,0   ,48 ,56  ,17}, 
				LUKA_PCH2[] PROGMEM   = {0,0 ,44,63,43,33 ,0   ,48 ,56  ,18},
				FVU[] PROGMEM         = {0,0 ,0 ,0 ,53,35 ,52} ,
				DT[] PROGMEM          = {0,0 ,0 ,0 ,37,51},
				
				GOTOVNOST[] PROGMEM   = {0,36,47,51,47,35 ,46  ,47 ,50  ,51 ,61},
				WORK[] PROGMEM        = {0,0 ,0 ,49,33,34 ,47  ,51 ,33} ,
				MU[] PROGMEM          = {0,0 ,0 ,0 ,0 ,45 ,52 },
				DU[] PROGMEM          = {0,0 ,0 ,0 ,0 ,37 ,52 },
				
				PROTECT[] PROGMEM     = {0,0 ,0 ,40,33,58 ,41  ,51 ,33} ,
				OTKLYCHENA[] PROGMEM  = {0,47,51,43,44,63 ,56  ,38 ,46  ,33},
				
				NO_SVYAZ[] PROGMEM    = {0,46,38,51,0 ,50 ,35  ,64 ,40  ,41},
				S_SHU[] PROGMEM       = {0,0 ,0 ,0 ,50,0  ,57  ,52},
				S_MSHU[] PROGMEM      = {0,0 ,0 ,50,0 ,45 ,58  ,52},
				SVYAZ_MSHU[] PROGMEM  = {0,0 ,50,35,64,40 ,14  ,45 ,58  ,52},
				SVYAZ_RN[] PROGMEM    = {0,0 ,50,35,64,40 ,14  ,49 ,46} ,
				PEREGRUZ[] PROGMEM    = {0,0 ,48,38,49,38 ,36  ,49 ,52  ,40},
				
				REGIM_MSHU[] PROGMEM      = {0,49 ,38,39,41,45,0 ,45 ,58  ,52};
				
				
				


	unsigned char			CtErrorLink[2];
	const unsigned char		CtErrorLink0[2]={20,100};
	unsigned char			Error;
	/*
		7	No link
	*/

	uint8_t			RomReceive[12],
					RegTransmit[12],
					RamReceive[12],
					EnableLoad = 0,
					TestTWI = 0,
					RegS = 0,
					Data10[3],
					NumberIndicator = 0,
					CtBegin = 0,
					CtReceive=0,
					CtTransmit=0;
					
	const unsigned int	CtTWCR0 = 5000;//500;

	const uint16_t CtErrorReceive0 = 10000;
	uint8_t	OverlowT1;
	
	volatile uint8_t	led_reg[21][5],
						index_char[24],
						sec05=0,
						sec=0;
					




	
//==================================================================================	
static void uart_38400(void)
{
 // // UCSRA |= (1 << U2X);
	UCSRA=0;//??????? ????????
	// /* Enable receiver and transmitter */
	// //UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRB = (1<<TXEN);
	// /* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);

	UBRRH=0;
	UBRRL=12;//38400

	while(!(UCSRA & 0x20))
		wdt_reset();
}

//==================================================================================	
void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while(!( UCSRA & (1<<UDRE)));
	/* Put data into buffer, sends the data */
	UDR = data;
}
//==================================================================================
unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}	


//==================================================================================
void load_led_reg( void )
{
	uint8_t reg = 0, 
			row = 0,
			digit = 0,
			bit7 = 0,
			bit8_reg=0,
			tmp_b=0;
			
	uint16_t index_font=0;

	cli();
	while( row < 5 )
	{
		digit=0;
		index_font = index_char[ digit ]*5 + row;
		
		bit7 = 7;
		reg=0;
		while(reg<21)
		{
			bit8_reg=0;
			led_reg[reg][row] = 0;
			
			while(bit8_reg<8)
			{
				wdt_reset();
				tmp_b = pgm_read_byte_near(&font5x7[index_font]);
				led_reg[reg][row] |= ((tmp_b>>bit7)&1)<<(bit8_reg);
								
				--bit7;
				++bit8_reg;
				
				if(!bit7)
				{
					bit7 = 7;
					if(digit<23)
					{
						++digit;
						index_font = index_char[ digit ]*5 + row;
					}
				}
			}
			++reg;
		}
		++row;
	}
	sei();
}
//==================================================================================	
	
void clear_lcd( void )
{
	uint8_t reg=0, row=0, cnt_b=0;
	
	while(cnt_b<24)
	{
		index_char[cnt_b] = 0;
		++cnt_b;
	}
	
	while(reg<21)
	{
		row=0;
		while(row<5)
		{
			led_reg[reg][row] = 0;
			++row;
		}
		++reg;
	}
}

//==================================================================================	
void ReadKn(void)
	{
		static uint8_t RegSTemp =0;
		unsigned char R0;

		R0=RegSTemp;//Temp;
		if(bit_is_clear(PINA,PA5))
			R0 |=0x1;
		else
			R0 &=0xfe;
		// if(bit_is_clear(PINB,PB3))
			// R0 |=0x2;
		// else
			// R0 &=0xfd;
		// if(bit_is_clear(PINB,PB4))
			// R0 |=0x4;
		// else
			// R0 &=0xfb;

		if(R0==RegSTemp)
			RegS=R0;
		RegSTemp=R0;	
	}

//void Show_led_str( const uint8_t *str, uint8_t n_str, uint8_t len )
void Show_led_str( PGM_P addr, uint8_t n_str, uint8_t len )
{
	uint8_t cnt_b = 0, i = 0;
	
	if( n_str == 0 )
	{
		while( cnt_b < 12 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		
		cnt_b = 0;
		while( cnt_b < len )
		{
			index_char[ cnt_b ] = pgm_read_byte(&addr[cnt_b]);
			++cnt_b;
		}
	}
	else
	{
		cnt_b = 12;
		while( cnt_b < 24 )
		{
			index_char[ cnt_b ] = 0;
			++cnt_b;
		}
		cnt_b = 12;
		while( i < len )
		{
			index_char[ cnt_b ] = pgm_read_byte(&addr[i]);
			++cnt_b;
			++i;
		}
	}

	load_led_reg();
}

void LoadRomReceive(void)
{
	unsigned char R0;
	unsigned char R1;           
	R0=0;
	for(R1=1;R1<=10;++R1)
	R0 +=RamReceive[R1];
	if(R0==RamReceive[0])
	{
		// ++TestTwi;
		for(R0=1;R0<=10;++R0)
		{
			RomReceive[R0]=RamReceive[R0];
			//USART_Transmit(RomReceive[R0]);
		}

	// USART_Transmit('\n');
	// USART_Transmit('\r');


		CtErrorLink[0]=CtErrorLink0[0];
		CtErrorLink[1]=CtErrorLink0[1];
	}
}  

void ReceiveTransmitSlave(void)
{

	//USART_Transmit((TWSR & 0xf8));
	if((TWSR & 0xf8)==0x60)//Adr Slave Receive
	{
		//USART_Transmit('A');
		CtReceive=10;
	}
	else if((TWSR & 0xf8)==0x80)//Data Slave Receive
	{
		RamReceive[ CtReceive ] = TWDR;
		if(CtReceive)
		{
			--CtReceive;
		}
		else
		{
			//	++TestTwi;
			LoadRomReceive();
		}
	}
	else if((TWSR & 0xf8)==0xA8)//Adr Slave Transmit
	{

		CtTransmit=10;
		EnableLoad=0;
		TWDR=RegTransmit[CtTransmit];
		//USART_Transmit('T');
	}
	else if((TWSR & 0xf8)==0xb8)//Data Slave Transmit
	{
		if(CtTransmit)
		{
			--CtTransmit;
			TWDR=RegTransmit[CtTransmit];
		}
		else
		{
			TWDR=RegTransmit[CtTransmit];
			EnableLoad=1;
		}
		//USART_Transmit('t');
	}
	TWCR |=(1<<TWINT);
} 

void LoadControlSum(void)
{
	unsigned char R0;
	RegTransmit[0]=0;
	for(R0=1;R0<=10;++R0)
		RegTransmit[0] +=RegTransmit[R0];
}

void LoadRegTransmit(void)
{
	//   unsigned char R0;
	//	++TestTwi;    
	//    for(R0=1;R0<=10;++R0)
	RegTransmit[7]=TestTWI;
	RegTransmit[6]=3;
	RegTransmit[5]=4;
	RegTransmit[4]=15;
	RegTransmit[3]=16;
	RegTransmit[2]=17;
	RegTransmit[1]=RegS;//TestTWI;
	/*    RegTransmit[7]=RegTransmitUsart[7];
	RegTransmit[6]=RegTransmitUsart[8];
	RegTransmit[5]=RegTransmitUsart[9];
	RegTransmit[4]=RegTransmitUsart[10];
	RegTransmit[3]=RegTransmitUsart[11];
	RegTransmit[2]=RegTransmitUsart[12];*/
}


void HexDec(unsigned int R2)
{
	unsigned int R0;
	unsigned char R1;
	
	R0=R2;

	R1=0;
	while(R0>=100)
	{
		R0 -=100;
		++R1;
	}	
	Data10[0]=16+R1;//0x7c;
	R1=0;
	while(R0>=10)
	{
		R0 -=10;
		++R1;
	}	
	Data10[1]=16+R1;
	Data10[2]=16+R0;
}

void IndicatorTest(void)
{		
	
	clear_lcd();
	
	HexDec(RomReceive[2]);
	if(Data10[0]>16)
	{
		index_char[0]=Data10[0];
		index_char[1]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[1]=Data10[1];
		
	index_char[2]=Data10[2];
	
	HexDec(RomReceive[3]);
	if(Data10[0]>16)
	{
		index_char[3]=Data10[0];
		index_char[4]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[4]=Data10[1];
		
	index_char[5]=Data10[2];
	
	HexDec(RomReceive[4]);
	if(Data10[0]>16)
	{
		index_char[6]=Data10[0];
		index_char[7]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[7]=Data10[1];
		
	index_char[8]=Data10[2];
	
	HexDec(RomReceive[5]);
	if(Data10[0]>16)
	{
		index_char[9]=Data10[0];
		index_char[10]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[10]=Data10[1];
		
	index_char[11]=Data10[2];
	
	HexDec(RomReceive[7]);
	if(Data10[0]>16)
	{
		index_char[12]=Data10[0];
		index_char[13]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[13]=Data10[1];
		
	index_char[14]=Data10[2];
	
	HexDec(RomReceive[8]);
	if(Data10[0]>16)
	{
		index_char[15]=Data10[0];
		index_char[16]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[16]=Data10[1];
		
	index_char[17]=Data10[2];
	
	HexDec(RomReceive[9]);
	if(Data10[0]>16)
	{
		index_char[18]=Data10[0];
		index_char[19]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[19]=Data10[1];
		
	index_char[20]=Data10[2];
	
	HexDec(RomReceive[10]);
	if(Data10[0]>16)
	{
		index_char[21]=Data10[0];
		index_char[22]=Data10[1];
	}
	else
		if(Data10[1]>16)
			index_char[22]=Data10[1];
		
	index_char[23]=Data10[2];
	
	load_led_reg();
}	
	
//==================================================================================
//==================================================================================
//==================================================================================
	
void main(void) __attribute__((__noreturn__));
	
//int main(void)
void main(void)
{
	uint8_t cnt_b = 0, CtStart=0; 
	
	DDRA = 0x1f;


	DDRB = 0xA4;
	PORTB = 0x00;

	DDRC = 0x00;
	PORTC = 0x03;//poll up SDA SCL

	DDRD = 0x00;
	PORTD = 0x00;/*imp=off,Reset=off*/
	
	TCCR1A = 0;
	TCCR1B = 0x02;
	
	TCCR0 = 3;// 2ms
	TIMSK = 0x05;//enable int overlow T0 T1
	
	/*InitCpi*/
	SPCR = 0x55;//1mhz
	SPSR = 0x01;


	//INIT TWI;
	//TWBR = 0x3f;//F TWI
	TWBR = 0x7f;//F TWI
	//TWAR = (NumberLink<<1) + 4;
	TWAR = 4;
	TWCR = (1<<TWEA) | (1<<TWEN) | (1<<TWIE);

	uart_38400();
	
	wdt_enable( WDTO_500MS );
	
	clear_lcd();
	
	while( cnt_b < 24 )
	{
		index_char[ cnt_b ] = 6; //FF
		++cnt_b;
	}
	load_led_reg();
	
	
	_delay_ms(100);
	sei();
	wdt_reset();

	
	CtErrorLink[0]=CtErrorLink0[0];
	CtErrorLink[1]=CtErrorLink0[1];
	
	Error=0;
	NumberIndicator=2;
	
	USART_Transmit('!');
	USART_Transmit('\n');
	USART_Transmit('\r');
	
	// Show_led_str((PGM_P)KHU,0,7);
	// Show_led_str((PGM_P)NIOBII,1,9);
	
	RomReceive[1]=2; 
	RomReceive[2]=16;//3;  
	RomReceive[3]=0; 
	RomReceive[4]=0;    
	RomReceive[5]=0; 
	RomReceive[6]=0;  
	RomReceive[7]=0; 
	RomReceive[8]=0;  
	RomReceive[9]=0; 
	RomReceive[10]=0; 
	
	// RomReceive[1]=0; 
	// RomReceive[2]=16;//3;  
	// RomReceive[3]=150; 
	// RomReceive[4]=1;    
	// RomReceive[5]=255; 
	// RomReceive[6]=5;  
	// RomReceive[7]=6; 
	// RomReceive[8]=7;  
	// RomReceive[9]=8; 
	// RomReceive[10]=9;
	
	cnt_b=0;
	
	while(1)
	{
		wdt_reset();
		ReadKn();
	//	_delay_ms(50);
		
		if(Error & 0x80)
		{
			if(sec>1)
			{
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)CHSN,1,7);
				sec=0;
			}
		}
		else 
		if(!RomReceive[1])
		{
			if(sec05)
			{
				clear_lcd();
				IndicatorTest();
				sec05=0;
			}
		}
		else 
		if(sec>1)
		{
			sec=0;		
			
			switch( RomReceive[ NumberIndicator ] )
			{
			case 1:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_1,1,9);

				break;
			case 2:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_2,1,9);

				break;

			case 3:
				clear_lcd();
				Show_led_str((PGM_P)GOTOVNOST,0,11);
				Show_led_str((PGM_P)DU,1,7);
				break;
			case 4:
				clear_lcd();
				Show_led_str((PGM_P)WORK,0,9);
				
				break;
			case 5:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_3,1,9);
				break;

			case 6:
				clear_lcd();
				Show_led_str((PGM_P)NO_SVYAZ,0,10);
				Show_led_str((PGM_P)S_SHU,1,8);
				break;
			case 7:
				clear_lcd();
				Show_led_str((PGM_P)NO_SVYAZ,0,10);
				Show_led_str((PGM_P)S_MSHU,1,8);
				
				break;
			case 8:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)CHSN,1,7);
			
				break;
			case 9:
				clear_lcd();
				Show_led_str((PGM_P)REGIM_MSHU,0,10);

				break;

			case 10:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_4,1,9);

				break;
			case 11:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_5,1,9);

				break;

			case 12:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_DG1,1,9);

				break;
			case 13:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_DG2,1,9);

				break;
			case 14:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_LR_DG1,1,9);

				break;
			case 15:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_LR_DG2,1,9);

				break;
			case 16:
				clear_lcd();
				Show_led_str((PGM_P)GOTOVNOST,0,11);
				Show_led_str((PGM_P)MU,1,7);
				
				break;
			case 17:
				clear_lcd();
				Show_led_str((PGM_P)GOTOVNOST,0,11);
				break;
			case 18:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)FVU,1,7);
				
				break;
			case 19:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_PCH1,1,10);

				break;
			case 20:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)LUKA_PCH2,1,10);

				break;
			case 21:
				clear_lcd();
				Show_led_str((PGM_P)OTKAZ,0,8);
				Show_led_str((PGM_P)DT,1,6);
				
				break;

			default:
				break;
			}
		
			++NumberIndicator;
			while(!RomReceive[ NumberIndicator ])
				if(NumberIndicator<=9)
				{
					++NumberIndicator;
				}
				else
				{
					NumberIndicator=2;
					break;
				}
		}
		
		if(EnableLoad)
		{
			LoadRegTransmit();
			LoadControlSum();
		}
		
		// if(TWCR & 0x80)
		// {
			// ++TestTWI;
			// ReceiveTransmitSlave();
		// }
	
		if(!CtErrorLink[0])//ErrorLink;
		{
			//	++TestTwi;
			//INIT TWI;
			TWBR = 0;//F TWI
			TWAR = 0;
			TWCR = 0;
			TWSR = 0xf8;
			CtStart = 50;
			while(CtStart--)	
				wdt_reset();
			TWBR = 0x7f;//F TWI
			TWAR = 4;
			TWCR = (1<<TWEA)|(1<<TWEN);
			//   TWCR |=(1<<TWINT);
			CtErrorLink[0]=CtErrorLink0[0];

			CtStart=200;
			while(CtStart--)
				wdt_reset();
		}
	}
}
	
	
 SIGNAL(SIG_OVERFLOW1)/*65ms*/
	{
		static uint8_t cnt=0;
		
		if( cnt>15 )
		{
			cnt = 0;
			++sec;
			++sec05;
		}
		else
		{
			++cnt;
			if(cnt==7)
				++sec05;
		}
		
		
		if( CtErrorLink[1] )
			--CtErrorLink[1];
		else
			Error |= 0x80;
	}
	
 SIGNAL(SIG_OVERFLOW0)//2ms
	{
		static uint8_t 	cnt_row = 0;
		uint8_t reg = 21;
		
		LCD_DIS;
		LCD_ALL_ROW_OFF;
		
		while( reg )
		{
			--reg;			
			SPDR = led_reg[reg][cnt_row];
			while(!(SPSR & (1<<SPIF)));
		}
		
		LCD_EN;
		
		switch( cnt_row )
		{
			case 0:
				LCD_ROW1_ON;
				break;
			case 1:
				LCD_ROW2_ON;
				break;
			case 2:
				LCD_ROW3_ON;
				break;
			case 3:
				LCD_ROW4_ON;
				break;
			case 4:
				LCD_ROW5_ON;
				break;
			default: break;
		}


		if( cnt_row < 4 )
			++cnt_row;
		else
			cnt_row = 0;
	}
	
SIGNAL(SIG_2WIRE_SERIAL)
{
	++TestTWI;
	if((TWSR & 0xf8)==0x60)//Adr Slave Receive
	{
		//USART_Transmit('A');
		CtReceive=10;
	}
	else if((TWSR & 0xf8)==0x80)//Data Slave Receive
	{
		RamReceive[ CtReceive ] = TWDR;
		if(CtReceive)
		{
			--CtReceive;
		}
		else
		{
			//	++TestTwi;
			LoadRomReceive();
		}
	}
	else if((TWSR & 0xf8)==0xA8)//Adr Slave Transmit
	{

		CtTransmit=10;
		EnableLoad=0;
		TWDR=RegTransmit[CtTransmit];
		//USART_Transmit('T');
	}
	else if((TWSR & 0xf8)==0xb8)//Data Slave Transmit
	{
		if(CtTransmit)
		{
			--CtTransmit;
			TWDR=RegTransmit[CtTransmit];
		}
		else
		{
			TWDR=RegTransmit[CtTransmit];
			EnableLoad=1;
		}
		//USART_Transmit('t');
	}
	TWCR |=(1<<TWINT);
}	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
