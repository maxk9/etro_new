
	/*TWI Slave*/

	#include <io.h>
	#include <stdint.h>
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#define F_CPU 8000000UL  // 8MHz
	#include <util/delay.h>
	//#include <eeprom.h>

	#include "font5x7.h"
	
//==================================================================================

#define  LCD_DIS 		( PORTB &= ~4 )
#define  LCD_EN 		( PORTB |= 4 )
#define  LCD_ALL_ROW_OFF ( PORTA &= ~0x1F )
							
							
#define  LCD_ROW1_ON 	( PORTA |= 1 )
#define  LCD_ROW2_ON 	( PORTA |= 2 )
#define  LCD_ROW3_ON 	( PORTA |= 4 )					
#define  LCD_ROW4_ON 	( PORTA |= 8 )
#define  LCD_ROW5_ON 	( PORTA |= 16 )




const uint8_t   LG19U6[]    = {0, 0,0 ,0 ,17,25,52  ,22 } ,
				LGNIOBII[]  = {0, 0,0 ,46,41,47,34  ,41   ,42 },
				KHU[]       = {0, 0,0 ,0 ,43,57,52 },
				NO[]        = {0, 0,0 ,0 ,46,38,51 },
				MASLA[]     = {0,0 ,0 ,45,33,50,44  ,33 } ,
				TOPLIVA[]   = {0, 0,51,47,48,44,41  ,35   ,33} ,
				PUSKA[]     = {0, 0,0 ,48,52,50,43  ,33 } ,
				OBOROTOV[]  = {0, 0,47,34,47,49,47  ,51   ,47  ,35 },
				VOZB[]      = {0, 0,0 ,0 ,35,47,40  ,34   ,14 },
				OBRAT[]     = {0, 0,0 ,47,34,49,33  ,51 } ,
				MOCH[]      = {0, 0,0 ,45,47,58,46 },
				FIRE[]      = {0, 0,0 ,48,47,39,33  ,49  },
				DOZAPRAVKA[]= {0,37,47,40,33,48,49  ,33   ,35  ,43  ,33},
				NIZKAYA[]   = {0,0 ,0 ,46,41,40,43  ,33   ,64} ,
				ISOLYACIA[] = {0,0 ,41,40,47,44,64  ,55   ,41  ,64} ,
				AVARIA[]    = {0,0 ,0 ,33,35,33,49  ,41   ,64} ,
				DVIGATELA[] = {0,37,35,41,36,33,51  ,38   ,44  ,64} ,
				NO_NORMA[]  = {0,0 ,46,38,0 ,46,47  ,49   ,45  ,33} ,
				NEISPRAV[]  = {0,0 ,46,38,41,50,48  ,49   ,33  ,35  ,14},
				VG[]        = {0,0 ,0 ,0 ,0 ,35,36 },
				VS[]        = {0,0 ,0 ,0 ,0 ,35,50 },
				FVU[]       = {0,0 ,0 ,0 ,53,35,52 },
				UPPA[]      = {0,0 ,0 ,0 ,52,48,48  ,33}  ,
				LUKA[]      = {0,0 ,0 ,0 ,44,63,43  ,33}  ,
				NA_KORP[]   = {0,0 ,46,33,0 ,43,47  ,49   ,48  ,14} ,
				GOTOV[]     = {0,0 ,0 ,36,47,51,47  ,35   ,46} ,
				WORK[]      = {0,0 ,0 ,49,33,34,47  ,51   ,33} ,
				TOJ_8[]     = {0,0 ,0 ,51,47,39,28  ,24   ,50} ,
				TOJ_37[]    = {0,0 ,0 ,51,47,39,28  ,19   ,23  ,50} ,
				PROTECT[]   = {0,0 ,0 ,40,33,58,41  ,51   ,33} ,
				OTKL[]      = {0,0 ,0 ,0 ,47,51,43  ,44}  ,
				SYSTEM[]    = {0,0 ,50,41,50,51,38  ,45   ,60} ,
				SETI[]      = {0,0 ,0 ,0 ,50,38,51  ,41}  ,
				MU[]        = {0,0 ,0 ,0 ,0 ,45,52 },
				DU[]        = {0,0 ,0 ,0 ,0 ,37,52 },
				OBRYV[]     = {0,0 ,0 ,47,34,49,60  ,35}  ,
				SVYAZ_SN[]  = {0,0 ,50,35,64,40,14  ,57   ,50  ,46} ,
				SVYAZ_MSHU[]= {0,0 ,50,35,64,40,14  ,45   ,58  ,52} ,
				SVYAZ_RN[]  = {0,0 ,50,35,64,40,14  ,49   ,46} ,
				PEREGRUZ[]  = {0,0 ,48,38,49,38,36  ,49   ,52  ,40};
				
				
				


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
					CtBegin = 0;
					
	const unsigned int	CtTWCR0=5000;//500;

	const uint16_t		CtErrorReceive0=10000;
	uint8_t	OverlowT1;
	
	uint8_t	index_char[24];
	uint8_t	led_reg[23][5];
	//uint8_t led_rg[24];


	
	
