
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
	unsigned char RegIndicator;
	unsigned char MaskaInd;
	unsigned char CtChangeInd;
	unsigned char RegIndicator;
	unsigned char CtIndAd;
	unsigned char RevunOff;
	unsigned    int 	CtRegim[4][3];
	const unsigned    int 	CtRegim0=33;//66;//2sec
	const unsigned    int 	CtRegim1=500;//1000;//30sec
	const unsigned    int 	CtRegim2=150;//300;//10sec
	const unsigned    int 	CtRegim3=5000;//10000;//300sec

	unsigned    char 	Regim[3];
	unsigned    int 	RegimError;
	unsigned    char	CtRevun;
    const unsigned    char		CtRevun0=16;//33;
	unsigned    char	MyStatus;
	unsigned    char	NumberAd;

    const unsigned    char		NumberAdNew[5]={4,4,4,2,3};
	unsigned    char    IndData[4];
    const unsigned  char Table[0Xa]={0X3f,0X6,0X5b,0X4f,0X66,
                                      0X6d,0X7d,0X7,0X7f,0X6f};
   unsigned    int	CtAd;

	static const unsigned    int	    CtAd0=1024;//3msec
    unsigned    int            TestAd[5]; 
    unsigned    int            AdTempMin;
    unsigned    int            AdTempMax;
	unsigned    int	AdTemp[5];
	unsigned    int	AdResult[5];

    unsigned char CtKnKn;
    unsigned char CtKn;
    unsigned char CtStart;
    unsigned char TestRS;//RegSegmentMig[4];
    unsigned char RegSegmentMig[4];
    unsigned char RegSegmentOn[4];

    unsigned char CtErrorRS;
    unsigned char NumberBlok;
    unsigned char NumberReceiveRS;
	volatile    unsigned char CtError[16];
 //   const unsigned char CtError0[16]={30,30,30,30,30,30,30,30,
