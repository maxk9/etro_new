 //TWI Slave   
 //10.10.13 if KG2=on NU=on   
//with Int comparator=off 
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
    unsigned    char    RezervDiz2;
    unsigned    char    ErrorPZA;
    const unsigned    char    CtPZA0=100;
    unsigned    char    CtPZA;
    unsigned    char    ClearPZA;
    unsigned    char    SregTemp;

   unsigned char RomOld1;
   unsigned char RomOld;
   unsigned int CtNUOn;
   unsigned int CtNUOff;
   const unsigned int CtNUOn0=300;
   const unsigned int CtNUOff0=60;


   unsigned char CtNNom;//Maslo;
	unsigned int CtNU;
	unsigned int TestLamp;
   unsigned char CtMaslo;
    unsigned char TestStop;//CtErrorTransmit;
    unsigned char CtErrorTransmit;
    unsigned char TestTransmit;//RegStop1;
    volatile unsigned char RegimTransmit;
    volatile unsigned char CtEnableTransmit;

    volatile unsigned char CtErrorRSMin;
    volatile unsigned char TestRS;
    volatile unsigned char TestLink;
    volatile unsigned char RegStop1;
    volatile unsigned char TOG;
    unsigned char CtWellKn;
    unsigned char CtWellKn1;

    volatile unsigned char CtBegin;

    volatile unsigned char RegNumberBlok;
    volatile unsigned int NNom;
    volatile unsigned int NDiz;
    volatile unsigned char RegStop;



    volatile unsigned char EnableNNom;
    volatile unsigned int Error;
    volatile unsigned char NumberRegim;
	volatile    unsigned char CtStarterOn[4];
	volatile    unsigned char CtStarterOff[4];
    const unsigned char CtStarterOn0=100;
    const unsigned char CtStarterOff0=200;
	volatile    unsigned char NumberStarter;
	volatile    unsigned char CtEnableKn;
    volatile unsigned char CtErrorRS;

	volatile    unsigned char NumberBlok;
	volatile    unsigned char NumberReceiveRS;

	volatile    unsigned char CtUsart;


    volatile unsigned char RegTransmitRS[9];
    volatile unsigned char RamReceiveRS[9];
    volatile unsigned char RomReceiveRS[9][8];
    volatile unsigned char	ControlTransmitRS;
    volatile unsigned char ControlReceiveRS;


	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=8;



	const   unsigned    char   NumberLink=1;

    volatile unsigned    char            CtKn;
 

    volatile unsigned    char            EndAd;

    volatile unsigned    char            EnableLoad;
 
    volatile unsigned    char            CtPusk;
	volatile unsigned    char            CtTransmit;
	volatile unsigned    char            CtReceive;
 	volatile unsigned    char            RomReceive[13];
 	volatile unsigned    char            RegTransmit[13];

 	volatile unsigned    char            RamReceive[13];

    volatile unsigned    char            RegimWork;



    volatile unsigned    int             CtTime[9];
    const unsigned    int       CtTime0[8]={152,5500,61,459,5500,301,220,61};
    volatile unsigned    char            RegimTime;