//==================================================================================	
static void uart_9600(void)
{
	#define BAUD 9600
	#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |= (1 << U2X);
	#else
	UCSRA &= ~(1 << U2X);
	#endif

	/* Enable receiver and transmitter */
	//UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRB =(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);
}
//==================================================================================	
void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
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
			bit8 = 0;
			
	uint16_t index_font=0;
	
	while( row < 5 )
	{
		reg = 0;
		digit = 0;
		while( reg < 21 )
		{
			led_reg[reg][row] = 0;
			
			bit8 = 0;
			while( bit8 < 7 )
			{
				++bit8;
				index_font = (uint16_t)index_char[ digit ]*5 + row;
				led_reg[ reg ][ row ] |= font5x7[ index_font ]&(1<<(8-bit8));
				//led_reg[ reg ][ row ] |= ((font5x7[index_font] >> (8-bit8))&1)<<bit8;
			}
			++digit;
			
			index_font = (uint16_t)index_char[ digit ]*5 + row;
			led_reg[ reg ][ row ] |= font5x7[index_font]&(1<<7);
			
			++reg;
		}
		++row;
	}
	
	// row = 0;
	// reg = 0;
	// while( row < 5 )
	// {
		// USART_Transmit('0'+row);
		// USART_Transmit(' ');
		
		// while( reg < 21 )
		// {
			// USART_Transmit(led_reg[ reg ][ row ]);
			// USART_Transmit(' ');
			// ++reg;			
		// }
		// USART_Transmit('\n');
	// ++row;
	// }
}

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
	void LoadRomReceive(void)
	{
		unsigned char R0;
		unsigned char R1;
		R0=0;
		for(R1=1;R1<=10;++R1)
		R0 +=RamReceive[R1];
		if(R0==RamReceive[0])
		{
			//	++TestTWI;
			for(R0=1;R0<=10;++R0)
			RomReceive[R0]=RamReceive[R0];
			CtErrorLink[0]=CtErrorLink0[0];
			CtErrorLink[1]=CtErrorLink0[1];
		}

	}

	void ReceiveTransmitSlave(void)
	{
		static uint8_t CtReceive =0, CtTransmit = 0;
		
		if((TWSR & 0xf8)==0x60)//Adr Slave Receive
		{

			CtReceive=10;


		}
		else    if((TWSR & 0xf8)==0x80)//Data Slave Receive
		{

			RamReceive[CtReceive]=TWDR;
			if(CtReceive)
			{

				--CtReceive;

			}
			else
			{
				LoadRomReceive();

			}
		}
		else if((TWSR & 0xf8)==0xA8)//Adr Slave Transmit
		{

			CtTransmit=10;
			EnableLoad=0;
			TWDR=RegTransmit[CtTransmit];


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
		if(R1)
			Data10[0]=16+R1;//0x7c;
		else
			Data10[0]=0;
		
		R1=0;
		while(R0>=10)
		{
			R0 -=10;
			++R1;
		}	
		
		if(R1)
			Data10[0]=16+R1;//0x7c;
		else
			Data10[0]=0;
		
		Data10[2]=16+R0;//0x7c;	
	}	


	void Load0_999(unsigned char R0,unsigned int R1)
	{
		unsigned int R2;
		R2=R1;//Data
		HexDec(R2);
		index_char[R0]=Data10[0];
		index_char[R0+1]=Data10[1];
		index_char[R0+2]=Data10[2];
	}

	void ReadKn(void)
	{
		static uint8_t RegSTemp =0;
		unsigned char R0;

		R0=RegSTemp;//Temp;
		if(bit_is_clear(PINB,PB2))
			R0 |=0x1;
		else
			R0 &=0xfe;
		if(bit_is_clear(PINB,PB3))
			R0 |=0x2;
		else
			R0 &=0xfd;
		if(bit_is_clear(PINB,PB4))
			R0 |=0x4;
		else
			R0 &=0xfb;

		if(R0==RegSTemp)
			RegS=R0;
		RegSTemp=R0;	
	}

	void IndicatorTest(void)
	{	
	Load0_999(1,RomReceive[1]); 
	Load0_999(5,RomReceive[2]);
	Load0_999(9,RomReceive[3]); 
	Load0_999(13,RomReceive[4]);
	Load0_999(17,RomReceive[5]); 
	
	Load0_999(22,RomReceive[6]);
	Load0_999(26,RomReceive[7]); 
	Load0_999(30,RomReceive[8]);
	Load0_999(34,RomReceive[9]); 
	Load0_999(38,RomReceive[10]);	
	}

void Show_led_str( const uint8_t *str, uint8_t n_str, uint8_t len )
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
			index_char[ cnt_b ] = str[ cnt_b ];
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
			index_char[ cnt_b ] = str[i];
			++cnt_b;
			++i;
		}
	}

	load_led_reg();
}



