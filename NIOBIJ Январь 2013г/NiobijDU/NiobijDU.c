
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
	unsigned int CtFire;
    unsigned char CtKnKn;
    unsigned char CtKn;
    unsigned char CtStart;
    unsigned char TestRS;//RegSegmentMig[4];
    unsigned char RegSegmentMig[4];
    unsigned char RegSegmentOn[4];

    unsigned char CtErrorRS;
    unsigned char NumberBlok;
    unsigned char NumberReceiveRS;
	volatile    unsigned char CtErrorGlobalRS[3];
    const unsigned char CtErrorGlobalRS0=100;
	volatile    unsigned char CtUsart;
    unsigned char NumberLinkRS;
    unsigned char RegTransmitRS[7];
    unsigned char RamReceiveRS[8];
    unsigned char RomReceiveRS[7][3];
    unsigned char RomReceiveRSOld[7];

    unsigned char	ControlTransmitRS;
    unsigned char ControlReceiveRS;
	volatile unsigned char CtReceiveRS;
    const unsigned char CtReceiveRS0=7;

	unsigned    int         	Error;
	unsigned    char         	ErrorInf;

    unsigned    int    RegimWork;
    unsigned    int    ReceiveRegimWork;
    const    unsigned char        CtMig0=60;
    unsigned    char              CtMig;

	volatile unsigned char Segment[8];
	volatile unsigned char CtByteAnod;
	volatile unsigned char CtSetka;
 	volatile unsigned    char    MaskaSetka;

	volatile 	unsigned    int	RegS;
	volatile 	unsigned    int	RegSTemp;
    volatile 	unsigned    int RegSOld;
    volatile 	unsigned    int RegSWork;
	unsigned    char	RegS1;
	unsigned    char	RegSTemp1;
    unsigned char CtEnableKn;

       
    /*============================================*/
	void	ResetErrorLinkRS(void)
{
	unsigned char R0;
	R0=UDR;//UDR1;
	UCSRA &=0x22;//reset error


	CtReceiveRS=CtReceiveRS0;
}
 	void	InitUsart(void)
	{	
	unsigned char R0;
   UCSRA=2;//������� ��������
   UCSRA |=1;//only 9bit=1
   UCSRB=0x1c;//enable transmit 9bit
   UBRRH=0;
   UBRRL=207;
	R0=0x86;
	UCSRC=R0;

   UCSRB |=0x80;//enable int receive
   PORTD &=0xf7;//Out Transmit=off


	}	
	void	TransmitUsart(void)
	{
	unsigned int R0;

   PORTD |=0x8;//Out Transmit=on
   R0=1000;
   while(R0)
   --R0;
     while(!(UCSRA & 0x20))
   _WDR();
/*	if(NumberLinkRS)
	--NumberLinkRS;
	else
	NumberLinkRS=5;*/
	NumberLinkRS=1;
   UCSRB |=1;//9 bit=1

    UDR=NumberBlok;//LinkRS;
	ControlTransmitRS =NumberBlok;
	UCSRA |=1;//Receive only 9 bit=1


			
	}		  

	void	LinkPult(void)
{

	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	unsigned char R3;
	unsigned char R4;
//	unsigned int R5;

	PORTD &= 0xf7;//Transmit denable
	if(UCSRA &(1<<RXC))
	{

	R1=UCSRA;
	R2=UCSRB;
	R3=UCSRC;
	R0=UDR;
	RamReceiveRS[CtReceiveRS]=R0;
	if(R2 & 2)
		{

	NumberReceiveRS=R0;
	if(NumberBlok>NumberReceiveRS)
			{
	R4=NumberBlok-NumberReceiveRS;
	CtUsart=R4<<1;
			}
	else
			{
	R4=NumberBlok-NumberReceiveRS;
	R4 <<=1;
	R4=~R4;
	CtUsart=R4+7;


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

	if(R1 & 0x1c)//error
		{


	ResetErrorLinkRS();
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
    UDR=ControlTransmitRS;
	else
				{
    UDR=RegTransmitRS[CtReceiveRS];
	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
				}
			}
		}
	else
		{


	PORTD &= 0xf7;//Transmit denable
	if(R0==ControlReceiveRS)
			{

	CtErrorRS=10;
	CtErrorGlobalRS[NumberReceiveRS]=CtErrorGlobalRS0;

//	if(!NumberReceiveRS)//prov for pksn13
					{

	for(R4=1;R4<=6;++R4)
						{
	if(!NumberReceiveRS)
	RomReceiveRSOld[R4]=RomReceiveRS[R4][0];
	RomReceiveRS[R4][NumberReceiveRS]=RamReceiveRS[R4];

						}
	if((!(RomReceiveRSOld[1] & 3))&&(RomReceiveRS[1][0] &3)&&(!(RegimWork& 0x80)))
	RegimWork &=0xffdf;	//set=0ff
	if((!(RomReceiveRSOld[1] & 0x20))&&(RomReceiveRS[1][0] & 0x20))
	RegimWork &=0xffed;	//gen=0ff	
												
					}
	R4=NumberReceiveRS;
	if(R4<=1)
	++R4;
	else
	R4=0;
	if(R4==NumberBlok)
	TransmitUsart();


			}

		}

	}
}



     void    ReadKn(void)
    {

    unsigned    int R0;
    unsigned    char R1;
    unsigned    char R2;
    R0=RegSTemp;
    R1=RegSTemp1;
	if(!(PORTC & 0x1))
		{
    if(bit_is_clear(PINC,PC4))
    R0 |=0x1;
	else
    R0 &=0xfffe;
    if(bit_is_clear(PINC,PC5))
    R0 |=0x10;
	else
    R0 &=0xffef;
    if(bit_is_clear(PINC,PC6))
    R0 |=0x100;
	else
    R0 &=0xfeff;
    if(bit_is_clear(PINC,PC7))
    R0 |=0x1000;
	else
    R0 &=0xefff;

		}
	else if(!(PORTC & 0x2))
		{
    if(bit_is_clear(PINC,PC4))
    R0 |=0x2;
	else
    R0 &=0xfffd;
    if(bit_is_clear(PINC,PC5))
    R0 |=0x20;
	else
    R0 &=0xffdf;
    if(bit_is_clear(PINC,PC6))
    R0 |=0x200;
	else
    R0 &=0xfdff;
    if(bit_is_clear(PINC,PC7))
    R0 |=0x2000;
	else
    R0 &=0xdfff;

		}
	else if(!(PORTC & 0x4))
		{
    if(bit_is_clear(PINC,PC4))
    R0 |=0x4;
	else
    R0 &=0xfffb;
    if(bit_is_clear(PINC,PC5))
    R0 |=0x40;
	else
    R0 &=0xffbf;
    if(bit_is_clear(PINC,PC6))
    R0 |=0x400;
	else
    R0 &=0xfbff;
    if(bit_is_clear(PINC,PC7))
    R0 |=0x4000;
	else
    R0 &=0xbfff;

		}
	else if(!(PORTC & 0x8))
		{
    if(bit_is_clear(PINC,PC4))
    R0 |=0x8;
	else
    R0 &=0xfff7;
    if(bit_is_clear(PINC,PC5))
    R0 |=0x80;
	else
    R0 &=0xff7f;
    if(bit_is_clear(PINC,PC6))
    R0 |=0x800;
	else
    R0 &=0xf7ff;
    if(bit_is_clear(PINC,PC7))
    R0 |=0x8000;
	else
    R0 &=0x7fff;

		}
	if(bit_is_clear(PINB,PB3))
    R1 |=1;
	else
    R1 &=0xfe;
	 
    if(R0==RegSTemp)
		{
	if(CtKnKn)
	--CtKnKn;
	else
			{
	RegSOld=RegS;
    RegS=R0;

	for(R2=0;R2<=15;++R2)
				{
	if((RegS & (1<<R2))&&(!(RegSOld & (1<<R2))))
					{
	RegSWork |=(1<<R2);
	CtKn=30;
					}
	CtKnKn=5;
				}
			}
		}
	else
	CtKnKn=5;
	if(!CtKn)
	RegSWork=0;
    RegSTemp=R0;
     if(R1==RegSTemp1)
    RegS1=R1;

    RegSTemp1=R1;
    }











   void IndicatorSegment  (void)	
	{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	for(R1=0;R1<=3;++R1)
		{
	for(R0=0;R0<=6;++R0)
			{
	if(RegSegmentOn[R1] & (1<<R0))
	Segment[R1] |=(1<<R0);
	else	if(RegSegmentMig[R1] & (1<<R0))
				{
	if(CtMig>=(CtMig0>>1))
	Segment[R1] |=(1<<R0);
	else

					{

	R2=~(1<<R0);
	Segment[R1] &=R2;										

					}
				}
	else
				{
	R2=~(1<<R0);
	Segment[R1] &=R2;
				}
			}
		}


	}




   void SetRegSegmentOn (void)
    {
	if(RomReceiveRS[1][0] & 1)// DG1
	RegSegmentOn[2] |=0x10;
	else
	RegSegmentOn[2] &=0xef;

	if(RomReceiveRS[1][0] & 0x2)// DG2
	RegSegmentOn[2] |=0x40;
	else
	RegSegmentOn[2] &=0xbf;

	if(RomReceiveRS[1][0] & 4)// PCH1
	RegSegmentOn[0] |=0x40;
	else
	RegSegmentOn[0] &=0xbf;

	if(RomReceiveRS[1][0] & 8)// PCH2
	RegSegmentOn[1] |=0x8;
	else
	RegSegmentOn[1] &=0xf7;

	if(RomReceiveRS[1][0] & 0x10)// Set
	RegSegmentOn[3] |=0x2;
	else
	RegSegmentOn[3] &=0xfd;

	if(RomReceiveRS[1][0] & 0x20)// Kontaktor Set
	RegSegmentOn[2] |=0x8;
	else
	RegSegmentOn[2] &=0xf7;

	if(RomReceiveRS[2][0] & 0x1)// Error DG1
	RegSegmentOn[2] |=0x20;
	else
	RegSegmentOn[2] &=0xdf;

	if(RomReceiveRS[2][0] & 0x2)// Error DG2
	RegSegmentOn[3] |=0x1;
	else
	RegSegmentOn[3] &=0xfe;

	if(RomReceiveRS[2][0] & 0x4)// Error PCH1
	RegSegmentOn[1] |=0x1;
	else
	RegSegmentOn[1] &=0xfe;

	if(RomReceiveRS[2][0] & 0x8)// Error PCH2
	RegSegmentOn[1] |=0x10;
	else
	RegSegmentOn[1] &=0xef;

	if(RomReceiveRS[2][0] & 0x20)// Izol
	RegSegmentOn[2] |=0x4;
	else
	RegSegmentOn[2] &=0xfb;



	if(RegimWork & 1)//pusk
	RegSegmentOn[0] |=8;
	else
	RegSegmentOn[0] &=0xf7;

	if(RegimWork & 2)//DG1
	RegSegmentOn[0] |=0x10;
	else
	RegSegmentOn[0] &=0xef;

	if(RegimWork & 4)//PCH1
	RegSegmentOn[0] |=0x20;
	else
	RegSegmentOn[0] &=0xdf;

	if(RegimWork & 8)//PCH2
	RegSegmentOn[1] |=0x4;
	else
	RegSegmentOn[1] &=0xfb;

	if(RegimWork & 0x10)//DG2
	RegSegmentOn[1] |=0x2;
	else
	RegSegmentOn[1] &=0xfd;

	if(RegimWork & 0x20)//Set

	RegSegmentOn[1] |=0x40;
	else
	RegSegmentOn[1] &=0xbf;




	if(RegimWork & 0x40)//RU50
	RegSegmentOn[0] |=0x4;
	else
	RegSegmentOn[0] &=0xfb;

	if(RegimWork & 0x80)//Rezerv Set
	RegSegmentOn[1] |=0x20;
	else
	RegSegmentOn[1] &=0xdf;

	RegSegmentMig[0] &=0xfd;
	if(RegimWork & 0x100)//DU=on
	RegSegmentOn[0] |=0x2;
	else
		{
	RegSegmentOn[0] &=0xfd;
	if(NumberBlok==1)
			{
	if(!(RomReceiveRS[2][2] & 1))
	RegSegmentMig[0] |=0x2;
			}
	else if(NumberBlok==2)
			{
	if(!(RomReceiveRS[2][1] & 1))
	RegSegmentMig[0] |=0x2;
			}

		}

	if(RegimWork & 0x200)//FVU=on
	RegSegmentOn[0] |=0x1;
	else
	RegSegmentOn[0] &=0xfe;

	if(RegimWork & 0x400)//Fire=on
	RegSegmentOn[2] |=0x1;
	else
	RegSegmentOn[2] &=0xfe;

	if(RegimWork & 0x800)//Revun=off
	RegSegmentOn[2] |=0x2;
	else
	RegSegmentOn[2] &=0xfd;

	}	
   void SetRegSegmentMig (void)
    {




	if(RomReceiveRS[4][0] & 0xa0)// Error DG1,MSHU
	RegSegmentMig[2] |=0x20;
	else
	RegSegmentMig[2] &=0xdf;

	if(RomReceiveRS[4][0] & 0xc0)// Error DG2,MSHU
	RegSegmentMig[3] |=0x1;
	else
	RegSegmentMig[3] &=0xfe;



	if(RomReceiveRS[4][0] & 0x10)// Izol
	RegSegmentMig[2] |=0x4;
	else
	RegSegmentMig[2] &=0xfb;

	if(RomReceiveRS[4][0] & 0x8)// Pogar
	RegSegmentMig[3] |=0x4;
	else
	RegSegmentMig[3] &=0xfb;


	}	

	void	ResetError(void)
	{
	unsigned char R0;

	Error=0;
	ErrorInf=0;
	for(R0=0;R0<=2;++R0)
	CtErrorGlobalRS[R0]=CtErrorGlobalRS0;

	}			
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=2;++R0)
		{
	if(!CtErrorGlobalRS[R0])
	Error |=(1<<R0);
		}

	}
	void ControlPCH(void)
	{
	unsigned int R0;
	R0=RegimWork;
	if(R0 & 4)//PCH1=on
		{
	if(RomReceiveRS[2][0] & 4)//Avaria PCH1
			{
	RegTransmitRS[4]|=2;
	R0 |=8;//PCH2=on
	R0 &=0xfffb;//PCH1=off
	
			}
		}
	if(R0 & 8)//PCH2=on
		{
	if(RomReceiveRS[2][0] & 8)//Avaria PCH2
			{
	RegTransmitRS[4] |=1;
	R0 |=4;//PCH1=on
	R0 &=0xfff7;//PCH2=off
			}
		}
	if(RegSWork & 0x20)//PCH1
		{
	if(R0 & 4)
			{		
	R0 &=0xfffb;
	RegTransmitRS[4] &=0xfe;
			}
	else
	R0|=4;
	RegSWork &=0xffdf;
		}		
	if(RegSWork & 0x40)//PCH2
		{
	if(R0 & 8)
			{		
	R0 &=0xfff7;
	RegTransmitRS[4] &=0xfd;
			}
	else
	R0 |=8;
	RegSWork &=0xffbf;
		}
	if(RomReceiveRS[2][0] & 4)//Avaria PCH1
			{
	R0 &=0xfffb;//PCH1=off
	RegTransmitRS[4] &=0xfe;

			}

	if(RomReceiveRS[2][0] & 8)//Avaria PCH2
			{
	R0 &=0xfff7;//PCH2=off
	RegTransmitRS[4] &=0xfd;
			}
	RegimWork=R0;


	}

	void ChangeRegim(void)
	{
	if(RegSWork & 0x10)//DU=on
		{
	if(RegimWork & 0x100)		
	RegimWork &=0xfeff;
	else
	RegimWork |=0x100;
	RegSWork &=0xffef;
		}

	if(RegimWork & 0x100)
	{
	if(RegSWork & 0x1000)//pusk
		{
	if(RegimWork & 1)		
	RegimWork &=0xfffe;
	else
	RegimWork |=1;
	RegSWork &=0xefff;
		}
	if(RomReceiveRS[1][0] & 0x40)//Rezerv=on
		{
	if(!(RomReceiveRS[1][0] & 0x20))//KS=off
			{
	if(!(RomReceiveRS[2][0] & 0x1))//AvariaDG1=off
	RegimWork |=2;
	else
				{
	RegimWork &=0xfffd;
	if(!(RomReceiveRS[2][0] & 0x2))//AvariaDG2=off
	RegimWork |=0x10;
	else
	RegimWork &=0xffef;
				}
			}
	if(RomReceiveRS[1][0] & 0x10)//set norma
	RegimWork |=0x20;
	else
	RegimWork &=0xffdf;
		}
	else
		{
	if(RomReceiveRS[2][0] & 0x1)//Avaria DG1
			{

	if((RegimWork & 2)&&(!(RomReceiveRS[2][0] & 0x2)))//Avaria DG2==0ff
	RegimWork |=0x10;//Pusk DG2
	RegimWork &=0xfffd;
			}

	else if(RegSWork & 0x2)//DG1
			{
	if(RegimWork & 2)		
	RegimWork &=0xfffd;
	else
				{
	RegimWork |=2;

	if(!(RomReceiveRS[1][0] & 0x20))//Set=off
	RegimWork &=0xffdf;
				}
	RegSWork &=0xfffd;
			}

	if(RomReceiveRS[2][0] & 0x2)//Avaria DG2
			{
	if((RegimWork & 0x10)&&(!(RomReceiveRS[2][0] & 0x1)))//Avaria DG1==0ff
	RegimWork |=0x2;//Pusk DG1

	RegimWork &=0xffef;
			}

	else if(RegSWork & 0x4)//DG2
			{
	if(RegimWork & 0x10)		
	RegimWork &=0xffef;
	else
				{
	RegimWork |=0x10;

	if(!(RomReceiveRS[1][0] & 0x20))//Set=off
	RegimWork &=0xffdf;
				}
	RegSWork &=0xfffb;

			}		
		}				
	ControlPCH();		
		
	if(RegSWork & 0x80)//Set
		{
	if(RegimWork & 0x20)		
	RegimWork &=0xffdf;
	else
			{
//	if(!(RomReceiveRS[1][0] & 0x80))
	RegimWork |=0x20;

	if((RomReceiveRS[1][0] & 1)&&(RomReceiveRS[1][0] & 2))//DG1,2=on
	RegimWork &=0xffef;//DG2=off
	else 
				{
	if(!(RomReceiveRS[1][0] & 1))//DG1=off
	RegimWork &=0xfffd;
	if(!(RomReceiveRS[1][0] & 2))//DG2=off
	RegimWork &=0xffef;

				}


			}
	RegSWork &=0xff7f;
		}

	if(RegSWork & 0x100)//RU50
		{
	if(RegimWork & 0x40)		
	RegimWork &=0xffbf;
	else
	RegimWork |=0x40;
	RegSWork &=0xfeff;
		}		
	if(RegSWork & 0x8)//Rezerv Set
		{
	if(RegimWork & 0x80)		
	RegimWork &=0xff7f;
	else
	RegimWork |=0x80;
	RegSWork &=0xfff7;
		}		
		
	if(RegSWork & 0x1)//FVU=on
		{
	if(RegimWork & 0x200)		
	RegimWork &=0xfdff;
	else
	RegimWork |=0x200;
	RegSWork &=0xfffe;
		}
	if(RegSWork & 0x800)//Fire=on
		{
	if(RegimWork & 0x400)		
	RegimWork &=0xfbff;
	else
	RegimWork |=0x400;
	RegSWork &=0xf7ff;
		}
		
				
	if(RegSWork & 0x8000)//Revun=off
		{
	if(RegimWork & 0x800)		
	RegimWork &=0xf7ff;
	else
	RegimWork |=0x800;
	RegSWork &=0x7fff;
		}
	}		
	
	else
	RegSWork &=0x10;		
		
						
					
	}

	void	SetRegTransmitRS(void)
	{
	unsigned char R0;
	R0=0;
	RegTransmitRS[1]=RegimWork;
	R0=((RegimWork>>8) & 0xf);
	if(RegS & 0x200)
	R0 |=0x10;
	if(RegS & 0x400)
	R0 |=0x20;
	if(RegS & 0x2000)
	R0 |=0x40;
	if(RegS & 0x4000)
	R0 |=0x80;

	RegTransmitRS[2]=R0;
	RegTransmitRS[3]=Error;
	
	}	
	
	void	ClearRomReceive(void)
	{
	unsigned char R0;
	for(R0=0;R0<=7;++R0)
		{
	RomReceiveRS[0][R0]=0;			
	RomReceiveRS[1][R0]=0;	
	RomReceiveRS[2][R0]=0;			
		}
	}
	void	ControlRevun(void)
	{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	R0=RomReceiveRS[3][0];
	R1=RomReceiveRS[2][0];
	R2=RomReceiveRS[4][0];
	if(RegimWork & 0x800)//Revun=off
	PORTD |=0x10;
	else if((R0 & 3)||(R1 & 3)||(R2 & 0xf8))
	PORTD &=0xef;
	else
	PORTD |=0x10;


	}	
	
																			     						    
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	DDRA=0;
	PORTA=0xf4;
    DDRB=0xf6;
	PORTB |= 0xf3;// Control UAB

    DDRC=0xf;
    PORTC |=0xff;


    DDRD=0x1a;
    PORTD=0xff;/*Reset=on*/