//										30,30,30,30,30,30,30,30};

    const unsigned char CtError0[16]={15,15,15,15,15,15,15,15,
										15,15,15,15,15,15,15,15};
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
    const    unsigned char        CtMig0=30;//60;
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
   void IndicatorSegment  (void)
    {
	Segment[0]=Table[IndData[1]];
	Segment[1]=Table[IndData[0]];	


	}
 	void		DecSegm( unsigned int Arg2)
    {
     unsigned int	Arg1;
     unsigned int	Arg0;
	 Arg0=Arg2;
	if(Arg0>99)
	Arg0=99;

	Arg1=0;

	while(Arg0>=10)
		{
	Arg0=Arg0-10;
	++Arg1;
		}

	IndData[1]=Arg1;
	Arg1=0;


	IndData[0]=Arg0;	
    }




	  





     void    ReadKn(void)
    {

    unsigned    int R0;
    unsigned    char R1;
    R0=RegSTemp;
    if(bit_is_clear(PINC,PC0))
    R0 |=0x1;
	else
    R0 &=0xfffe;
    if(bit_is_set(PINC,PC6))
    R0 |=0x2;
	else
    R0 &=0xfffd;
    if(bit_is_set(PIND,PD5))
    R0 |=0x4;
	else
    R0 &=0xfffb;
    if(bit_is_clear(PINB,PB2))
    R0 |=0x8;
	else
    R0 &=0xfff7;
    if(bit_is_clear(PINB,PB3))
    R0 |=0x10;
	else
    R0 &=0xffef;
    if(bit_is_set(PINB,PB0))
    R0 |=0x20;
	else
    R0 &=0xffdf;
    if(bit_is_clear(PINC,PC7))
    R0 |=0x40;
	else
    R0 &=0xffbf;
    if(bit_is_clear(PINC,PC2))
    R0 |=0x80;
	else
    R0 &=0xff7f;

    if(bit_is_clear(PINC,PC4))
    R0 |=0x100;
	else
    R0 &=0xfeff;
    if(bit_is_clear(PIND,PD3))
    R0 |=0x200;
	else
    R0 &=0xfdff;
    if(R0==RegSTemp)
		{
	RegSOld=RegS;
    RegS=R0;
	if(RegS & 0x26)
	RegSWork=RegS;
	if(RegS & 0x10)//Otmena
	RegSWork &=0xdf;
	if((RegS & 0x40)&&(!(RegSOld & 0x40)))
		{
	if(RevunOff)
	RevunOff=0;
	else
	RevunOff=1;
		}


	for(R1=0;R1<=2;++R1)
		{
	if((RegS & 0x10)&&(!(RegSOld & 0x10)))
			{
	if(Regim[R1] & 8)
	Regim[R1] &=0xf7;
	else
				{
	if(Regim[R1])
					{
	Regim[R1] |=8;
	Regim[R1] &=0xf8;
					}
				}
			}
		}			
		}

    RegSTemp=R0;

    }


	void	ResetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
	CtError[R0]=CtError0[R0];

		}
	Error=0;



	}			
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}

	}


	void ChangeRegim(void)
	{
	unsigned int R0;
	R0=AdResult[2];
	if(Error & 0x30)
	CtRegim[0][0]=CtRegim0;//2sec
	else
		{
	if(Regim[0] & 8)
			{
	Regim[0] &=0xfc;
	RegSWork &=0xd9;
			}

	else if(!(Regim[0] & 3 ))//Norma
			{
	if((R0<500)||(R0>800))
	CtRegim[0][0]=CtRegim0;//2sec
	else if((!CtRegim[0][0])&&(!(Regim[0] & 8)))
	Regim[0]=1;

			}
	else
			{
	if(Regim[0]==1)
				{
	if(!CtRegim[1][0])
	Regim[0]=2;
				}
	else if(Regim[0]==2)
				{
	CtRegim[3][0]=CtRegim3;
	if(RegS & 0x8)
					{
	if(RegS & 0x6)
	Regim[0]=3;
	else if(!CtRegim[2][0])
	Regim[0]=3;
					}
				}
	else if(Regim[0]==3)
				{
	if(!CtRegim[3][0])
	Regim[0]=4;
				}
	else if(Regim[0]==4)
				{
	if(R0<500)
	Regim[0]=0;
				}

			}
		}
	R0=AdResult[3];
	if(Error & 0xc0)
	CtRegim[0][1]=CtRegim0;//2sec
	else
		{
	if(Regim[1] & 8)
			{
	Regim[1] &=0xfc;
	RegSWork &=0xd9;
			}

	else if(!(Regim[1] & 3 ))//Norma
			{
	if((R0<500)||(R0>800))
	CtRegim[0][1]=CtRegim0;//2sec
	else if((!CtRegim[0][1])&&(!(Regim[1] & 8)))
	Regim[1]=1;

			}
	else
			{
	if(Regim[1]==1)
				{
	if(!CtRegim[1][1])
	Regim[1]=2;
				}
	else if(Regim[1]==2)
				{
	CtRegim[3][1]=CtRegim3;
	if(RegS & 0x8)
					{
	if(RegS & 0x6)
	Regim[1]=3;
	else if(!CtRegim[2][1])
	Regim[1]=3;
					}
				}
	else if(Regim[1]==3)
				{
	if(!CtRegim[3][1])
	Regim[1]=4;
				}
	else if(Regim[1]==4)
				{
	if(R0<500)
	Regim[1]=0;
				}

			}
		}

	R0=AdResult[4];
	if(Error & 0x300)
	CtRegim[0][2]=CtRegim0;//2sec
	else
		{
	if(Regim[2] & 8)
			{
	Regim[2] &=0xfc;
	RegSWork &=0xd9;
			}

	else if(!(Regim[2] & 3 ))//Norma
			{
	if((R0<500)||(R0>800))
	CtRegim[0][2]=CtRegim0;//2sec
	else if((!CtRegim[0][2])&&(!(Regim[2] & 8)))
	Regim[2]=1;

			}
	else
			{
	if(Regim[2]==1)
				{
	if(!CtRegim[1][2])
	Regim[2]=2;
				}
	else if(Regim[2]==2)
				{
	CtRegim[3][2]=CtRegim3;
	if(RegS & 0x8)
					{

	if(RegS & 0x6)
	Regim[2]=3;
	else if(!CtRegim[2][2])
	Regim[2]=3;
					}
				}
	else if(Regim[2]==3)
				{
	if(!CtRegim[3][2])
	Regim[2]=4;
				}
	else if(Regim[2]==4)
				{
	if(R0<500)
	Regim[2]=0;
				}



			}
		}


	}
	void SetRegimError(void)
	{
	unsigned int R0;

	R0=0;
	if((PORTC & 2)&&(RegS & 1))//MPP1
	R0 |=1;
	if((PORTC & 8)&&(RegS & 0x80))//MPP2
	R0 |=2;
	if((PORTC & 0x20)&&(RegS & 0x100))//MPP3
	R0 |=4;
//	if((PORTD & 0x80)&&(RegS & 0x200))//Revun
//	R0 |=8;

	if(AdResult[2]>800)
	R0 |=0x10;
	else if(AdResult[2]<100)
	R0 |=0x20;
	if(AdResult[3]>800)
	R0 |=0x40;
	else if(AdResult[3]<100)
	R0 |=0x80;
	if(AdResult[4]>800)
	R0 |=0x100;
	else if(AdResult[4]<100)
	R0 |=0x200;

	RegimError=R0;
	}

	
