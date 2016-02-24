
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

    unsigned char CtErrorTransmit;
    unsigned char TestTwi;
    unsigned char GlobalError;
    unsigned char SmallError;
    unsigned char Work;

    unsigned char Error;
	volatile    unsigned char CtEnableKn;
    const unsigned char CtErrorRS0=100;
    unsigned char CtErrorRS;
    static const unsigned  char  NumberLink=1;
    const unsigned char NumberBlok=0;
volatile    unsigned char PORTDD;
    unsigned char NumberReceiveRS;
    unsigned char ErrorLink;
    unsigned char CtUsart;


    const unsigned char CtTransmitRS0=6;
    unsigned char RegTransmitRS[7];
    unsigned char RamReceiveRS[8];
    unsigned char RomReceiveRS[7][3];
    const unsigned int CtErrorLinkRS0=500;
    unsigned char	ControlTransmitRS;
    unsigned char ControlReceiveRS;

	volatile unsigned char CtTransmitRS;
	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=7;
    unsigned int CtErrorLinkRS;






    unsigned char TestTWI;


    unsigned char TestRS3;
    unsigned char TestRS2;
    unsigned char TestRS1;
    unsigned char TestRS;
    unsigned char CtSicle;



    unsigned int UA;
    unsigned int UB;
    unsigned int UC;


    unsigned int UNom;
    unsigned int Reg0;





    unsigned    char            TestTransmit;
    unsigned    char            CtKn;
    unsigned    int            ArgL0;
    unsigned    int            ArgL1;
    unsigned    int            RegTG;
    unsigned    char            EndAd;
    unsigned    char            CtTG;
    unsigned    char            EnableLoad;
    unsigned    char            TestLink;
    unsigned    char            CtPusk;
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;
 	unsigned    char            RomReceive[12];
 	unsigned    char            RegTransmit[12];
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=5000;//500;
    unsigned    char            RegimWork;




    unsigned int RegInt0;
   unsigned int RegInt1;

    unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=10;


    unsigned    char            RegIntCap1;
    unsigned    char            CtOverLow;

    unsigned int CtStart;
    unsigned char CtEeprom;

 	unsigned    int	CtError[4];
 	const unsigned    int CtError0=100;
 	const unsigned    int CtError0FVU=1000;
    unsigned char RegimError;

    unsigned int CtInitInd;
    unsigned int RegInt0;

	unsigned    int    Reg0;
	unsigned    char   Reg1;
    unsigned    char    SregTemp;   
	unsigned    char    IndData[6];
	unsigned    char	NumberAd;
    unsigned    char	CtAd;

	static const unsigned    char	    CtAd0=33;//3.2msec
	unsigned    long	AdTemp;
 	unsigned    int	AdResult;
	unsigned    int	RegS;
	unsigned    int	RegSTemp;
	unsigned    int	RegSOld;
	unsigned    int	RegSWork;
    unsigned    int              CtUst;
    static const    unsigned int        CtUst0=10;
       
    /*============================================*/
     void    ReadKnPB5(void)
    {

    unsigned    int R0;


    R0=RegSTemp & 0xffe0;


    if(bit_is_clear(PINB,PB7))
    R0 |=0x1;

    if(bit_is_clear(PINB,PB0))
    R0 |=0x2;

    if(bit_is_clear(PINB,PB1))
    R0 |=0x4;

    if(bit_is_clear(PINB,PB4))
    R0 |=0x8;

    if(bit_is_clear(PINA,PA7))
    R0 |=0x10;

	RegSTemp=R0;
	}


     void    ReadKnPB6(void)
    {

    unsigned    int R0;
    unsigned    char R2;

    R0=RegSTemp & 0x1f;



    if(bit_is_clear(PINB,PB7))
    R0 |=0x20;

    if(bit_is_clear(PINB,PB0))
    R0 |=0x40;

    if(bit_is_clear(PINB,PB1))
    R0 |=0x80;

    if(bit_is_clear(PINB,PB4))
    R0 |=0x100;

    if(bit_is_clear(PINA,PA7))
    R0 |=0x200;


    if(bit_is_clear(PINA,PA1))
    R0 |=0x400;

    if(bit_is_clear(PINA,PA2))
    R0 |=0x800;

    if(bit_is_clear(PINA,PA3))
    R0 |=0x1000;

    if(bit_is_clear(PINA,PA4))
    R0 |=0x2000;
	 
    if(R0==RegSTemp)
		{
	RegSOld=RegS;
    RegS=R0;

	for(R2=0;R2<=13;++R2)
			{
	if((RegS & (1<<R2))&&(!(RegSOld & (1<<R2))))
				{
	RegSWork |=(1<<R2);
	CtKn=30;
				}
			}
		}
	if(!CtKn)
	RegSWork=0;
    RegSTemp=R0;
 

 
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
    for(R0=1;R0<=10;++R0)
				{
    RomReceive[R0]=RamReceive[R0];
	RamReceive[R0]=1;
				}
	RamReceive[0]=0xff;
	CtErrorLink=CtErrorLink0;

        }
   
    }
 unsigned char WaitTWCR(void)
    {
   while(!(TWCR & 0x80))
        {
   _WDR();

    if(CtTWCR)
    --CtTWCR;
    else
            {
    CtTWCR=CtTWCR0;
    return 0;
            }
        }
    CtTWCR=CtTWCR0;
    return 1;
    }   
    void ReceiveTransmitSlave(void)
    {

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
    void    LoadControlSum(void)
    {
    unsigned char R0;
    RegTransmit[0]=0;
    for(R0=1;R0<=10;++R0)
    RegTransmit[0] +=RegTransmit[R0];
    }



    void    LoadRegTransmit(void)
    {
	unsigned char R0;
	unsigned char R1;

	
	R0=RomReceiveRS[2][1];
	R1=RomReceiveRS[2][2];

   RegTransmit[1]=RomReceiveRS[1][1];
	if(R0 & 1)
   RegTransmit[2]=RomReceiveRS[2][1];
	else if(R1 & 1)
   RegTransmit[2]=RomReceiveRS[2][2];
	else
   RegTransmit[2]=RomReceiveRS[2][2] & 0xf;
   RegTransmit[3]=RegimWork	& 0x1f;
   if(RegS & 0x2000)
   			{
   RegTransmit[3] |= 0x20;//MU
	RegTransmit[2] =0;
	if(RegimWork & 4)//FVU
	RegTransmit[2] |=2;

			}

   RegTransmit[4]=RegS>>8;
   RegTransmit[5]=RegTransmit[2];//11;//RegS>>8;
   RegTransmit[6]=Error;//PORTC;//22;
    } 




  	void	ResetErrorLinkRS(void)
{
	unsigned char R0;
	R0=UDR;//UDR1;
	UCSRA &=0x22;//reset error


	CtReceiveRS=CtReceiveRS0;
}
 	void	InitUsart(void)
	{	

   UCSRA=0;
   UCSRA |=1;//only 9bit=1
   UCSRB=0x1c;//enable transmit 9bit
   UBRRH=0;
   UBRRL=103;

	UCSRC=(1<<7)|(1<<2)|(1<<1);

   UCSRB |=0x80;//enable int receive

   PORTD &=0xf7;//Out Transmit=off


	}	
	void	TransmitUsart(void)
	{
	volatile unsigned char R0;

   PORTD |=0x8;//Out Transmit=on 

   R0=100;
   while(R0)
   --R0;
	UCSRA =1;//Receive only 9 bit=1
	R0=NumberBlok;

     while(!(UCSRA & 0x20))
   _WDR();
    UCSRB |=1;//9 bit=1
    UDR=R0;//LinkRS;
	ControlTransmitRS =NumberBlok;



			
	}		  
	void	LinkRS(void)
{

	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;
	volatile unsigned char R4;
	unsigned int R5;
	PORTD &= 0xf7;//Transmit denable
	CtErrorTransmit=5;
	if(UCSRA &(1<<RXC))
	{

	R1=UCSRA;
	R2=UCSRB;
	R3=UCSRC;
	R0=UDR;

	RamReceiveRS[CtReceiveRS]=R0;
	if(R1 & 0x1c)//error
		{

	ResetErrorLinkRS();

	return;
		}



	if(R2 & 2)
		{

	NumberReceiveRS=R0;
	if(R0>2)
		{
	CtUsart=(NumberBlok<<1)+7;
	return;
		}



	R4=NumberReceiveRS-NumberBlok;

	R4 <<=1;
	R4=~R4;
	CtUsart=R4+7;




	ControlReceiveRS =R0;
	CtReceiveRS=CtReceiveRS0-1;
	UCSRA &=0xfe;
	UCSRB &=0xfe;
	if(NumberReceiveRS==NumberBlok)
			{
	PORTD |= 0x8;//Transmit enable

	R0=100;
	while(R0)
	--R0;
	R0=RegTransmitRS[CtReceiveRS];
     while(!(UCSRA & 0x20))
   _WDR();


	UCSRB &=0xfe;
    UDR=R0;
	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
			}
	return;
		}


	if(CtReceiveRS)
		{

	
					
	ControlReceiveRS +=R0;
	--CtReceiveRS;
	if(NumberReceiveRS==NumberBlok)
			{

	PORTD |= 0x8;//Transmit enable

     while(!(UCSRA & 0x20))
   _WDR();
   if(!CtReceiveRS)
   				{
	UCSRB &=0xfe;
    UDR=ControlTransmitRS;

				}
	else
				{
	R0=RegTransmitRS[CtReceiveRS];
	UCSRB &=0xfe;
    UDR=R0;
	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
				}
			}
		}
	else
		{

	UCSRA |=1;
	PORTD &= 0xf7;//Transmit denable


	if(R0==ControlReceiveRS)
				{



	CtError[NumberReceiveRS]=CtError0;
	if(NumberReceiveRS != NumberBlok)
	CtErrorRS=20;

	R5=0;
	R5 |=1<<(NumberReceiveRS+8);
	R5=~R5;
	Error &=R5;

	for(R4=1;R4<=6;++R4)
	RomReceiveRS[R4][NumberReceiveRS]=RamReceiveRS[R4];


				}

		}

	}
}
	void ControlLed(void)
	{

	PORTD |=4;
	PORTA |=0x40;
 	if(RegimWork & 1)
	PORTD &=0xdf;
	else
	PORTD |=0x20;
	   
	if(RegimWork & 4)
	PORTD &=0x7f;
	else
	PORTD |=0x80;

	if(RegimWork & 0x10)
		{
	PORTA &=0xdf;
	PORTA |=0x40;
		}
	else
		{
	PORTA &=0xbf;
	PORTA |=0x20;
		}

 	if(RegimWork & 2)
	PORTD &=0xbf;
	else
	PORTD |=0x40;

 	if(RegimWork & 8)
	PORTA &=0xfe;
	else
	PORTA |=0x1;

	}
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=3;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}

	}	
	void	ResetError(void)
	{
	unsigned char R0;

	Error=0;
	for(R0=0;R0<=3;++R0)
	CtError[R0]=CtError0;

	}
	
	void ControlRU(void)
	{
	PORTC |=0xc;
	}
	void ControlFVU(void)
	{


	if(RegS & 0x2000)//MU
		{
	RegTransmit[1]=0;
//	RegTransmit[2]=0;
	if(RegimWork & 4)
	PORTC &=0xef;
	else
	PORTC |=0x10;
		}
	else
		{
	if(RegTransmit[2] & 2)
	PORTC &=0xef;
	else
	PORTC |=0x10;
		}
	if((PORTC & 0x10)||(!(RegS & 0x800)))
			{
	CtError[3]=CtError0FVU;
	Error &=0xf7;
			}
	}		
	void ControlRevun(void)
	{
	if(RegimWork & 8)
	PORTC |=0x40;
	else if(RomReceive[3] & 0x40)
	PORTC &=0xbf;
	else
	PORTC |=0x40;
	}		
	void ControlReleOpenLuk(void)
	{
	if(RomReceive[3] & 0x80)
	PORTC |=0x80;

	else
	PORTC &=0x7f;
	}
	void ControlOgnetush(void)
	{

	if(RegTransmit[2] & 4)//&&(!RomReceive[5]))
	PORTB &=0xf7;
	else
	PORTB |=0x8;


	}	
	void ControlPCH(void)
	{
	if(RegS & 0x2000)//MU
		{
	if(RomReceive[4] & 1)
	PORTC &=0xdf;
	else
	PORTC |=0x20;
		}
	else
		{
	if((RomReceiveRS[4][1] & 3)||(RomReceiveRS[4][2] & 3))
		
	PORTC &=0xdf;
	else
	PORTC |=0x20;
		}
	}
	void	LoadRegRS(void)
	{
	Work=0;
	GlobalError=0;	
	SmallError=0;		
		
	RegTransmitRS[1]=RomReceive[1];
	if(RegS & 0x400)
	RegTransmitRS[1] |=0x80;
	else
	RegTransmitRS[1] &=0x7f;
/*
0	DG1 work
1	DG2 work
2	PCH1 work
3	PCH2 work
*/
	RegTransmitRS[2]=RomReceive[2];
	RegTransmitRS[3]=SmallError;//CtEnableKn;//RegSTemp;
	RegTransmitRS[4]=RomReceive[4];	
	}
	void ControlRegim(void)
	{	
	if(RegSWork & 0x2)//Blok PCH
		{
	if(RegimWork & 1)		
	RegimWork &=0xfffe;
	else
	RegimWork |=1;
	RegSWork &=0xfffd;
		}	
	if(RegSWork & 0x40)//Blok U PCH
		{
	if(RegimWork & 2)		
	RegimWork &=0xfffd;
	else
	RegimWork |=2;
	RegSWork &=0xffbf;
		}
	if(RegSWork & 0x4)//FVU
		{
	if(RegimWork & 4)		
	RegimWork &=0xfffb;
	else
	RegimWork |=4;
	RegSWork &=0xfffb;
		}
	if(RegSWork & 0x80)//Blok ZS
		{
	if(RegimWork & 8)		
	RegimWork &=0xfff7;
	else
	RegimWork |=8;
	RegSWork &=0xff7f;
		}
	if(RegS & 0x8)//Vent=Avt
	RegimWork |=0x10;
	else if(RegS & 0x100)//Vent=hand
	RegimWork &=0xffef;
	}				
																					        
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	unsigned char Arg0;
 



