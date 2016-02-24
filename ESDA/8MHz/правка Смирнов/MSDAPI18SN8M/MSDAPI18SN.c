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


const char GotovMu[] PROGMEM={" √Œ“Œ¬ÕŒ—“‹ Ã”  "};
const char GotovSystem[] PROGMEM={"   √Œ“Œ¬ÕŒ—“‹   "};
const unsigned char OffZashita[] PROGMEM={"«¿Ÿ»“¿ Œ“ Àﬁ◊≈Õ¿ "};
const unsigned char ErrorLuk1[] PROGMEM={"  Œ“ ¿« Àﬁ ¿ 1  "};
const unsigned char ErrorLuk2[] PROGMEM={"  Œ“ ¿« Àﬁ ¿ 2  "};
const unsigned char ErrorLuk3[] PROGMEM={"  Œ“ ¿« Àﬁ ¿ 3  "};
const unsigned char ErrorLuk4[] PROGMEM={"  Œ“ ¿« Àﬁ ¿ 4  "};
const unsigned char ErrorLuk5[] PROGMEM={"  Œ“ ¿« Àﬁ ¿ 5  "};
const unsigned char ErrorLG1[] PROGMEM= {"  Œ“ ¿« À√ ƒ√ 1 "};
const unsigned char ErrorLG2[] PROGMEM= {"  Œ“ ¿« À√ ƒ√ 2 "};
const unsigned char ErrorLR1[] PROGMEM= {"  Œ“ ¿« ÀP ƒ√ 1 "};
const unsigned char ErrorLR2[] PROGMEM= {"  Œ“ ¿« ÀP ƒ√ 2  "};
const unsigned char ErrorLPCH1[] PROGMEM= {"Œ“ ¿« Àﬁ ¿ œ◊ 1 "};
const unsigned char ErrorLPCH2[] PROGMEM= {"Œ“ ¿« Àﬁ ¿ œ◊ 2 "};
const unsigned char Izol[] PROGMEM={"Õ»« ¿ﬂ »«ŒÀﬂ÷»ﬂ   "};
const unsigned char OtkazDT[] PROGMEM={"  Œ“ ¿« ƒ“      "};
     unsigned    int             CtInd;
    unsigned    int             CtBegin;
    unsigned    char             TestTwi;
    unsigned    int             TestInd;
volatile    unsigned    char             NumberIndicator;
    unsigned    int             CtChangeInd;
    unsigned    char             ClearBit;
    unsigned    char             CtErrorLink[2];
	const   unsigned    char     CtErrorLink0[2]={20,200};
    unsigned    char             Error;
/*
	7	No link
*/
	unsigned    char            RegS;
	unsigned    char            RegSTemp;
	unsigned    char            Data10[3];
	unsigned    char            TestTWI;
    static const unsigned  char  NumberLink=0;
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;
  	unsigned    char            RomReceive[12];
 	unsigned    char            RegTransmit[12];
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=5000;//500;


    unsigned int CtStart;
    unsigned    char            EnableLoad;

 	volatile	unsigned    char    CtTest;


 	volatile	unsigned    int    CtErrorReceive;

	const unsigned    int    CtErrorReceive0=10000; 
 
	volatile unsigned    char    OverlowT1;
	volatile 	unsigned    int	            Alfa;

	    
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
    while(R0--)         _WDR();
    SPDR=0x1f;
    R0=100;
    while(R0--)         _WDR();
    SPDR=0x24;
    R0=100;
    while(R0--)         _WDR();
    SPDR=R2;
    R0=100;
    while(R0--)         _WDR();
    SPDR=0;
    R0=100;
    while(R0--)         _WDR();
    SPDR=R1;
    R0=100;
    while(R0--)         _WDR();
    SPDR=0;

    }


    void    InitIndStart(void)
    {
    unsigned long R0;
    PORTB &=0xfe;
    R0=300000;//780000;
    while(R0--)         _WDR();
    PORTB |=0x1;
    R0=5000;
    while(R0--)         _WDR();
    SPDR=0x1b;
    R0=5000;
    while(R0--)         _WDR();


    SPDR=0x74;
    R0=5000;
    while(R0--)         _WDR();
    SPDR=17;//Kirilica 

    R0=5000;
    while(R0--)         _WDR();     
    SPDR=0xc;//clear ind
     
    } 