void InitAd   (void)
    {
	NumberAd=4;
    ADMUX=0x40;//2.56=Ref
    SFIOR=SFIOR & 0x1f;//start from end AD
 	CtAd=CtAd0;


	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);

//	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }	
	void	IndicatorAd(unsigned char R0)
	{
	unsigned int R1;
	R1=AdResult[R0] & 0x3ff;
	R1 >>=4;
	DecSegm(R1);	
	IndicatorSegment();	
	}
	void	ControlRevun(void)
	{
	unsigned char R0;
	unsigned char R1;
	R1=0;
	for(R0=0;R0<=2;++R0)
		{
	if((Regim[R0]==2)||(Regim[R0]==3))
	R1=1;
		}
	if(!R1)
		{			
	for(R0=0;R0<=2;++R0)
			{
	if(Regim[R0]==1)
	R1=2;
			}		
		}
	if(R1==1)
		{
	PORTD &=0xfd;
	PORTD |=0x1;
		}
	else if(R1==2)
		{
	PORTD &=0xfe;
	PORTD |=0x2;
		}
	else 
		{

	PORTD |=0x3;
		}
	if(RevunOff)
	PORTD |=0x80;//off
	else if(Error)
	PORTD &=0x7f;//on
	else if(!R1)
	PORTD |=0x80;//off		
	else if(R1==1)
	PORTD &=0x7f;//on
	else if(R1==2)
		{											
	if(!CtRevun)
			{
	if(PORTD & 0x80)
	PORTD &=0x7f;
	else
	PORTD |=0x80;
	CtRevun=CtRevun0;
			}
		}

	}
	void	ControlNorma(void)
	{
	if(RegS & 0x30)
	Segment[2]=0;

	else if(!Error)
		{
	PORTD &=0xbf;
	Segment[2] |=1;
	Segment[2] &=0xfd;
		}
	else
		{
	PORTD |=0x40;
	Segment[2] |=2;
	Segment[2] &=0xfe;
		}
	}	
	void	ControlMPP(void)
	{
	if(Regim[0]==3)
	PORTC &=0xfd;
	else
	PORTC |=0x2;
	if(Regim[1]==3)
	PORTC &=0xf7;
	else
	PORTC |=0x8;
	if(Regim[2]==3)
	PORTC &=0xdf;
	else
	PORTC |=0x20;


	}	
	void	ControlPogar(void)
	{
	if((Regim[0])||(Regim[1])||(Regim[2]))
		{
	PORTD |=0x4;
	Segment[2] |=4;
		}
	else
		{
	PORTD &=0xfb;
	Segment[2] &=0xfb;
		}
	}	
	void	Indicator(void)
	{
	unsigned int R0;
	R0=Error & 0xf;
	if(Error & 0x30)
	R0 |=0x10;
	if(Error & 0xc0)
	R0 |=0x20;
	if(Error & 0x300)
	R0 |=0x40;

	if(Regim[0]>=1)
	R0 |=0x10;
	if(Regim[1]>=1)
	R0 |=0x20;
	if(Regim[2]>=1)
	R0 |=0x40;
	RegIndicator=R0;
	}							

	void	IndicatorAll(void)
	{
	const unsigned char Tab0[7]={6,0x4f,0x5b,0,6,0x4f,0x5b};
	const unsigned char Tab1[7]={0x4f,0x4f,0x4f,6,0x5b,0x5b,0x5b};
	if(RegIndicator)
		{
	Segment[0]=Tab0[MaskaInd];
	Segment[1]=Tab1[MaskaInd];
		}
	else
		{
	Segment[0]=0;
	Segment[1]=0;
		}

	}
	void	ControlPoroshok(void)
	{
	if((Regim[0]==1)||(Regim[1])||(Regim[2]))
		{
	PORTD &=0xfb;
	Segment[2] |=4;
		}
	else
		{
	PORTD |=4;
	Segment[2] &=0xfb;
		}
	}
																					     						    
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	DDRA=0;
	PORTA=0;
    DDRB=0xb2;
	PORTB |= 0xf3;// Control UAB

    DDRC=0x2a;
    PORTC |=0x2a;
    DDRD=0xc7;
    PORTD=0xff;/*Reset=on*/

