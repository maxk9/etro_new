
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
void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
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
		
		TCCR1A = 0;
		TCCR1B = 0x02;
		
		TCCR0 = 3;// 2ms
		TIMSK = 0x05;//enable int overlow T0 T1
		ASSR = 0;
		
		/*InitCpi*/
		SPCR = 0x55;//1mhz
		SPSR = 0x01;


		//INIT TWI;
		TWBR = 0x7f;//F TWI
		//TWAR = (NumberLink<<1) + 4;
		TWAR = 4;
		TWCR =(1<<TWEA) | (1<<TWEN);

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1	

		USART_Init ( MYUBRR );
		
		CtStart=200;
		while(CtStart--)
			wdt_reset();
		
		USART_Transmit('!');
			
		while(1)
		{
			// LCD_ALL_ROW_OFF;
			// _delay_ms(100);
			// wdt_reset();
			 // LCD_ROW1_ON;
			USART_Transmit('A');
			 _delay_ms(100);
			wdt_reset();
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
			
			
	// M1:		if(NumberIndicator<=9)
				// ++NumberIndicator;
			// else
				// NumberIndicator=2;
			
			// if((!RomReceive[NumberIndicator])&&(NumberIndicator !=2))
				// goto M1;
		}

		//++TestTWI;
				
		
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
	}
	
	
	