void    TransmitInd(unsigned char R1)
{
    unsigned int R0;  
    R0=400;

    if(R1>=192)
    R1=R1-64;
    while(R0--)         _WDR();	   
    SPDR=R1;           
 }










   void    InitInd(void)
    {
    unsigned int R0;
    PORTB &=0xfe;
    R0=65;
    while(R0--)         _WDR();
    PORTB |=0x1;
    R0=3000;
    while(R0--)         _WDR();
    SPDR=0x1b;
    R0=1500;
    while(R0--)         _WDR();


    SPDR=0x74;
    R0=100;
    while(R0--)         _WDR();
    SPDR=17;//Kirilica 

    R0=100;
    while(R0--)         _WDR();     
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
    while(R0--)         _WDR(); 
	SPDR=Data10[0];
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=Data10[1];
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=Data10[2];
		}
	SetCursor(0,1);
	for(R1=6;R1<=10;++R1)
		{
	HexDec(RomReceive[R1]);
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=Data10[0];
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=Data10[1];
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=Data10[2];
		}

	}	
	void Ind19U6(void)
	{
const unsigned char R2[]=" ÿ” 19”6";
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=3;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=0x20;
		}

	for(R1=0;R1<=7;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 

    R3=R2[R1];
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	for(R1=0;R1<=3;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	SPDR=0x20;
		}
	}
//1		
	void IndOtkazLuk1(void)
	{

    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLuk1[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}

	}
//2	
	void IndOtkazLuk2(void)
	{

    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLuk2[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}

	}
//3			
	void IndGotovDU(void)
	{
const unsigned char R2[]="  √Œ“Œ¬ÕŒ—“‹ ƒ” ";
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
//4
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
//5
	void IndOtkazLuk3(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLuk3[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
//6
	void IndNoLinkSHU(void)
	{
const unsigned char R2[]="Õ≈“ —¬ﬂ«» — ÿ”   ";
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

//7		
	void IndNoLinkMSHU(void)
	{
const unsigned char R2[]="Õ≈“ —¬ﬂ«» — ÃŸ”  ";
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
//8
	void IndOtkazSHSN(void)
	{
const unsigned char R2[]="   OTKA3 ÿ—Õ    ";
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
//9	
	void IndRegimMSHU(void)
	{
const unsigned char R2[]="  –≈∆»Ã ÃŸ”    ";
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
	void IndOtkazLuk4(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLuk4[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}	
	
		
//11	
	void IndOtkazLuk5(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLuk5[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
		
//12
	void IndOtkazLukDG1(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLG1[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
//13
	void IndOtkazLukDG2(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLG2[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
//14
	void IndOtkazLukLR1(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLR1[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}	
//15	
	void IndOtkazLukLR2(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLR2[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}	
//16	
	void IndGotovMU(void)
	{


    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);
	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&GotovMu[R1]);

    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}

//17	
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
//18	
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
//19
	void IndOtkazLukPCH1(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLPCH1[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
//20
	void IndOtkazLukPCH2(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&ErrorLPCH2[R1]);
    if(R3>=192)
    R3=R3-64;
	SPDR=R3;
		}
	}
//21
	void IndOtkazDT(void)
	{
    unsigned int R0;
    unsigned char R1;
    unsigned char R3;
	SetCursor(0,0);

	for(R1=0;R1<=15;++R1)
		{		
    R0=500;
    while(R0--)         _WDR(); 
	R3=pgm_read_byte_near(&OtkazDT[R1]);
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
   RomReceive[2]=17;//3;  
  RomReceive[3]=0; 
   RomReceive[4]=0;    
   RomReceive[5]=0; 
   RomReceive[6]=0;  
  RomReceive[7]=0; 
   RomReceive[8]=0;  
   RomReceive[9]=0; 
   RomReceive[10]=0;  
 	CtInd=0;//50;     	 
    while(1)
	    {
    _WDR();




	ReadKn();
    if(!RomReceive[1])
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
	IndOtkazLuk1();

	break;
	case 2:
	IndOtkazLuk2();

	break;

	case 3:
	IndGotovDU();
	break;
	case 4:
	IndRabota();
	break;
	case 5:
	IndOtkazLuk3();
	break;

	case 6:
	IndNoLinkSHU();
	break;
	case 7:
	IndNoLinkMSHU();
	break;
	case 8:
	IndOtkazSHSN();
	break;
	case 9:
	IndRegimMSHU();
	break;

	case 10:
	IndOtkazLuk4();

	break;
	case 11:
	IndOtkazLuk5();

	break;

	case 12:
	IndOtkazLukDG1();

	break;
	case 13:
	IndOtkazLukDG2();

	break;
	case 14:
	IndOtkazLukLR1();

	break;
	case 15:
	IndOtkazLukLR2();

	break;
	case 16:
	IndGotovMU();
	break;
	case 17:
	IndGotovSystem();
	break;
	case 18:
	IndOtkazFVU();
	break;
	case 19:
	IndOtkazLukPCH1();

	break;
	case 20:
	IndOtkazLukPCH2();

	break;
	case 21:
	IndOtkazDT();
	break;

	default:
	break;


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
