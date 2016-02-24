/*TWI Slave   
WDTON CKOPT
BOODLEVEL BODEN SUT1 CKSEL2*/    
//ÓÚÓ·‡ÊÂÌËÂ ÒËÏ‚ÓÎÓ‚ ËÁ Font
#include <avr/boot.h>
#include <compat/ina90.h>
#include <avr/iom8535.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>
//===================================================
const char GotovSystem[] PROGMEM=        {"   √Œ“Œ¬ÕŒ—“‹   "};
const char NoTopliva[] PROGMEM=          {"  Õ≈“ “ŒœÀ»¬¿   "};
const unsigned char T8[] PROGMEM=        {"   “Œ∆ < 8 C    "};
const unsigned char T37[] PROGMEM=       {"   “Œ∆ < 37 C   "};
const char GotovMu[] PROGMEM=            {" √Œ“Œ¬ÕŒ—“‹ Ã”  "};
const unsigned char OffZashita[] PROGMEM={"«¿Ÿ»“¿ Œ“ Àﬁ◊≈Õ¿"};
const unsigned char ErrorLuk[] PROGMEM=  {"  Œ“ ¿« Àﬁ ¿    "};
const unsigned char Izol[] PROGMEM=      {"Õ»« ¿ﬂ »«ŒÀﬂ÷»ﬂ "};
unsigned int	CtInitInd;
unsigned int	CtInd;
unsigned int	CtBegin;
unsigned char	TestTwi;
unsigned int	TestInd;
volatile unsigned char	NumberIndicator;
unsigned int	CtChangeInd;
unsigned char	ClearBit;
unsigned char	CtErrorLink[2];
const unsigned char	CtErrorLink0[2]={20,200};
unsigned char	Error;
/*
	7	No link
*/
unsigned char	RegS;
unsigned char	RegSTemp;
unsigned char	Data10[3];
unsigned char	TestTWI;
static const unsigned  char  NumberLink=0;
unsigned char	CtTransmit;
unsigned char	CtReceive;
unsigned char	RomReceive[12];
unsigned char	RegTransmit[12];
unsigned int	CtTWCR;
unsigned char	RamReceive[12];
const unsigned int	CtTWCR0=5000;//500;


unsigned int CtStart;
unsigned char EnableLoad;

volatile unsigned char CtTest;


volatile unsigned int CtErrorReceive;

const unsigned int CtErrorReceive0=10000; 

volatile unsigned char OverlowT1;
volatile unsigned int Alfa;


//	volatile  unsigned  char     RegInd[43];//data 0...41
unsigned    char    Test;









void LoadRomReceive(void)
{
	unsigned char R0;
	unsigned char R1;           
	R0=0;
	for(R1=1;R1<=10;++R1)
	R0 +=RamReceive[R1];
	if(R0==RamReceive[0])
	{
		++TestTwi;
		for(R0=1;R0<=10;++R0)
		RomReceive[R0]=RamReceive[R0];

		CtErrorLink[0]=CtErrorLink0[0];
		CtErrorLink[1]=CtErrorLink0[1];
	}
}  