//  INIT SPI
    SPSR=0;//f/64
	SPCR=0x72;
	SPCR |=0x80;//enable Int SPI

    InitAd();

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
	CtStart=25;//50;
	RevunOff=0;
//	ClearRomReceive();

  /*Work program*/     	 
    while(1)
    {
    _WDR();
	if(!CtIndAd)
		{
//	IndicatorAd(2);
	CtIndAd=15;//30;
		}
	ReadKn();
	if(RegS & 0x10)
	ResetError();

	ChangeRegim();
 	ControlRevun();
	ControlMPP();
	ControlPogar();
 	ControlNorma();
	Indicator();
	IndicatorAll();

	SetRegimError();
	SetError();
//	DecSegm(RegS);
//	IndicatorSegment();	
	if(Error)
	PORTD |=0x40;//Error
	else
	PORTD &=0xbf;//Norma


	}

}

 SIGNAL(SIG_OVERFLOW2)/*128mks*/
{
	unsigned char R0;
	TCNT2=0;

	if(!MaskaSetka)
	{

	MaskaSetka=1;


	}
	else
	{
	MaskaSetka=0;

	if(CtSetka<=1)
	++CtSetka;
	else
	CtSetka=0;

	CtByteAnod=0;
	R0=SPSR;
	SPCR |=0x80;//enable Interrupt
	PORTB |=2;
 	SPDR=Segment[CtSetka];

	}
}








SIGNAL(SIG_SPI)
{
	unsigned char R0;
	if(!CtByteAnod)
	{
		++CtByteAnod;	

	switch(CtSetka)
		{
	case 0:SPDR=0x6;
	break;
	case 1:SPDR=0x5;
	break;
	case 2:SPDR=0x3;
	break;




	default:break;

		}


	}


	else
	{
//	if(EnableInd)
	CtByteAnod=0;
	PORTB &=0xfd;
	R0=SPSR;
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
	if(RegIndicator)
		{
	if(CtChangeInd)
	--CtChangeInd;
	else
			{
M1:	if(MaskaInd<=5)
	++MaskaInd;
	else
	MaskaInd=0;
	if(RegIndicator &(1<<MaskaInd))
	CtChangeInd=60;
	else
	goto M1;
			}	
		}

	if(CtIndAd)
	--CtIndAd;
	for(R0=0;R0<=2;++R0)
		{
	if(CtRegim[0][R0])
	--CtRegim[0][R0];
	if(Regim[R0]==1)
			{
	if(CtRegim[1][R0])
	--CtRegim[1][R0];
			}
	else
	CtRegim[1][R0]=CtRegim1;

	if((RegSWork & 0x20)&&(Regim[R0] & 3))
			{
	if(CtRegim[2][R0])
	--CtRegim[2][R0];
			}
	else
	CtRegim[2][R0]=CtRegim2;
	if(CtRegim[3][R0])
	--CtRegim[3][R0];
		}		
	if(CtRevun)
	--CtRevun;

	if(CtKn)
	--CtKn;
	if(CtStart)
	--CtStart;
	for(R0=0;R0<=15;++R0)
		{
	if(RegimError &(1<<R0))
			{
	if(CtError[R0])
	--CtError[R0];
			}
	else
	CtError[R0]=CtError0[R0];
		}
	++TestRS;


	if(CtMig)
	--CtMig;
	else
	CtMig=CtMig0;
    }

SIGNAL(SIG_ADC)
{
    unsigned int RegInt0;

    RegInt0=ADC;


	if(CtAd)
	{
	--CtAd;
	if(CtAd<=CtAd0)
		{  	
	if(RegInt0>=AdTemp[NumberAd])
	AdTemp[NumberAd]=RegInt0;
		}
	}
	else
	 {
	 AdResult[NumberAd]=AdTemp[NumberAd];

	 CtAd=CtAd0+2;
	 MyStatus=MyStatus | 1;
	 NumberAd=NumberAdNew[NumberAd];
	 AdTemp[NumberAd]=0;
	 }

	RegInt0=ADMUX;
	RegInt0 &=0xe0;
	RegInt0 |=NumberAd;
	ADMUX=RegInt0;

	 ADCSRA |=(1<<ADSC);
  	
}