//  INIT SPI
    SPSR=0;//f/64
	SPCR=0x72;
	SPCR |=0x80;//enable Int SPI
//INIT USART
	InitUsart();


    TCCR1A=0;//0x82;
    TCCR1B=0x2;//0.5mks


 //   TIMSK=TIMSK | 0x20;//enable Int capture1 

    TIMSK=TIMSK | 0x4;//enable Int overlowT1 

		
    /*Interrupt1*/
//    MCUCR=0x8;// Log1>0 Int1

 
//    GICR |=(1<<INT1);/*Enable INT1*/

 
   	/*Timer0*/
 	TCCR0=0x61;//0x61;//1;//1;
	OCR0=0xe0;
//    TIMSK |=1;/*Enable Int Overlow Timer0*/

   	/*Timer2*/
 	TCCR2=0x4;
    TIMSK |=(1<<TOIE2);/*Enable Int Overlow Timer2*/

	CtSetka=0;

    _SEI();

 	RegimWork=0;

	ResetError();
	CtStart=100;
//	ClearRomReceive();

  /*Work program*/     	 
    while(1)
    {

    _WDR();
	if(CtStart)
		{
	




	if(RomReceiveRS[1][0] & 1)//DG1=on
	RegimWork |=2;
	else
	RegimWork &=0xfffd;
	if(RomReceiveRS[1][0] & 2)//DG2=on
	RegimWork |=0x10;
	else
	RegimWork &=0xffef;
	if(RomReceiveRS[1][0] & 4)//PCH1=on
	RegimWork |=4;
	else
	RegimWork &=0xfffb;
	if(RomReceiveRS[1][0] & 8)//PCH2=on
	RegimWork |=8;
	else
	RegimWork &=0xfff7;
	if(RomReceiveRS[1][0] & 0x20)//AvtSet=on
	RegimWork |=0x20;
	else
	RegimWork &=0xffdf;
	if(RegimWork & 0x3e)
	RegimWork |=0x1;
	else
	RegimWork &=0xfffe;
	if(RegS1 & 1)
	NumberBlok=2;
	else
	NumberBlok=1;
		}
	else
		{

	if(NumberBlok==1)
			{
	if((RomReceiveRS[2][2] & 1)&&(!(Error & 4)))
				{
	RegimWork=(RomReceiveRS[2][2]<<8) & 0xeff;
	RegimWork |=RomReceiveRS[1][2];

				}
	else
	ChangeRegim();

			}
	else
			{
	if((RomReceiveRS[2][1] & 1)&&(!(Error & 2)))
				{
	RegimWork=(RomReceiveRS[2][1]<<8) & 0xeff;
	RegimWork |=RomReceiveRS[1][1];
				}
	else
	ChangeRegim();
			}
		}
	SetError();
	SetRegTransmitRS();



		{
	SetRegSegmentOn();
	SetRegSegmentMig();
		}
	IndicatorSegment();
	ControlRevun();

	}

}

 SIGNAL(SIG_OVERFLOW2)/*128mks*/
{
	TCNT2=0;

	if(!MaskaSetka)
	{

	MaskaSetka=1;


	}
	else
	{
	MaskaSetka=0;

	if(CtSetka<=3)
	++CtSetka;
	else
	CtSetka=0;

	CtByteAnod=0;
	SPCR |=0x80;//enable Interrupt
	PORTB |=2;
 	SPDR=Segment[CtSetka];

	}
}