void ReceiveTransmitSlave(void)
{

	if((TWSR & 0xf8)==0x60)//Adr Slave Receive
	{
		//	++TestTwi;
		CtReceive=10;
	}
	else    if((TWSR & 0xf8)==0x80)//Data Slave Receive
	{
		//	TestTwi=CtReceive;
		RamReceive[CtReceive]=TWDR;
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
void    LoadControlSum(void)
{
	unsigned char R0;
	RegTransmit[0]=0;
	for(R0=1;R0<=10;++R0)
	RegTransmit[0] +=RegTransmit[R0];
}  
void    LoadRegTransmit(void)
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
	Data10[0]=0x30+R1;//0x7c;
	R1=0;
	while(R0>=10)
	{
		R0 -=10;
		++R1;
	}	
	Data10[1]=0x30+R1;//0x7c;
	Data10[2]=0x30+R0;//0x7c;	
}	



void    ReadKn(void)
{


	unsigned    char R0;

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






















void	SetCursor(unsigned char R2,unsigned char R1)
{
	unsigned int R0;  
	R0=500;
	while(R0--) _WDR();
	SPDR=0x1f;
	R0=100;
	while(R0--) _WDR();
	SPDR=0x24;
	R0=100;
	while(R0--) _WDR();
	SPDR=R2;
	R0=100;
	while(R0--) _WDR();
	SPDR=0;
	R0=100;
	while(R0--) _WDR();
	SPDR=R1;
	R0=100;
	while(R0--) _WDR();
	SPDR=0;

}


void    InitIndStart(void)
{
	unsigned long R0;
	PORTB &=0xfe;
	R0=780000;
	while(R0--) _WDR();
	PORTB |=0x1;
	R0=5000;
	while(R0--) _WDR();
	SPDR=0x1b;
	R0=5000;
	while(R0--) _WDR();


	SPDR=0x74;
	R0=5000;
	while(R0--) _WDR();
	SPDR=17;//Kirilica 

	R0=5000;
	while(R0--) _WDR();     
	SPDR=0xc;//clear ind
	
} 
void    TransmitInd(unsigned char R1)
{
	unsigned int R0;  
	R0=400;

	if(R1>=192)
		R1=R1-64;
	while(R0--) _WDR();	   
	SPDR=R1;           
}










void    InitInd(void)
{
	unsigned int R0;
	PORTB &=0xfe;
	R0=65;
	while(R0--) _WDR();
	PORTB |=0x1;
	R0=3000;
	while(R0--) _WDR();
	SPDR=0x1b;
	R0=1500;
	while(R0--) _WDR();


	SPDR=0x74;
	R0=100;
	while(R0--) _WDR();
	SPDR=17;//Kirilica 

	R0=100;
	while(R0--) _WDR();     
	SPDR=0xc;//clear ind
	
}


void IndicatorTest(void)
{
	unsigned char R1;
	unsigned int R0;		
	SetCursor(0,0);
	for(R1=1;R1<=5;++R1)
	{
		HexDec(RomReceive[R1]);
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[0];
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[1];
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[2];
	}
	SetCursor(0,1);
	for(R1=6;R1<=10;++R1)
	{
		HexDec(RomReceive[R1]);
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[0];
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[1];
		R0=500;
		while(R0--) _WDR(); 
		SPDR=Data10[2];
	}
}	

void IndSDA(void)
{
	const unsigned char R2[]=" ÿ” ›—ƒ¿";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=3;++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
			SPDR=0x20;
	}

	for(R1=0;R1<=7; ++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 

		R3=R2[R1];
		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
	for(R1=0; R1<=3; ++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
			SPDR=0x20;
	}
}


//1		
void IndNoTopliva(void)
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);

	for(R1=0; R1<=15; ++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
		R3=pgm_read_byte_near(&NoTopliva[R1]);
		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//2	
void IndT8Gr(void)
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);

	for(R1=0; R1<=15; ++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
		R3 = pgm_read_byte_near(&T8[R1]);

		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//3			
void IndT37Gr(void)	
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
		R3=pgm_read_byte_near(&T37[R1]);

		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}	
//4
void IndZashitaOff(void)
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
		R3=pgm_read_byte_near(&OffZashita[R1]);

		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//5
void IndOtkazLuk(void)
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 
		R3=pgm_read_byte_near(&ErrorLuk[R1]);

		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//6
void IndLowIzol(void)
{

	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{
		R0=500;
		while(R0--) _WDR(); 
		R3=pgm_read_byte_near(&Izol[R1]);

		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//7
void IndNoPusk(void)
{
	const unsigned char R2[]="    HET œ”— ¿   ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 

		R3=R2[R1];
		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}
//8		
void IndNoLinkMSHU(void)
{
	const unsigned char R2[]="Õ≈“ —¬ﬂ«» — ÃŸ” ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0; R1<=15; ++R1)
	{		
		R0=500;
		while(R0--) _WDR(); 

		R3=R2[R1];
		if(R3>=192)
			R3=R3-64;
		SPDR=R3;
	}
}	
//9
void IndNoOborotov(void)
{
	const unsigned char R2[]="  Õ≈“ Œ¡Œ–Œ“Œ¬  ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}
//10	
void IndNoVozb(void)
{
	const unsigned char R2[]="Õ≈“ ¬Œ«¡”∆ƒ≈Õ»ﬂ  ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}	
//11	
void IndErrorU(void)
{
	const unsigned char R2[]="    U HE HOPMA   ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}	
//12
void IndPeregruzKG(void)
{
	const unsigned char R2[]=" œ≈–≈√–”« ¿ ¬√   ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}
//13
void IndErrorKG(void)
{
	const unsigned char R2[]="   Œ“ ¿« ¬√      ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}

//15	
void IndNoLinkRU(void)
{
	const unsigned char R2[]=" Õ≈“ —¬ﬂ«» — –Õ  ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}

}	
//16	

void IndGotovSystem(void)
{


	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 
		R3=pgm_read_byte_near(&GotovSystem[R1]);

		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}

//19	
void IndPogar(void)
{
	const unsigned char R2[]="    œŒ∆¿–         ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}	
//20	
void IndErrorKS(void)
{
	const unsigned char R2[]="  Œ“ ¿« ¬—       ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}
//21	
void IndUKorpus(void)
{
	const unsigned char R2[]="  U Õ¿  Œ–œ”—≈   ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}	

//23
void IndErrorSet(void)
{
	const unsigned char R2[]=" —≈“‹ Õ≈ ÕŒ–Ã¿   ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}
//24	
void IndNoMasla(void)
{
	const unsigned char R2[]=" Õ≈“ Ã¿—À¿      ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}

//25	
void IndDozapravkaTopliva(void)
{
	const unsigned char R2[]="ƒŒ«¿œ–. “ŒœÀ»¬¿ ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}		

//26
void IndDozapravkaMasla(void)
{
	const unsigned char R2[]="ƒŒ«¿œ–¿¬ ¿ Ã¿—À¿ ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}

//27	
void IndOtkazFVU(void)
{
	const unsigned char R2[]="  Œ“ ¿« ‘¬”     ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}

}

//28	
void IndAvarDvig(void)
{
	const unsigned char R2[]={0x80,0x82,0x80,0x90,0x88,0x9f,0x20,0x84,0x82,0x88,0x83,0x80,0x92,0x85,0x8b,0x9f};//"¿¬¿–»ﬂ ƒ¬»√¿“≈Àﬂ ";



	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}
}
//29	


//31	
void IndRabota(void)
{
	const unsigned char R2[]="    –¿¡Œ“¿       ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}

}

//32	
void IndNoLinkSN(void)
{
	const unsigned char R2[]="Õ≈“ —¬ﬂ«» — ÿ—Õ ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}

}
//33	
void IndOtkazPZA(void)
{
	const unsigned char R2[]="   Œ“ ¿« ”œœ¿    ";
	unsigned int R0;
	unsigned char R1;
	unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
	{		
		R0=500;
		while(R0--)         _WDR(); 

		R3=R2[R1];
		if(R3>=192)
		R3=R3-64;
		SPDR=R3;
	}

}	

/*++++++++++++++++++++++++++++++++++++++++++*/



int main(void)
{
	DDRA=0xff;


	DDRB=0xb1;
	PORTB |= 0xff;// Control UAB

	DDRC=0xfc;
	PORTC |=0xff;/*VT3=off*/

	DDRD=0xbf;
	PORTD=0xff;/*imp=off,Reset=off*/

	//  INIT SPI
	SPSR=0;//f/64
	SPCR=0x70;

	InitIndStart();
	TIFR |=0x40;//reset flag
	TIMSK |=0x40;
	TCCR2=0x3;

	TCCR0=5;// /1024
	TIMSK |=0x1;//enable int overlow T0


	//INIT TWI;
	TWBR=0x7f;//F TWI
	TWAR=(NumberLink<<1)+4;   
	TWCR =(1<<TWEA)|(1<<TWEN);

	CtStart=200;
	while(CtStart--)	_WDR();




	CtErrorLink[0]=CtErrorLink0[0];
	CtErrorLink[1]=CtErrorLink0[1];
	Error=0;
	NumberIndicator=2;
	_SEI();

	CtBegin=250;    
	/*Work program*/ 
	RomReceive[1]=2; 
	RomReceive[2]=29;//3;  
	RomReceive[3]=0; 
	RomReceive[4]=0;    
	RomReceive[5]=0; 
	RomReceive[6]=0;  
	RomReceive[7]=0; 
	RomReceive[8]=0;  
	RomReceive[9]=0; 
	RomReceive[10]=0;  
	CtInd=50;
	CtInitInd=300;     	 
	while(1)
	{
		_WDR();

		if(!CtInitInd)
		{
			InitInd();
			CtInitInd=300;
		}


		ReadKn();
		if(CtBegin)
		{
			if(!CtInd)
			{
				IndSDA();
				CtInd=50;
			}
		}
		else if(!RomReceive[1])
		{
			if(!CtInd)
			{
				IndicatorTest();
				CtInd=50;
			}
		}
		else if(!CtInd)
		{

			CtInd=50;


			switch(RomReceive[NumberIndicator])
			{

			case 1:
				IndNoTopliva();
				break;
			case 2:
				IndT8Gr();
				break;
			case 3:
				IndT37Gr();
				break;

			case 4:
				IndZashitaOff();
				break;
			case 5:
				IndOtkazLuk();
				break;

			case 6:
				IndLowIzol();
				break;
			case 7:
				IndNoPusk();
				break;
			case 8:
				IndNoLinkMSHU();
				break;

			case 9:
				IndNoOborotov();
				break;
			case 11:
				IndErrorU();
				break;
			case 12:
				IndPeregruzKG();
				break;
			case 13:
				IndErrorKG();
				break;

			case 15:
				IndNoLinkRU();
				break;


			case 17:
				IndPeregruzKG();
				break;


			case 19:
				IndPogar();
				break;

			case 20:
				IndErrorKS();
				break;

			case 21:
				IndUKorpus();
				break;
			case 22:
				IndOtkazLuk();
				break;
			case 23:
				IndErrorSet();
				break;
			case 24:
				IndNoMasla();
				break;
			case 25:
				IndDozapravkaTopliva();
				break;
			case 26:
				IndDozapravkaMasla();
				break;
			case 27:
				IndOtkazFVU();
				break;
			case 28:
				IndAvarDvig();
				break;
			case 29:
				IndGotovSystem();
				break;


			case 31:
				IndRabota();
				break;


			case 32:
				IndNoLinkSN();
				break;
			case 33:
				IndOtkazPZA();
				break;
			default:break;

			}
		}
		if(EnableLoad)
		{
			LoadRegTransmit();
			LoadControlSum();
		}
		if(TWCR & 0x80)
		{
			++TestTWI;
			ReceiveTransmitSlave();

		}

		if(!CtErrorLink[0])//ErrorLink;
		{
			//	++TestTwi;
			//INIT TWI;
			TWBR=0;//F TWI
			TWAR=0;
			TWCR =0;
			TWSR=0xf8;
			CtStart=50;
			while(CtStart--)	_WDR();
			TWBR=0x7f;//F TWI
			TWAR=(NumberLink<<1)+4;
			TWCR =(1<<TWEA)|(1<<TWEN);
			//   TWCR |=(1<<TWINT);
			CtErrorLink[0]=CtErrorLink0[0];

			CtStart=200;
			while(CtStart--)	_WDR();
		}


	}


}




SIGNAL(SIG_OVERFLOW0)//16ms
{
	if(CtInitInd)
	--CtInitInd;
	if(CtInd)
	--CtInd;
	if(CtBegin)
	--CtBegin;
	if(CtErrorLink[0])
	--CtErrorLink[0];

	if(CtErrorLink[1])
	--CtErrorLink[1];
	else
	Error |=0x80;

}
SIGNAL(SIG_OVERFLOW2)/*128mks*/
{
	if(CtChangeInd)
	--CtChangeInd;
	else
	{
		if(TestInd)
		--TestInd;
		else
		TestInd=999;
		CtChangeInd=5000;
M1:	if(NumberIndicator<=9)
		++NumberIndicator;
		else
		NumberIndicator=2;
		if((!RomReceive[NumberIndicator])&&(NumberIndicator !=2))
		goto M1;
	}

}
