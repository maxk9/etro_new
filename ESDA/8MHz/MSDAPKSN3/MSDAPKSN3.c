
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
    unsigned    char            CtTimeClose[2];
    unsigned    char            CtTimeOpen[2];
    const unsigned    char      CtTimeClose0=15;
    const unsigned    char      CtTimeOpen0=15;



    unsigned char TestTWI;//GlobalError;
    unsigned char GlobalError;
    unsigned char SmallError;
    unsigned char Work;

    unsigned char Error;
	volatile    unsigned char CtEnableKn;

    static const unsigned  char  NumberLink=1;

	volatile    unsigned char PORTDD;

    unsigned char ErrorLink;

    unsigned char TestTWI;

    unsigned char CtSicle;

    unsigned int Reg0;
    unsigned    char            TestTransmit;
    unsigned    char            CtKn;
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

 	unsigned    int	CtError[5];
 	const unsigned    int CtError0[5]={50,500,500,500,500};

    unsigned char RegimError;

    unsigned int CtInitInd;
    unsigned int RegInt0;

	unsigned    int    Reg0;
	unsigned    char   Reg1;
    unsigned    char    SregTemp;   
	unsigned    char    IndData[6];

	unsigned    int	RegS;
	unsigned    int	RegSTemp;
	unsigned    int	RegSOld;
	unsigned    int	RegSWork;

       
    /*============================================*/
      void    ReadKn(void)
    {

    unsigned    char R0;
    unsigned    char R2;
	PORTB &=0xbf;//PB6=0
    R0=RegSTemp;



    if(bit_is_clear(PINA,PA1))
    R0 |=0x1;
	else
    R0 &=0xfe;
    if(bit_is_clear(PINA,PA2))
    R0 |=0x2;
	else
    R0 &=0xfd;
    if(bit_is_clear(PINA,PA3))
    R0 |=0x4;
	else
    R0 &=0xfb;
    if(bit_is_clear(PINA,PA4))
    R0 |=0x8;
	else
    R0 &=0xf7;
    if(bit_is_clear(PINB,PB0))
    R0 |=0x10;
	else
    R0 &=0xef;	
		 
    if(R0==RegSTemp)
		{
	RegSOld=RegS;
    RegS=R0;

	for(R2=0;R2<=4;++R2)
			{
	if((RegS & 0x10)&&(!(RegSOld & 0x10)))
				{
	RegSWork |=0x10;
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
    RomReceive[R0]=RamReceive[R0];
	CtErrorLink=CtErrorLink0;
	CtError[0]=CtError0[0];
	Error &=0xfe;
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
	++TestTWI;//
   RegTransmit[3]=RegimWork;	
   RegTransmit[4]=TestTWI;//PINA;//RegS>>8;
   RegTransmit[5]=Error;//11;//RegS>>8;
   RegTransmit[6]=RegS;//PORTC;//22;
    } 

	void ControlLed(void)
	{

	PORTD |=0xb4;
	PORTA |=0x61;
 	if(RegimWork & 8)
	PORTD &=0xbf;//bl zs
	else
	PORTD |=0x40;


	}
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=4;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}

	}	
	void	ResetError(void)
	{
	unsigned char R0;

	Error=0;
	for(R0=0;R0<=4;++R0)
	CtError[R0]=CtError0[R0];

	}
	

		
	void ControlRevun(void)
	{
	if(RegimWork & 8)
	PORTC |=0x40;
	else if(RomReceive[3] & 0x40)
	PORTC &=0xbf;
	else if(Error & 0x1e)
	PORTC &=0xbf;
	else
	PORTC |=0x40;
//	PORTC |=0x40;

	}		

	void ControlRegim(void)
	{	

	if(RegSWork & 0x10)//Blok ZS
		{
	if(RegimWork & 8)		
	RegimWork &=0xfff7;
	else
	RegimWork |=8;
	RegSWork &=0xffef;
		}
	if(RegS & 0x8)//Vent=Avt
	RegimWork |=0x10;
	else if(RegS & 0x100)//Vent=hand
	RegimWork &=0xffef;
	}				
void	ControlPech(void)
	{
	if(RomReceive[1] & 0x10)
	PORTC &=0xdf;
	else if(RomReceive[2] & 0x10)
	PORTC |=0x20;		
	}
	void OpenLuk1(void)
	{
	PORTC |=8;//close=off
	if(Error & 2)
	PORTC |=4;
	else if(RegS & 1)
	PORTC |=4;	 	
	else if(CtTimeClose[0])
	PORTC |=4;		
	else
	PORTC &=0xfb;
	}



	void CloseLuk1(void)
	{
	PORTC |=4;//on=off
	if(Error & 4)
	PORTC |=8;
	else if(RegS & 2)
	PORTC |=8;	 	
	else if(CtTimeOpen[0])//Pauza after open
	PORTC |=8;		
	else
	PORTC &=0xf7;
	}

	void OpenLuk2(void)
	{
	PORTB |=8;//Off=off
	if(Error & 8)
	PORTC |=0x10;
	else if(RegS & 4)
	PORTC |=0x10;	 	
	else if(CtTimeClose[1])
	PORTC |=0x10;		
	else
	PORTC &=0xef;
	}
		
	void CloseLuk2(void)
	{
	PORTC |=0x10;//on=off
	if(Error & 0x10)
	PORTB |=8;
	else if(RegS & 8)
	PORTB |=8;	 	
	else if(CtTimeOpen[1])
	PORTB |=8;		
	else
	PORTB &=0xf7;
	}	
	void	SetRegimError(void)
	{
	unsigned char R0;
	R0=0;
	if(!(PORTC & 4))//Luk1 on=on
	R0|=2;
	if(!(PORTC & 8))//Luk1 off=on
	R0 |=4;
	if(!(PORTC & 0x10))//Luk2 on=on
	R0 |=8;
	if(!(PORTB & 8))//Luk2 off=on
	R0 |=0x10;
	RegimError=R0;
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



		
	for(Arg0=1;Arg0<=10;++Arg0)
	RomReceive[Arg0]=0;


	CtEeprom=1;

	RegimWork=0x10;//avt vent
	CtErrorLink=CtErrorLink0;
	CtKn=5;

			
	ResetError();	
//WORK	
	           
      while(1)
    {
	_WDR();
	if(RomReceive[3] & 0x1)
	ResetError();
	SetError();
	SetRegimError();


	ControlRegim();
	ControlPech();
	if(RomReceive[1] & 1)
	OpenLuk1();
	else if(RomReceive[2] & 1)
	CloseLuk1();
	else
	PORTC |=0xc;
	if(RomReceive[1] & 2)
	OpenLuk2();
	else	if(RomReceive[2] & 2)
	CloseLuk2();
	else
			{
	PORTC |=0x10;
	PORTB |=0x8;
			}



	ControlLed();
	ControlRevun();
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
	unsigned char R0;
	if(CtKn)
	--CtKn;


	if(CtEnableKn)
	--CtEnableKn;
	else
	CtEnableKn=3;
	switch(CtEnableKn)
		{

	case 0:ReadKn();

	break;
	case 1:	PORTB &=0xdf;
	PORTB |=0x40;

	break;
	case 2:ReadKn();


	break;
	case 3:	PORTB &=0xbf;
	PORTB |=0x20;

	break;
	default :CtEnableKn=3;
	break;
		}


	++TestTWI;


    if(CtErrorLink)
    --CtErrorLink;
	if(CtError[0])
	--CtError[0];

    for(R0=1;R0<=4;++R0)
	    {
	if(RegimError &(1<<R0))
			{

    if(CtError[R0])
    --CtError[R0];
			}
	else
	CtError[R0]=CtError0[R0];

		}
	if(!(PORTC & 8))//Luk1=(off=on)
	CtTimeClose[0]=CtTimeClose0;//Pauza after close
	else if(CtTimeClose[0])
	--CtTimeClose[0];
	if(!(PORTC & 4))//Luk1=(on=on)
	CtTimeOpen[0]=CtTimeOpen0;
	else if(CtTimeOpen[0])
	--CtTimeOpen[0];

	if(!(PORTB & 8))//Luk2=(off=on)
	CtTimeClose[1]=CtTimeClose0;
	else if(CtTimeClose[1])
	--CtTimeClose[1];
	if(!(PORTC & 0x10))//Luk2=(on=on)
	CtTimeOpen[1]=CtTimeOpen0;
	else if(CtTimeOpen[1])
	--CtTimeOpen[1];
 

    }