//============================================================================================
//============================================================================================
//============================================================================================


	int main(void)
	{
		uint8_t cnt_b = 0, CtStart=0; 
		
		DDRA = 0x3f;


		DDRB = 0xA4;
		PORTB = 0x00;

		DDRC = 0x00;
		PORTC = 0x00;

		DDRD = 0x00;
		PORTD = 0x00;/*imp=off,Reset=off*/

		//ICR1 = 400;//20KH
		TCCR1A = 0;
		TCCR1B = 0x02;

		
		TCCR0 = 3;// 2ms
		TIMSK = 0x05;//enable int overlow T0 T1
		ASSR = 0;
		
		/*InitCpi*/
		SPCR = 0x55;//1mhz
		SPSR = 0x01;
		//SPDR=0x55;

		//INIT TWI;
		TWBR = 0x7f;//F TWI
		//TWAR = (NumberLink<<1) + 4;
		TWAR = 4;
		TWCR =(1<<TWEA) | (1<<TWEN);

	//	uart_9600();
		
		CtStart=200;
		while(CtStart--)
			wdt_reset();

		// while( cnt_b < 24 )
		// {
			// index_char[ cnt_b ] = 6; //FF
			// ++cnt_b;
		// }
		// load_led_reg();
		
		// cnt_b=255;
		// while(--cnt_b)
		// {
			// CtStart = 255;
			// while(CtStart--)
				// wdt_reset();
		// }
		
		CtErrorLink[0] = CtErrorLink0[0];
		CtErrorLink[1] = CtErrorLink0[1];
		Error = 0;
		NumberIndicator = 2;
		sei();
		//clear_lcd();
		CtBegin = 250;
		/*Work program*/
		RomReceive[1] = 2;  
		
		//Show_led_str( LG19U6,0,8 );

		//index_char[0] = 6;
		// index_char[0] = 1;
		// load_led_reg();

		led_reg[ 0 ][ 0 ]= 0x80;
		
		while(1)
		{
			wdt_reset();
			ReadKn();
			
			// _delay_ms(100);
			// SPDR = 0xAA;
			// while(!(SPSR & (1<<SPIF)));
			
			
			// if(CtBegin)
				// Show_led_str(LG19U6,0,8);
			// //	Ind19U6();
			// //	else if(RegS !=2)
			// //	IndNiobij();
			// else if(!RomReceive[1])
			// {
				// IndicatorTest();
			// }
			// else
			// {
				// //	if(RomReceive[NumberIndicator] !=19)
				// //	ClearDisplay2();
				// switch( RomReceive[ NumberIndicator ] )
				// {

				// case 1:
				// //	IndNoTopliva();
					// break;
				// case 2:
					// //IndT8Gr();
					// break;
				// case 3:
					// //IndT37Gr();
					// break;

				// case 4:
					// //IndZashitaOff();
					// break;
				// case 5:
					// //IndOtkazLuk();
					// break;

				// case 6:
					// //IndLowIzol();
					// break;
				// case 7:
					// //IndNoPusk();
					// break;
				// case 8:
					// //IndNoLinkMSHU();
					// break;

				// case 9:
					// //IndNoOborotov();
					// break;
				// case 10:
					// //IndNoVozb();
					// break;
				// case 11:
					// //IndErrorU();
					// break;
				// case 12:
					// //IndPeregruzKG();
					// break;
				// case 13:
					// //IndErrorKG();
					// break;

				// case 15:
					// //IndNoLinkRU();
					// break;
				// case 16:
					// //IndObratnajaP();
					// break;

				// case 17:
					// //IndPeregruzKG();
					// break;
				// case 18:


				// case 19:
					// //IndPogar();
					// break;

				// case 20:
					// //IndErrorKS();
					// break;

				// case 21:
					// //IndUKorpus();
					// break;
				// case 22:
					// //IndErrorLuk();
					// break;
				// case 23:
					// //IndErrorSet();
					// break;
				// case 24:
					// //IndNoMasla();
					// break;
				// case 25:
					// //IndDozapravkaTopliva();
					// break;
				// case 26:
					// //IndDozapravkaMasla();
					// break;
				// case 27:
					// //IndOtkazFVU();
					// break;
				// case 28:
					// //IndAvarDvig();
					// break;
				// case 29:
					// //IndGotovMU();
					// break;
				// case 30:
					// //IndGotovDU();
					// break;

				// case 31:
					// //IndRabota();
					// break;


				// case 32:
					// //IndNoLinkSN();
					// break;
				// case 33:
					// //IndOtkazPZA();
					// break;
				// }
			// }
			// if(EnableLoad)
			// {
				// LoadRegTransmit();
				// LoadControlSum();
			// }
			// if(TWCR & 0x80)
			// {
				// ReceiveTransmitSlave();
			// }

			// if(!CtErrorLink[0])//ErrorLink;
			// {
				// //INIT TWI;
				// TWBR=0;//F TWI
				// TWAR=0;
				// TWCR =0;
				// TWSR=0xf8;
				// CtStart=50;
				// while(CtStart--)	
					// wdt_reset();
				// TWBR=0x7f;//F TWI
				// //TWAR=(NumberLink<<1)+4;
				// TWAR = 4;
				// TWCR =(1<<TWEA)|(1<<TWEN);
				// //   TWCR |=(1<<TWINT);
				// CtErrorLink[0]=CtErrorLink0[0];

				// CtStart=200;
				// while(CtStart--)
					// wdt_reset();
			// }
		}
	}

	SIGNAL(SIG_OVERFLOW1)/*65ms*/
	{
		static uint16_t CtChangeInd = 0;
		
		if(CtBegin)
			--CtBegin;
		if(CtErrorLink[0])
			--CtErrorLink[0];

		if(CtErrorLink[1])
			--CtErrorLink[1];
		else
			Error |=0x80;

		
		
		
		
		if(CtChangeInd)
			--CtChangeInd;
		else
		{
			CtChangeInd=5000;
			
			
	M1:		if(NumberIndicator<=9)
				++NumberIndicator;
			else
				NumberIndicator=2;
			
			if((!RomReceive[NumberIndicator])&&(NumberIndicator !=2))
				goto M1;
		}

		++TestTWI;
				
		
	}
	SIGNAL(SIG_SPI)
	{


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
			while(!(SPSR & (1<<SPIF)));
			SPDR = led_reg[reg][cnt_row];
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
		
		// if( delay_count )
			// --delay_count;
	}