/*
0	time Ostanov after N<150
1	Rasholagivanie
*/




    volatile unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=19;


    volatile unsigned char Regim;
    volatile unsigned int CtStart;


 	volatile unsigned    int	CtError[16];
 	const unsigned    int CtError0[16]={100,100,30000,60,
										300,100,100,100,100,100,100,100,100,100,100,100};

    volatile unsigned int RegimError;


	volatile 	unsigned    int	RegS;
	volatile 	unsigned int RegSTemp;
	volatile 	unsigned    char	RegS1;
	volatile 	unsigned char RegSTemp1;

    /*============================================*/
    void    WriteEeprom(unsigned char Arg4,unsigned char Arg5)
    {
    _WDR();
    while(bit_is_set(EECR,EEWE))
    ;
    while(bit_is_set(SPMCR,SPMEN))
    ;

	EEARL=Arg4;
	EEARH=0;
	EEDR=Arg5;
    SregTemp=SREG;
    _CLI();
	EECR |=(1<<EEMWE);
                       		 
	EECR |=(1<<EEWE);
    SREG=SregTemp;				
    while(bit_is_set(EECR,EEWE))
    ;

    while(bit_is_set(SPMCR,SPMEN))
    ;
}
    unsigned char    ReadEeprom(unsigned char Arg6)
	
    {		 	
	
	EEARH=0;
    EEARL=Arg6;
	EECR=EECR | (1<<EERE);

	while	(bit_is_set(EECR,EERE))
    ;

	return EEDR;
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
	R5=0;
	R5 |=1<<(NumberReceiveRS+8);
	R5=~R5;
	Error &=R5;
	if(R0>6)
		{
	CtUsart=(NumberBlok<<1)+14;
	return;
		}


	if(NumberBlok>NumberReceiveRS)
			{
	R4=NumberBlok-NumberReceiveRS;
	CtUsart=R4<<1;
			}
	else
			{
	R4=NumberReceiveRS-NumberBlok;

	R4 <<=1;
	R4=~R4;
	CtUsart=R4+15;

			}



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



	CtError[NumberReceiveRS+8]=CtError0[NumberReceiveRS+8];
	if(NumberReceiveRS != NumberBlok)
			{

	CtErrorRS=39;

			}
	R5=0;
	R5 |=1<<(NumberReceiveRS+8);
	R5=~R5;
	Error &=R5;

	for(R4=1;R4<=7;++R4)
					{
	RomReceiveRS[R4][NumberReceiveRS]=RamReceiveRS[R4];
	RamReceiveRS[R4]=1;
					}

				}



		}

	}
}
     void    ReadKnPA6(void)
    {
/* RegS
0	BUD=on
1	Peregrev KG
2	RBP
3	Rezervirovanie
4	KG=on
5	KSet=0n
6	Avtomat.Regim
7	Hand SHU
8	Hand MSHU
9	Mestnoe
10	Uroven Masla
11	Uroven Topliva high
12	Uroven Topliva sredniy
13	Uroven Topliva low	
14	Avtomat Zapravka
15	Avtomat Podogrev

RegS1

0	Blokirovka	Zashit
1	DT	EPG
2	Control	Zashit
3	Reset	Error
4	Pogar
5	PZA=Norma
6	R Izol	150k
7	R Izol  15k
*/
    unsigned    int R0;

    R0=RegSTemp & 0xfff3;
    if(bit_is_set(PIND,PD2))
    R0 |=0x4;
    if(bit_is_set(PIND,PD4))
    R0 |=0x8;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
	if(!CtWellKn)
    RegS=R0;
		}
 	RegSTemp=R0;
	}
	void ReadKnPA7(void)
	{
    unsigned    int R0;
 
    R0=RegSTemp & 0xffcf;
    if(bit_is_set(PIND,PD2))
    R0 |=0x10;
    if(bit_is_set(PIND,PD4))
    R0 |=0x20;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
	void ReadKnPC4(void)
	{
    unsigned    int R0;

    R0=RegSTemp & 0xff3f;
    if(bit_is_set(PIND,PD2))
    R0 |=0x40;
    if(bit_is_set(PIND,PD4))
    R0 |=0x80;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
	void ReadKnPC5(void)
	{
    unsigned    int R0;

    R0=RegSTemp & 0xfcff;
    if(bit_is_set(PIND,PD2))
    R0 |=0x100;
    if(bit_is_set(PIND,PD4))
    R0 |=0x200;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
	void ReadKnPC6(void)
	{
    unsigned    int R0;

    R0=RegSTemp & 0xf3ff;
    if(bit_is_set(PIND,PD2))
    R0 |=0x400;
    if(bit_is_set(PIND,PD4))
    R0 |=0x800;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
	void ReadKnPC7(void)
	{
    unsigned    int R0;

    R0=RegSTemp & 0xcfff;
    if(bit_is_set(PIND,PD2))
    R0 |=0x1000;
    if(bit_is_set(PIND,PD4))
    R0 |=0x2000;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
	void ReadKnPA0(void)
	{
    unsigned    int R0;

    R0=RegSTemp & 0x3fff;
    if(bit_is_set(PIND,PD2))
    R0 |=0x4000;
    if(bit_is_set(PIND,PD4))
    R0 |=0x8000;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}

     void    ReadKnPA1(void)
    {
/*

RegS1

0	Blokirovka	Zashit
1	DT	EPG
2	Control	Zashit
3	Reset	Error
4	Pogar
5	PZA=Norma
6	R Izol
7	R Izol
*/
    unsigned    char R0;

    R0=RegSTemp1 & 0xfc;
    if(bit_is_set(PIND,PD2))
    R0 |=0x1;
    if(bit_is_set(PIND,PD4))
    R0 |=0x2;
    if(R0==RegSTemp1)
		{
	if(CtWellKn1)
	--CtWellKn1;
	if(!CtWellKn1)
    RegS1=R0;
		}
 	RegSTemp1=R0;
	}
	void ReadKnPA2(void)
	{
    unsigned    char R0;

    R0=RegSTemp1 & 0xf3;
    if(bit_is_set(PIND,PD2))
    R0 |=0x4;
    if(bit_is_set(PIND,PD4))
    R0 |=0x8;
    if(R0==RegSTemp1)
		{
	if(CtWellKn1)
	--CtWellKn1;

		}
 	RegSTemp1=R0;
	}
	void ReadKnPA3(void)
	{
    unsigned    char R0;

    R0=RegSTemp1 & 0xcf;
    if(bit_is_set(PIND,PD2))
    R0 |=0x10;
    if(bit_is_set(PIND,PD4))
    R0 |=0x20;
    if(R0==RegSTemp1)
		{
	if(CtWellKn1)
	--CtWellKn1;

		}
 	RegSTemp1=R0;
	}
	void ReadKnPA4(void)
	{
    unsigned    char R0;

    R0=RegSTemp1 & 0x3f;
    if(bit_is_set(PIND,PD2))
    R0 |=0x40;
    if(bit_is_set(PIND,PD4))
    R0 |=0x80;
    if(R0==RegSTemp1)
		{
	if(CtWellKn1)
	--CtWellKn1;

		}
 	RegSTemp1=R0;
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
//	Error &=0xff7f;
    for(R0=1;R0<=10;++R0)
			{
    RomReceive[R0]=RamReceive[R0];
	RamReceive[R0]=2;
			}
	CtErrorLink=CtErrorLink0;
	CtError[7]=CtError0[7];
        }
   
    }
  
void ReceiveTransmitSlave(void)
    {

    if((TWSR & 0xf8)==0x60)//Adr Slave Receive
    CtReceive=10;

	else    if((TWSR & 0xf8)==0x80)//Data Slave Receive
        {

    RamReceive[CtReceive]=TWDR;
    if(CtReceive)
    --CtReceive;
	else
	LoadRomReceive();

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
	R0=0;

	if(RegimWork & 8)//enable KG
		{
	if(NumberBlok==3)// SHU1
	R0 |=1;
	else if((RegS & 0x20)&&(!(RomReceiveRS[1][3] & 4)))
	R0 |=1;
	else if(Error & 0x800)//no link SHU1
	R0 |=1;
	else if(!(RomReceiveRS[1][3] & 0x80))//No Zapret from Shu1
	R0 |=1;
	else if(RegS & 0x10)//KG=on
	R0 |=1;
		}
	if(RegS & 0x10)
	R0 |=2;//KG=on
	if(RegS & 0x20)
	R0 |=4;//KSet=on
	if(NumberBlok==3)// SHU1
		{
	if(RomReceiveRS[1][6] & 0x20)//Status DG2=on
	R0 |=0x20;
	if(RomReceiveRS[1][6] & 0x40)//Status Set2=on
	R0 |=0x40;
		}
	else if(NumberBlok==6)// SHU2
		{
	if(RomReceiveRS[1][3] & 0x20)
	R0 |=0x20;
	if(RomReceiveRS[1][3] & 0x40)
	R0 |=0x40;

		}
	if(( ErrorPZA)&&(!(ClearPZA)))
	R0 |=0x80;
	else
	R0 &=0x7f;	
    RegTransmit[1]=R0;

	R0=0;


	if(RomReceiveRS[4][NumberBlok-1])
	R0 |=0x10;
	else if(RomReceiveRS[5][NumberBlok-1])
	R0 |=0x10;
	else
	{
	if(TOG<=58)
	R0|=1;
	else if(TOG<=87)
	R0|=2;
	}
	if(RegS1 & 0x1)//Zashita=off
	R0 |=4;
	if(Error & 0x10)
	R0 |=8;

	if(!(PORTB & 2))
	R0|=0x40;//dozapravka topliva
	if(Error & 2)
	R0|=0x80;//no topliva
	if(Error & 0x20)
	R0 |=0x20;//pogar

    RegTransmit[7]=R0;


	 R0=NumberRegim;
	if(!(PORTB & 4))
    R0 |=8;//dozapravka masla
	if(Error & 4)
	R0 |=0x10;
	if(Error & 0x40)//RBP
    R0 |=0x20;
	if(NumberBlok==6)//SHU2
		{
	if(RomReceiveRS[6][0] & 0x50)
	R0 |=0x40;
		}
	else if(NumberBlok==3)//SHU1
		{
	if(RomReceiveRS[6][0] & 0x28)
	R0 |=0x40;
		}
    RegTransmit[6] =R0;	
	R0=0;
	if(NumberBlok==3)//SHU1
		{
	if(Error & 0x400)
	R0 |=1;
		}
	else//SHU2
		{
	if(Error & 0x2000)
	R0 |=1;
		}
	if(Error & 0x100)//no link SHSn
	R0 |=2;
	if(Error & 0x8001)//no oborotov
	R0 |=4;
    RegTransmit[3]=R0;//RomReceive[3];//NumberBlok;//R0;
	R0=RomReceiveRS[5][0];
	R0=~R0;
	if((RomReceiveRS[1][0] & 0x13)==(R0 & 0x13))
	RegTransmit[5]=RomReceiveRS[1][0];//From PDU
	if(NumberBlok==6)//SHU2
		{
	if(RomReceiveRS[1][0] & 2)
	RegTransmit[5] |=1;
	else
	RegTransmit[5] &=0xfe;
	if(!(Error & 0x4000))//No Error RS SHU 1
			{
    R0=RomReceiveRS[1][3];//1;
	R1=RomReceiveRS[4][3];
	R1 <<=3;
	R1 &=0x70;
	R0 &=0xf;
	R0 |=R1;


			}
	else
	R0=0;


	RegTransmit[2]=R0;
		}
	else
		{
	//SHU 1

	if(RomReceiveRS[1][0] & 1)
	RegTransmit[5] |=1;
	else
	RegTransmit[5] &=0xfe;

	if(!(Error & 0x800))//No Error RS SHU 2
			{
    R0=RomReceiveRS[1][6];//1
	R1=RomReceiveRS[4][6];
	R1 <<=3;
	R1 &=0x70;
	R0 &=0xf;
	R0 |=R1;

			}
	else
	R0=0;


	RegTransmit[2]=R0;
		}
	R0=0;
	if(RegS1 & 8)
	R0 |=1;
	if(NumberBlok==6)//SHU2
	R0|=2;
	if(Error & 0x8000)
	R0 |=0x4;
	if(Error & 0x20)
	R0 |=0x80;
	RegTransmit[4]=R0;
    } 


void	ControlStarter(void)
	 {
//Ndiz>400 from PUD
 	if(CtStarterOn[NumberStarter])
		{
 	if(!CtStarterOff[NumberStarter])
	PORTC &=0xf7;
		}
	else
		{
	PORTC |=8;

	if(!NumberStarter)
		{
	PORTC |=8;
	Error |=0x8000;
		}

	else
	--NumberStarter;

		}

	 }


void	InitStarter(void)
 {
	unsigned char R0;
	for(R0=0;R0<=2;++R0)
 	{
	CtStarterOn[R0]=CtStarterOn0;
	CtStarterOff[R0]=CtStarterOff0;
	}
	CtStarterOff[2]=0;
					
	NumberStarter=2;

 }
void	SettRegStop(void)
	{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	RegStop=0;
	R0=0xef;
	R1=0x8f;
	R2=8;
	if(RegS1 & 1)//Blokirovka zashit
		{
	R0=0x8f;
	R1=0xc;
		}
	if(NumberBlok==6)
		{
	RegNumberBlok=2;//SHU2
	R2=0x10;
		}
	else if(NumberBlok==3)
		{
	RegNumberBlok=1;//SHU1
	R2=8;
		}
	if(NumberRegim !=1)
		{

	if(RomReceiveRS[4][(NumberBlok-1)] & R0)//Error MSHU
	++RegStop;
	else if(RomReceiveRS[5][(NumberBlok-1)] & R1)//Error MSHU
	++RegStop;
	else if((RomReceiveRS[5][(NumberBlok-1)] & 0x20)&&(NDiz<400))//Ur.OG
	++RegStop;
	else if(Error & 0x8020)// NoLink 5min,pogar
	++RegStop;	
	else if(RomReceive[5] & 0x80)//Error Gen
	++RegStop;
	else if(RomReceiveRS[6][0] & R2)//Error LG
	++RegStop;
	else if(RomReceiveRS[2][0] & 4)//FVU
	++RegStop;
		}
	switch(NumberRegim)
		{
	case 0:
		
	if(!(RomReceiveRS[1][0] & RegNumberBlok))//Stop DU
		{
	if(RomReceiveRS[5][0] & RegNumberBlok)//Stop DU	
	++RegStop;
		}		

	break;
	case 1://hand MU
	RegStop=0;
	break;
	case 2://hand SHU


	if(RomReceive[1] & 0x20)//Stop KN KG
	++RegStop;

	break;
	case 3://Avt SHU
	if(RomReceive[1] & 0x20)//Stop KN KG
		{
	++RegStop;

		}
	else if(RomReceive[4] & 0x10)//Stop from KG
		{
	++RegStop;

		}

	break;
	case 4://Rezerv MU
	if((!(RomReceive[4] & 0x1))||(RomReceive[1] & 0x20))//Stop from KG
	++RegStop;

	break;
	case 5://Rezerv DU
	if(!(RomReceiveRS[1][0] & RegNumberBlok))//Stop DU
		{
	if(RomReceiveRS[5][0] & RegNumberBlok)// 1=~0 Stop DU	
	++RegStop;
		}



	break;
	default:break;
		}
	}

	void	Rashol(void)
	{
	if((Error & 0x20)||(RomReceiveRS[2][0] & 4)||(NDiz<200))
	RegimWork=1;
	else
	RegimTime |=2;
	if(!CtTime[1])
	RegimWork=1;
	}

void	ControlRegimWork(void)
 {
	volatile	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;
	R1=RomReceiveRS[1][3];
	R2=RomReceiveRS[5][3];
	R3=0x8;
	if(NumberBlok==6)
	R0=2;//SHU2
	else if(NumberBlok==3)
	R0=1;//SHU1
	SettRegStop();
	if(RegStop>100)
	RegStop=100;
	if(!RegimWork)
	NNom=1500; 
//NDiz >1400 from PUD
/*
0	Ostanov=1
1	Pusk=1
2	work=1
3	KG enable

6	Paralel work
7	Rasxolag=1
*/
	
/*	NumberRegim
0	DU
1	hand MU
2	hand SHU
3	Avt SHU
4	Rezerv MU
5	Rezerv DU
*/
	switch(NumberRegim)
	{


	case 0://DU
	if(!RegimWork)
		{
	InitStarter();
	RegStop=0;

	if(!(Error& 0x60))
			{

	if((RomReceiveRS[1][0] & R0)&&(!(RegStop))&&(TOG>=58))
	RegimWork |=2;

			}
		}
	else if(RegimWork & 1)
		{
	
	if(NDiz<150)//Stop=well
			{
	RegimTime |=1;
	PORTC |=8;
	if(!CtTime[0])
	RegimWork =0;
			}
		}



	else if(RegimWork & 2)
		{
	if(NDiz>1450)
			{

	RegimWork=4;
			}


	RegimTime &=0xfe;
	if(NDiz<400)
	ControlStarter();
	if(RegStop1>=5)//Stop
			{
	PORTC |=8;
	RegimWork &=0xfd;
	RegimWork |=1;
			}
		}
	else if(RegimWork & 4)
		{
	PORTC |=8;//Starter=off;
	if(TOG>=87)
	RegimWork=8;
	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}
	else if(RegimWork & 8)
		{

	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{

	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}


	else if(RegimWork & 0x80)//rasxol
		{

	if(RomReceiveRS[1][0] & R0)//Pusk
			{
	RegStop=0;
	RegimWork=0x4;//work
			}
	else
	Rashol();
		}
	
	
	
	break;

	case 1://hand MU
	PORTB |=0x1a;


	PORTC	|=0xc;
	RegimWork=0;
	break;

	case 2:
	if(!RegimWork)
		{
	InitStarter();
	RegStop=0;
	if(!(Error & 0x60))
			{
	if((RomReceive[1] & 1)&&(!(RegStop))&&(TOG>=58))//Pusk
	RegimWork |=2;
			}
		}
	else if(RegimWork & 1)
		{
	
	if(NDiz<150)//Stop=well
			{
	RegimTime |=1;
	PORTC |=8;
	if(!CtTime[0])
	RegimWork =0;
			}
		}



	else if(RegimWork & 2)
		{
	if(NDiz>1450)
			{

	RegimWork=4;
			}


	RegimTime &=0xfe;
	if(NDiz<400)
	ControlStarter();
	if(RegStop1>=5)//Stop
			{
	PORTC |=8;
	RegimWork &=0xfd;
	RegimWork |=1;
			}
		}
	else if(RegimWork & 4)
		{
	PORTC |=8;//Starter=off;
	if(TOG>=87)
	RegimWork=8;
	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}
	else if(RegimWork & 8)
		{

	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}


	else if(RegimWork & 0x80)//rasxol
		{

	if(RomReceive[1] & 1)//Pusk
			{
	RegStop=0;
	RegimWork=0x4;//work
			}
	else
	Rashol();



		}

	break;

	case 3://avt SHU
	if(NumberBlok==3)
		{
	R1=RomReceiveRS[1][6];//kg
	R2=RomReceiveRS[5][6];//avaria
	R3=0x10;//Error Lg
		}

	if(RomReceiveRS[6][0] & R3)
	R2 |=0x80;
	if(R1 & 2)//KG2=on
	RezervDiz2=1;
	else if(RegTransmit[1] &2)//KG1=on
	RezervDiz2=0;


	if(!RegimWork)
		{
	InitStarter();
	RegStop=0;
	if(!(Error& 0x60))
			{
	if((RomReceive[1] & 1)&&(!(RegStop))&&(TOG>=58))
	RegimWork |=2;
	else if(((RezervDiz2)&&(!(R1 & 2)))&&(R2 & 0x80)&&(TOG>=58))
	RegimWork |=2;	 

			}
		}
	else if(RegimWork & 1)
		{
	
	if(NDiz<150)//Stop=well
			{
	RegimTime |=1;
	PORTC |=8;
	if(!CtTime[0])
	RegimWork =0;
			}
		}



	else if(RegimWork & 2)
		{
	if(NDiz>1450)
			{

	RegimWork=4;
			}


	RegimTime &=0xfe;
	if(NDiz<400)
	ControlStarter();
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork &=0xfd;
	RegimWork |=1;
			}
		}
	else if(RegimWork & 4)
		{
	PORTC |=8;//Starter=off;
	if(TOG>=87)
	RegimWork=8;
	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}
	else if(RegimWork & 8)
		{

	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}


	else if(RegimWork & 0x80)//rasxol
		{

	if(RomReceive[1] & 1)//Pusk
			{
	RegStop=0;
	RegimWork=0x4;//work
			}
	else
	Rashol();



		}

	break;

	case 4://RezervMU
	if(!RegimWork)
		{
	InitStarter();
	RegStop=0;
	if(!(Error& 0x60))
			{
	if((RomReceive[4] & 1)&&(!(RegStop))&&(TOG>=58))
	RegimWork |=2;
			}
		}
	else if(RegimWork & 1)
		{
	
	if(NDiz<150)//Stop=well
			{
	RegimTime |=1;
	PORTC |=8;
	if(!CtTime[0])
	RegimWork =0;
			}
		}



	else if(RegimWork & 2)
		{
	if(NDiz>1450)
			{

	RegimWork=4;
			}


	RegimTime &=0xfe;
	if(NDiz<400)
	ControlStarter();
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork &=0xfd;
	RegimWork |=1;
			}
		}
	else if(RegimWork & 4)
		{
	PORTC |=8;//Starter=off;
	if(TOG>=87)
	RegimWork=8;
	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}
	else if(RegimWork & 8)
		{

	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}


	else if(RegimWork & 0x80)//rasxol
		{

	if(RomReceive[4] & 1)//Pusk
			{
	RegStop=0;
	RegimWork=0x4;//work
			}
	else
	Rashol();



		}

	break;
	case 5://RezervDU
	if(!RegimWork)
		{
	InitStarter();
	RegStop=0;
	if(!(Error & 0x60))

			{

	if((RomReceiveRS[1][0] & R0)&&(!(RegStop))&&(TOG>=58))
	RegimWork |=2;

			}


		}
	else if(RegimWork & 1)
		{
	
	if(NDiz<150)//Stop=well
			{
	RegimTime |=1;
	PORTC |=8;
	if(!CtTime[0])
	RegimWork =0;
			}
		}



	else if(RegimWork & 2)
		{
	if(NDiz>1450)
			{

	RegimWork=4;
			}

	RegimTime &=0xfe;
	if(NDiz<400)
	ControlStarter();
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork &=0xfd;
	RegimWork |=1;
			}
		}
	else if(RegimWork & 4)
		{
	PORTC |=8;//Starter=off;
	if(TOG>=87)
	RegimWork=8;
	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}
	else if(RegimWork & 8)
		{

	RegimTime &=0xfd;
	if(RegStop1>=5)//Stop
	
			{
	PORTC |=8;
	RegimWork=0x80;//rasxol
			}
		}


	else if(RegimWork & 0x80)//rasxol
		{

	if(RomReceive[4] & 1)//Pusk
			{
	RegStop=0;
	RegimWork=0x4;//work
			}
	else
	Rashol();



		}

	break;
	default:break;
	}
 }
	void ChangeP(void)
	{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	R0=RomReceive[1];
	R2=RomReceive[2];
	R1=RegTransmitRS[7];
	if((RomOld & 0x10)!=(R0 & 0x10))
		{
		if(R0 & 0x10)
		++R1;
	RomOld=R0;
		}
	else if((RomOld1 & 2)!=(R2 & 2))
		{
		if(R2 & 2)
		--R1;
		}
	if(R1<5)
	R1=5;
	if(R1>35)
	R1=35;
	RegTransmitRS[7]=R1;
	RomOld=R0;
	RomOld1=R2;
	}
void LoadRegTransmitRS(void)
	{
	unsigned char R0;
	volatile	unsigned char R1;
	R0=0;
	if(RegimWork & 1)
	R0 &=0xfe;//Stop
	else
	R0 |=1;//no stop
	if(RegS & 0x10)
	R0 |=2;//KG=on
	if(RegS & 0x20)
	R0 |=4;//KSet=on
	if(NumberBlok==3)//SHU1
	R1=RomReceiveRS[1][6];
	else if(NumberBlok==6)
	R1=RomReceiveRS[1][3];
	if((RomReceive[3] & 0x40)&&(R1 & 0x20))
	R0 |=8;//Statizm=on	
	else
	R0 &=0xf7;//statizm=off	prov???
	if(NumberRegim>=3)
	R0 |=0x10;
	else
	R0 &=0xef;
	if(RomReceive[3] & 0x40)//StatusKontGen=on
	R0 |=0x20;
	else
	R0 &=0xdf;
	if(RomReceive[3] & 0x80)//StatusKontSet=on
	R0 |=0x40;
	else
	R0 &=0xbf;
	if((RegimWork & 8)&&(!(RegS & 0x10))&&(NumberRegim !=2))
	R0 |=0x80;		
	RegTransmitRS[1] =R0;

	R0=NNom;
	RegTransmitRS[2]=R0;

	R0=NNom>>8;
	RegTransmitRS[3]=R0;

	R0=RomReceive[4]& 0x1f;
	if(!(RegS & 0x2c0))
	R0 |=0x20;
	if((ErrorPZA)&&(!ClearPZA))
	R0 |=0x40;
	RegTransmitRS[4]=R0;

	R0=0;
	if(NDiz>400)
	R0 |=1;
	if(RomReceive[5] & 1)
	R0 |=8;//ULine=norma
	if(RegS1 & 1)//Blokirovka zashit
	R0 |=0x10;
	if(NumberBlok==3)//SHU1
	R1=2;
	else
	R1=5;
	if((RomReceiveRS[6][R1]>=125)&&(NDiz>400))			
	R0 |=2;
	if((RomReceiveRS[6][R1]<=110)||(NDiz<400))			
	R0 |=0x20;
	if(RomReceive[5] & 0x80)//Error From KG
	R0 |=0x80;
	if(Error & 0x20)
	R0 |=0x80;
	if(CtBegin)
	R0 |=4;
	if(RegS1 & 8)
	R0 |=4;//ResetError
	RegTransmitRS[5]=R0;


	R0=0;
	if(NumberRegim==5)
	R0 |=8;//Rezerv
	if(RegS1 & 1)
	R0 |=0x10;//Blokirovka
	if(Error & 0x20)//Pogar
	R0 |=0x80;
	if(RomReceive[5] & 0x20)
	R0 |=4;
	if(Error & 0xf7)
	R0 |=0x40;
	if(Error & 0x10)//Izol
	R0 |=0x20;	
		
	RegTransmitRS[6] =R0;

	if(NumberBlok==6)
    R0=RomReceiveRS[1][3];

	else
    R0=RomReceiveRS[1][6];
	if(RegS & 0x10) //KG1,KG2=0n prov???
	ChangeP();	
					
	}
void	SetRegimError(void)
	{

	if((RegS & 4)&&(RomReceive[5] & 0x20))//RBP
		{
	RegimError |=0x40;

		}
	else
	RegimError &=0xffbf;
	if(!(RegS1 & 0x10))//POGAR
	RegimError |=0x20;
	else
	RegimError &=0xffdf;
	if((NDiz<1400) && (RegimWork & 0xc))
	RegimError |=0x1;
	else
	RegimError &=0xfffe;

	if(!(RegS1 & 0x40))//RIz1
	RegimError |=0x8;
	else
	RegimError &=0xfff7;
	if(!(RegS1 & 0x80))//RIz1
	RegimError |=0x10;
	else
	RegimError &=0xffef;
	if((RegS1 & 0x20)||(!(RegTransmit[1] & 6)))
		{
	ClearPZA=0;
	ErrorPZA=0;
	CtPZA=CtPZA0;
		}



	}

void	SetError(void)
	{
	unsigned char R0;

	for(R0=0;R0<=14;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}
	if(!CtPZA)
	ErrorPZA |=1;


	}
void	ClearError(void)
	{
	unsigned char R0;
	if(ErrorPZA)
	ClearPZA=0x55;
	CtPZA=CtPZA0;
	ErrorPZA=0;
	NumberStarter=2;
	Error=0;
	RegimError=0;
	for(R0=0;R0<=15;++R0)
	CtError[R0]=CtError0[R0];

	}
	

void	ControlZapravkaTopliva(void)
	{
	if(RegS & 0x800)
	PORTB |=2;
	else if(!(RegS & 0x1000))
	PORTB &=0xfd;
	if(!(RegS & 0x2000))
	RegimError |=2;
	else
	RegimError &=0xfffd;
	}		
	void	ControlZapravkaMasla(void)
	{
	if(CtMaslo>=15)
		{
	PORTB |=4;
	RegimError &=0xfffb;
		}
	else if(CtMaslo<=5)
		{
	RegimError |=0x4;
	PORTB &=0xfb;

		}

	}

void	ControlPodogrev(void)
	{

	unsigned char R1;
	R1=NumberBlok-1;
	if(RomReceiveRS[5][R1] & 0x20)
	PORTB |=1;
	else if(RomReceiveRS[6][R1]>=125)
	PORTB |=1;
	else if(RomReceiveRS[6][R1]<=117)
	PORTB &=0xfe;

	}	
/*	void	ControlAvarStop(void)
	{
	if((!(RegTransmitRS[4] & 1)) &&(NDiz>400))//stop	
	RegimError |=0x10;
	else
	RegimError &=0xffef;
	if(Error & 0x10)
	PORTD &=0x7f;
	}*/
	
	void	AccountNDiz(void)
	{
	unsigned int R0;
	R0=RomReceiveRS[3][NumberBlok-1]<<8;
	R0 |=RomReceiveRS[2][NumberBlok-1];
	NDiz=R0;
	}
	void	ChangeNNom(void)
	{
	unsigned char R0;
	unsigned char R1;
	R1=RegTransmitRS[1];
	if(NumberBlok==6)
    R0=RomReceiveRS[1][3];

	else
    R0=RomReceiveRS[1][6];
	if((RegS & 0x10) && (R0 & 2))//KG1,KG2=0n
		{
	NNom=1500;

		}
	else if(NumberRegim==2)//hand Regim
		{
	if(!EnableNNom)
				{	
	if(RomReceive[1] & 4)
	++NNom;
	else if(RomReceive[1] & 0x80)
	--NNom;
	EnableNNom=10;
				}
		}
	else if((RegS & 0x10)&&(NumberBlok==6)) //KG=0n
		{
	if(!(RomReceive[3] & 0x80))//StatusSet=0
	NNom=1500; 
	else if(RomReceive[3] & 0x80)
			{
	if(!EnableNNom)
				{	
	if(RomReceive[1] & 4)
	++NNom;
	else if(RomReceive[1] & 0x80)
	--NNom;
	EnableNNom=10;
				}
			}

		}
	else //KG=off
		{
	if((R0 & 6)||(R1 & 4))//KG2 | KSet=on
			{	
	if(RomReceive[3] & 0x40)//StatusKG=1
				{	
	 if(!EnableNNom)
					{	
	if(RomReceive[1] & 4)
	++NNom;
	else if(RomReceive[1] & 0x80)
	--NNom;
	EnableNNom=10;
					}
				}
	else if(NumberRegim != 2)//StatusKG=0
	NNom=1500;
			}
	else if(NumberRegim != 2)//KSet=off
	NNom=1500;
		}					
	if(NNom>1575)
	NNom=1575;
	if(NNom<1400)
	NNom=1400;
	}	
	void	ChangeNumberRegim(void)
	{
/*	NumberRegim
0	DU
1	hand MU
2	hand SHU
3	Avt SHU
4	Rezerv MU
5	Rezerv DU
*/
	unsigned char R0;
	unsigned char R1;
	if(NumberBlok==3)
	R1=6;
	else
	R1=3;
	R0=RegTransmit[1];
//	if(RomReceive[3] & 6)
//	NumberRegim=1;
	if(!(RegS & 0xc0))
	NumberRegim=1;
	else
	{

	if((!(RegS & 0x200))&&(RomReceiveRS[1][0] & 0x80))//DU
		{	
	if(RomReceiveRS[1][0] & 0x20)//Rezerv
			{
	if(R0 & 4)//KS=on
	NumberRegim=5;//Rezerv DU
			}
	else
	NumberRegim=0;// DU
		}	
	else if(RegS & 0x200)//MU
		{				

	if(RegS & 8)//Rezerv
			{
	if(R0 & 4)
	NumberRegim=4;//Rezerv MU
			}

	else
			{	
	
	if(RegS & 0x40)
	NumberRegim=3;//Avt SHU
	else	if(RegS & 0x80)
	NumberRegim=2;//Hand SHU
	else
	NumberRegim=1;//hand MShU
			}
		}
	}
	}
	
	void ControlLampError(void)
	{
	unsigned int R0;
	unsigned char R1;
	if(NumberBlok==6)
		{
	R0=0xa1f7;
	R1=0xd0;
		}
	else
		{
	R0=0x85f7;
	R1=0xa8;
		}
	if(RomReceive[5] & 0xc0)//Error KG
		{
	PORTC &=0xfb;

		}

	else if((RomReceiveRS[4][NumberBlok-1])||(RomReceiveRS[5][NumberBlok-1] ))
		{
	PORTC &=0xfb;
	TestLamp =10;

		}
	else if((ErrorPZA)&&(!ClearPZA))
		{
	PORTC &=0xfb;
	TestLamp =30;
		}


	else if(Error & R0)
		{
	PORTC &=0xfb;
	TestLamp =30;
		}
//	else if(TOG<87)
//	PORTC &=0xfb;
	else if(RomReceiveRS[6][0]& R1)
		{
	PORTC &=0xfb;
	TestLamp =30;
		}
	else
		{

	PORTC |=4;

		}
	if(TestLamp>255)
	TestLamp=0;



	}	
	void ControlNU(void)
	{
	unsigned char R0;
	unsigned char R1;
	if(NumberBlok==6)
	R1=3;
	else
	R1=6;
	R0=RomReceive[6];
	if(!(RegS & 0x10))
		{
	PORTB |=0x8;
	CtNUOn=CtNUOn0;
		}
	else
		{
	if(RomReceiveRS[1][R1] & 0x20)
			{
	CtNUOff=CtNUOff0;
	PORTB &=0xf7;//NU=on
			}
	if((R0>25)||(!(RegS & 0x10)))//25kbt,KG=off
	CtNUOn=CtNUOn0;
	else	if(!CtNUOn)
	PORTB &=0xf7;

	if(R0<50)
	CtNUOff=CtNUOff0;
	else if(!CtNUOff)
	PORTB |=0x8;
		}

	}
	
	
	
																											        
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	unsigned char Arg0;
 



//if DDRX,X=1 PORTX,X=OUT
    DDRA=0xff;


    DDRB=0x1f;
	PORTB = 0xff;

    DDRC=0xfc;
    PORTC =0xff;

    DDRD=0xea;

    PORTD=0xff;
	RegTransmitRS[7]=16;//ReadEeprom(4);
//INIT TWI;
    TWBR=0x7f;//F TWI


    TCCR1B=0x2;//0.5mkc
    TIMSK=TIMSK | 0x4;//enable Int overlowT1
    TWAR=(NumberLink<<1)+4;   
   TWCR =(1<<TWEA)|(1<<TWEN);

   _SEI();
    RegimTime =0;

		
	for(Arg0=1;Arg0<=10;++Arg0)
	RomReceive[Arg0]=0;




	RegimWork=0;
	CtErrorLink=CtErrorLink0;

	CtKn=5;

	Regim=1;//Gashenie=on;

	InitUsart();  
			
	CtUsart=1;	
	CtEnableKn=3;
	InitStarter();

	CtErrorRS=9;
	NumberRegim=1;// hand MShU
	NNom=1500;
	CtBegin=50;
	RegS=0x1200;
	RegSTemp=0x1200;
	RegS1=0;
	RegSTemp1=0;
	CtErrorRSMin=255;
	NumberRegim=2;
	ClearPZA=0;
	CtPZA=CtPZA0;
	ErrorPZA=0;
	RezervDiz2=0;
//WORK	
	           
      while(1)
    {


	_WDR();

	ControlNU();
	PORTB &=0xef;
/*	if(RegimWork<=2)
		{
	ReadEeprom(4);
	if(ReadEeprom(4)!=RegTransmitRS[7])
	WriteEeprom(4,RegTransmitRS[7]);
		}*/






	if((!RegimWork)||(RegimWork & 0x8))
	PORTD |=0x60;
	else if((RegimWork & 1)||(RegimWork & 0x80))
		{
	PORTD &=0xbf;

		}
	else
		{
	PORTD &=0xdf;
	PORTD |=0x40;
		}


	ControlZapravkaTopliva();
	ControlZapravkaMasla();
	ControlPodogrev();
//	ControlAvarStop();
	ControlLampError();
	TOG=RomReceiveRS[6][(NumberBlok-1)];


	if(CtBegin)
		
	ClearError();
	else if(RegS1 & 8)
		{
	ClearError();


		}
	if(!(RegTransmit[1] & 6))
	ClearPZA=0;
	if(RegS1 & 0x20)
		{
	ClearPZA=0;

		}
	if(RomReceive[3] & 1) 
	NumberBlok=6;//SHU 2
	else
	NumberBlok=3;//SHU 1



		


	if(PORTC & 8)
	ChangeNumberRegim();

	ChangeNNom();
	AccountNDiz();
	LoadRegTransmitRS();

	SetRegimError();
	SetError();
	ControlRegimWork();


     _WDR();
	if(EndAd)
		{

	EndAd=0;
		}
	if(EnableLoad)
		{
    LoadRegTransmit();
    LoadControlSum();
		}
	if(TWCR & 0x80)
			{
    ReceiveTransmitSlave();

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

	CtErrorLink=CtErrorLink0;

    CtStart=200;
	while(CtStart--)	_WDR();
		}



				

    }
  
}

 SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
    {
 	unsigned char R0;
	if(CtNNom)
	--CtNNom;
	if(CtNUOn)
	--CtNUOn;
	if(CtNUOff)
	--CtNUOff;



	if(!(RegS & 0x400))//Maslo=off
		{
	if(CtMaslo)
	--CtMaslo;
		}
	else
		{
	if(CtMaslo<20)
	++CtMaslo;
		}
	RegimError |=0x7f80;//RegimErrorLink PKG,RS


	if(RegStop)
	++RegStop1;
	else
	RegStop1=0;




	if(CtBegin)
	--CtBegin;
	if(EnableNNom)
	--EnableNNom;
	if(PORTC & 8)
		{
	if(CtStarterOff[NumberStarter])
	--CtStarterOff[NumberStarter];
		}
	else
		{
	if(CtStarterOn[NumberStarter])
	--CtStarterOn[NumberStarter];
		}
	if(CtEnableKn)
	--CtEnableKn;
	else
	CtEnableKn=21;
	switch(CtEnableKn)
		{

	case 0:
	ReadKnPA6();
	break;

	case 1:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x40;
	break;

	case 2:
	ReadKnPA7();
	break;

	case 3:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x80;
	break;

	case 4:
	ReadKnPC4();
	break;

	case 5:
	PORTA=0;
	PORTC &=0xf;
	PORTC |=0x10;
	break;

	case 6:
	ReadKnPC5();
	break;

	case 7:
	PORTA=0;
	PORTC &=0xf;
	PORTC |=0x20;
	break;

	case 8:
	ReadKnPC6();
	break;

	case 9:
	PORTA=0;
	PORTC &=0xf;
	PORTC |=0x40;
	break;

	case 10:
	ReadKnPC7();
	break;

	case 11:
	PORTA=0;
	PORTC &=0xf;
	PORTC |=0x80;
	break;

	case 12:
	ReadKnPA0();
	break;

	case 13:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x1;
	CtWellKn=7;
	break;

	case 14:
	ReadKnPA1();
	break;

	case 15:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x2;
	break;

	case 16:
	ReadKnPA2();
	break;

	case 17:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x4;
	break;

	case 18:
	ReadKnPA3();
	break;

	case 19:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x8;
	break;
	case 20:
	ReadKnPA4();
	break;

	case 21:
	PORTA=0;
	PORTC &=0xf;
	PORTA |=0x10;
	CtWellKn1=4;
	break;
	default :CtEnableKn=21;
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
	CtErrorRS=NumberBlok+39;
		}


	if(CtUsart)
	--CtUsart;
	else
		{

//	++TestRS;
	TransmitUsart();
	CtUsart=(NumberBlok<<1)+15;
		}

    if(CtErrorLink)//TWI
    --CtErrorLink;
    for(R0=7;R0<=14;++R0)
	    {

    if(CtError[R0])
    --CtError[R0];
        }



    for(R0=0;R0<=6;++R0)
	    {
    if(RegimError & (1<<R0))
	        {
    if(CtError[R0])
    --CtError[R0];
	        }
    else
    CtError[R0]=CtError0[R0];

    if(RegimTime & (1<<R0))
        {
    if(CtTime[R0])
    --CtTime[R0];
        }
    else
    CtTime[R0]=CtTime0[R0];
	    }
	if(CtPZA)
	--CtPZA;


    }
	SIGNAL(SIG_UART_RECV)
	{
//	++TestRS;
	LinkRS();
	}