SIGNAL(SIG_SPI)
{

	if(!CtByteAnod)
	{
	++CtByteAnod;	

	switch(CtSetka)
		{
	case 0:SPDR=0xfe;
	break;
	case 1:SPDR=0xfd;
	break;
	case 2:SPDR=0xfb;
	break;
	case 3:SPDR=0xf7;
	break;
	case 4:SPDR=0xef;
	break;
	case 5:SPDR=0xdf;
	break;
	case 6:SPDR=0xbf;
	break;
	case 7:SPDR=0x7f;
	break;



	default:break;

		}


	}


	else
	{
//	if(EnableInd)
	PORTB &=0xfd;
	SPCR &=0x7f;//denable interrupt SPI

	TCNT2=0xfe;
	TIFR |=0x40;//reset flag
	TIMSK |=0x40;
	TCCR2=0x4;
	MaskaSetka=0;
	}

}

 SIGNAL(SIG_OVERFLOW1)//32.0 Mc
    {
	unsigned char R0;
	if(RegimWork & 0x400)
		{
	if(!CtFire)
	RegimWork &=0xfbff;

	else
	--CtFire;
		}
	else
	CtFire=2000;

	if(CtKn)
	--CtKn;
	if(CtStart)
	--CtStart;
	for(R0=0;R0<=2;++R0)
		{
	if(CtErrorGlobalRS[R0])
	--CtErrorGlobalRS[R0];
		}
	++TestRS;


	if(CtMig)
	--CtMig;
	else
	CtMig=CtMig0;
	if(CtUsart)
	--CtUsart;
	else
		{
	TransmitUsart();
	CtUsart=NumberBlok+6;
		}

	if(CtErrorRS)
	--CtErrorRS;
	else
		{
	InitUsart();
	CtErrorRS=NumberBlok+12;
		}



	if(CtEnableKn)
	--CtEnableKn;
	else
	CtEnableKn=7;
	switch(CtEnableKn)
		{

	case 0:ReadKn();

	break;
	case 1:	PORTC &=0xfe;
	PORTC |=0xe;

	break;
	case 2:ReadKn();


	break;
	case 3:	PORTC &=0xfd;
	PORTC |=0xd;

	break;
	case 4:ReadKn();

	break;
	case 5:	PORTC &=0xfb;
	PORTC |=0xb;

	break;
	case 6:ReadKn();


	break;
	case 7:	PORTC &=0xf7;
	PORTC |=0x7;

	break;


	default :CtEnableKn=3;
	break;
		}

    }
	SIGNAL(SIG_UART_RECV)
{
	LinkPult();

}