//if DDRX,X=1 PORTX,X=OUT
    DDRA=0x61;
	PORTA = 0xff;//0xc0 prov

    DDRB=0x68;
	PORTB = 0xff;

    DDRC=0xfc;
    PORTC =0xff;

    DDRD=0xfe;
    PORTD=0xf7;
	TCCR2=0;
	TCCR1A=0;
	TCCR0=0;
//INIT TWI;
    TWBR=0x7f;//F TWI


    TCCR1B=0x2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1
    TWAR=(NumberLink<<1)+4;   
   TWCR =(1<<TWEA)|(1<<TWEN);

   _SEI();


    CtUst=CtUst0;
		
	for(Arg0=1;Arg0<=10;++Arg0)
	RomReceive[Arg0]=0;


	CtEeprom=1;

	RegimWork=0x10;//avt vent
	CtErrorLink=CtErrorLink0;
	CtKn=5;

	UNom=280;
	InitUsart();  
	CtErrorRS=CtErrorRS0;			
	ResetError();	
//WORK	
	           
      while(1)
    {
	_WDR();
	if(RomReceive[3] & 0x20)
	ResetError();
	LoadRegRS();
	SetError();
	ControlRegim();
	ControlLed();

	ControlRU();

	ControlFVU();
	ControlRevun();
	ControlReleOpenLuk();
	ControlOgnetush();
	ControlPCH();

//	PORTD &=0xa;





	if(EnableLoad)
		{
    LoadRegTransmit();
    LoadControlSum();
		}
	if(TWCR & 0x80)
			{
    ReceiveTransmitSlave();
	++TestTransmit;
			}
	if(!CtErrorLink)//ErrorLink;
		{

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
	CtErrorLink=CtErrorLink0;

    CtStart=200;
	while(CtStart--)	_WDR();
		}



				

    }
  
}

 SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
    {

	if(CtKn)
	--CtKn;


	if(CtEnableKn)
	--CtEnableKn;
	else
	CtEnableKn=3;
	switch(CtEnableKn)
		{

	case 0:ReadKnPB5();

	break;
	case 1:	PORTB &=0xdf;
	PORTB |=0x40;

	break;
	case 2:ReadKnPB6();


	break;
	case 3:	PORTB &=0xbf;
	PORTB |=0x20;

	break;
	default :CtEnableKn=3;
	break;
		}
	if((PORTD & 8)&&(CtErrorTransmit))
	--CtErrorTransmit;
	if(!CtErrorTransmit)
		{
	PORTD &=0xf7;
	CtErrorTransmit=5;
		}

	if(CtErrorRS)
	--CtErrorRS;
	else
		{
	InitUsart();
	CtErrorRS=50;
		}
	if(CtUsart)
	--CtUsart;
	else
		{


	TransmitUsart();
	CtUsart=(NumberBlok<<3)+7;
		}



	++TestTWI;


    if(CtErrorLink)
    --CtErrorLink;



    for(RegInt1=0;RegInt1<=3;++RegInt1)
	    {

    if(CtError[RegInt1])
    --CtError[RegInt1];
		}

 

    }
	SIGNAL(SIG_UART_RECV)
	{

	LinkRS();
	}


