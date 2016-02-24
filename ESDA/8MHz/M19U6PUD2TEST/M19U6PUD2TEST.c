
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>

    unsigned char UABError;
   	unsigned int CtChange;
    unsigned char RegimTest;
    unsigned char CtSicle;
    unsigned char CtWellKn;
   	unsigned int NDizMax;


   	unsigned char RegimStop;

   	unsigned char DelError;
   	unsigned int DPStart;
   	unsigned char CtStart;


volatile   	unsigned char CtEnableKn;


volatile       unsigned int UABOld;
volatile           unsigned char CtErrorRS;
volatile           unsigned int UABNew;
	      unsigned char NumberBlok;
volatile           unsigned char NumberReceiveRS;

volatile    unsigned char CtUsart;


volatile        unsigned char RegTransmitRS[9];
volatile       unsigned char RamReceiveRS[9];
volatile        unsigned char RomReceiveRS[9];

volatile           unsigned char	ControlTransmitRS;
volatile           unsigned char ControlReceiveRS;


	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=8;
volatile           unsigned int DPOld;
volatile    unsigned char CtChangeAlfa;
    unsigned int URef;
volatile           unsigned char CtTest;
     unsigned char BitTest;

    unsigned char TabMaxAlfa[17];
    unsigned char TabMinAlfa[17];
    unsigned    int    CtDif;
    unsigned    char    CtPlusPM;
    unsigned    char    CtMinusPM;
    unsigned    char   PM;

    unsigned    char    CtPlusTOG;
    unsigned    char    CtMinusTOG;

    unsigned    int    NNomStatizm;

    unsigned    int      DPNomMax;
    const unsigned    int      DPNomMin=200;
    const unsigned    int      DPNomMinStab0=400;

    const unsigned    int      DPNomMinStab100=350;
    unsigned    int      DPNomMinStab;

    const unsigned    int      NDizRaznos=1750;
    unsigned    char           RegimStatizm;
    unsigned    int            NNomConst;
//	const    unsigned    char           Statizm=3;

    unsigned    int            AdTempMin[4];
    unsigned    int            AdTempMax[4];


volatile             unsigned    int            NDizNew;
volatile               unsigned    int            NDizOld;
volatile              unsigned    char            TestO;
volatile              unsigned    char            TestODin;
    unsigned    int            NDizNewDin;
    unsigned    int            NDizOldDin;

	const unsigned    int       DeltaDP=270;//300;//340;//400;//370;//370;//370;



volatile                unsigned    char            NDizEnd;


	const unsigned    int       DPNom0=350;
volatile            	unsigned    int         	DPNom;

//	unsigned    int         	TestNDiz;
   	unsigned    int         	NNom;

volatile                unsigned    char            CtNumberInd;

	unsigned    int         	RegimError;
 volatile                 	unsigned    char         	CtError[16];
	const unsigned    char         	CtError0[16]={250,2,
									250,100,60,30,150,25,25,25,250,
									2,60,200,100,25};
	unsigned    char         	RegimTime;

	unsigned    char         	CtTime[2];
	const unsigned    char         	CtTime0=6;


	unsigned    int         	Error;
	unsigned    char         	TOG;
volatile         	unsigned    int         	DP;

volatile       unsigned    char            CtOverLow;
	unsigned    int         	ICR1Old;
	unsigned    int             NDiz;
	unsigned    long            TDiz;
volatile    unsigned    int             TDizTemp[16];
   unsigned    char            CtTDiz;


	unsigned    int             RegSOld;

	volatile 	unsigned    char    CtInd;


	volatile     unsigned    char             RegimWork;


	unsigned char Segment[5];
	volatile unsigned char CtByteAnod;
	unsigned char CtSetka;

 	unsigned    char    MaskaSetka;


	unsigned    char    IndData[4];

    const unsigned  char Table[0Xa]={0X3f,0X6,0X5b,0X4f,0X66,
                                      0X6d,0X7d,0X7,0X7f,0X6f};

	unsigned    char	NumberAd;
	unsigned    char	NumberAdSlow;
	unsigned    char	CtAd[4];
	const unsigned    char	    CtAd0=7;

	unsigned    int	AdResult[4];

	unsigned    int	RegS;
	unsigned    int	RegSTemp;
	volatile 	 	unsigned    char	            Alfa;

       
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


   UCSRA =1;//only 9bit=1
   UCSRB=0x1c;//enable transmit 9bit
   UBRRH=0;
   UBRRL=103;

	UCSRC=(1<<7)|(1<<2)|(1<<1);
   UCSRB |=0x80;//enable int receive
   PORTB &=0xfb;//Out Transmit=off


	}	
	void	TransmitUsart(void)
	{
	volatile unsigned char R0;

   PORTB |=0x4;//Out Transmit=on
   R0=100;
   while(R0)
   --R0;
     while(!(UCSRA & 0x20))
   _WDR();

    R0=NumberBlok;//LinkRS;	
	UCSRA |=1;//Receive only 9 bit=1
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
	unsigned char R4;

	if(UCSRA &(1<<RXC))
	{
	PORTB &= 0xfb;//Transmit denable
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
	PORTB |= 0x4;//Transmit enable
	R0=100;
	while(R0)
	--R0;

     while(!(UCSRA & 0x20))
   _WDR();
    R1=RegTransmitRS[CtReceiveRS];
	UCSRB &=0xfe;
    UDR=R1;

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
	PORTB |= 0x4;//Transmit enable
     while(!(UCSRA & 0x20))
   _WDR();

   if(!CtReceiveRS)
   				{

    R1=ControlTransmitRS;	
	UCSRB &=0xfe;
    UDR=R1;
				}
	else
				{
    R1=RegTransmitRS[CtReceiveRS];
	UCSRB &=0xfe;
    UDR=R1;
	ControlTransmitRS +=RegTransmitRS[CtReceiveRS];
				}

			}
		}
	else
		{

	UCSRA |=1;
	PORTB &= 0xfb;//Transmit denable
	if(R0==ControlReceiveRS)
			{



//	if(NumberReceiveRS != NumberBlok)
	CtErrorRS=39;

	if(NumberReceiveRS==(NumberBlok+1))//shu
					{

	CtError[14]=CtError0[14];
	Error &=0xbfff;	
	for(R4=1;R4<=7;++R4)
						{
	RomReceiveRS[R4]=RamReceiveRS[R4];
	RamReceiveRS[R4]=1;
						}

	
					}



			}



		}

	}
}

 

     void    ReadKnPC0(void)
	 {
    unsigned    int R0;

    R0=RegSTemp & 0xd07c;
    if(bit_is_set(PIND,PD5))
    R0 |=0x1;
    if(bit_is_set(PIND,PD7))
    R0 |=0x2;

    if(bit_is_clear(PINA,PA4))
    R0 |= 0x80;

    if(bit_is_clear(PINA,PA5))
    R0 |= 0x100;

    if(bit_is_clear(PINA,PA6))
    R0 |= 0x200;

    if(bit_is_clear(PINA,PA7))
    R0 |= 0x400;

    if(bit_is_clear(PINA,PA2))
    R0 |= 0x800;

    if(bit_is_clear(PINB,PB0))
    R0 |=0x2000;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
	if(!CtWellKn)
    RegS=R0;
		}
 	RegSTemp=R0;
	}




	void ReadKnPC5(void)
	{
    unsigned    int R0;
 
    R0=RegSTemp & 0xfff3;
    if(bit_is_set(PIND,PD5))
    R0 |=0x4;
    if(bit_is_set(PIND,PD7))
    R0 |=0x8;
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
 
    R0=RegSTemp & 0xffcf;
    if(bit_is_set(PIND,PD5))
    R0 |=0x10;
    if(bit_is_set(PIND,PD7))
    R0 |=0x20;
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
 
    R0=RegSTemp & 0xefbf;
    if(bit_is_set(PIND,PD5))
    R0 |=0x1000;
    if(bit_is_set(PIND,PD7))
    R0 |=0x40;
    if(R0==RegSTemp)
		{
	if(CtWellKn)
	--CtWellKn;
		}
 	RegSTemp=R0;
	}
 	void		DecSegm( unsigned int Arg2)
    {
     unsigned int	Arg1;
     unsigned int	Arg0;
	 Arg0=Arg2;
	if(Arg0>9999)
	Arg0=9999;

	Arg1=0;

	while(Arg0>=1000)
		{
	Arg0=Arg0-1000;
	++Arg1;
		}

	IndData[3]=Arg1;
	Arg1=0;

	while(Arg0>=100)
		{
	Arg0=Arg0-100;
	++Arg1;
		}

	IndData[2]=Arg1;
	Arg1=0;

	while(Arg0>=10)
		{
	Arg0=Arg0-10;
	++Arg1;
		}

	IndData[1]=Arg1;
	IndData[0]=Arg0;	
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




    unsigned int    ReadEepromWord(unsigned char Arg6)
	
    {
    unsigned int Arg7;
    Arg7=ReadEeprom(Arg6+1);		 	
    Arg7=Arg7<<8;
    Arg7=Arg7 | ReadEeprom(Arg6);   	
   	return Arg7;
    }



    void InitAd   (void)
    {
    ADMUX=0xc0;
 	CtAd[0]=CtAd0;
 	CtAd[3]=CtAd0;
	NumberAdSlow=0;
	NumberAd=0;
	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);

	ADCSRA |=(1<<ADPS0); /*128*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }
	void CalcTOG(void)
	{
	unsigned long R0;
	unsigned int R1;
	R0=AdResult[0];
	R0 *=URef;
	R0 >>=8;
	R1=R0;

	if(R1>=535)
	R1=R1-535;
	else
	R1=0;
	R0=R1;
	R0 *=108;
	R0 >>=8;
	R1=R0;
	if(R1>TOG)
		{
	CtMinusTOG=50;

	if(!CtPlusTOG)

			{
	++TOG;
	CtPlusTOG=5;
			}
		}
	else if(R1<TOG)
		{
	CtPlusTOG=50;

	if(!CtMinusTOG)

			{
	CtMinusTOG=5;
	--TOG;
			}
		}
	if(TOG>200)
	RegimError |=0x100;
	else if(TOG < 5)
	RegimError |=0x100;
	else
	RegimError &=0xfeff;

							
	}
		
	
	void CalcPM(void)
	{
	unsigned long R2;
	unsigned long R3;
	unsigned int R0;
	unsigned int R1;
	if(AdResult[3]>=900)
	RegimError |=0x200;
	else if(AdResult[3]<=100)
	RegimError |=0x200;
	else
	RegimError &=0xfdff;

	R0=AdResult[3];

	R1=R0;
	CtSicle=R1;
	if(R1>PM)
		{
	CtMinusPM=50;

	if(!CtPlusPM)

			{
	++PM;
	CtPlusPM=2;
			}
		}
	else if(R1<PM)
		{
	CtPlusPM=50;

	if(!CtMinusPM)

			{
	CtMinusPM=2;
	--PM;
			}
		}
	R2=AdResult[3];
	R3=URef;
	R2 *=R3;
	R2=R2>>8;
	PM=R2;//AdResult[3];
							
	}		
	
	
   void IndicatorSegment  (void)
    {
	Segment[0]=Table[IndData[3]];
	Segment[1]=Table[IndData[2]];	
	Segment[2]=Table[IndData[1]];
	Segment[3]=Table[IndData[0]];
	}	
	
   void IndicatorTOG  (void)
    {
	unsigned int R0;
	if(TOG>=50)
		{
	R0=TOG -50;			
	DecSegm(R0);
	IndicatorSegment();

		}
	else
		{
	R0=50-TOG;			
	DecSegm(R0);
	IndicatorSegment();
	Segment[1]=0x40;
		}
	Segment[0]=0x63;		
								
	}			


   void IndicatorPM  (void)
    {
	DecSegm(PM);
	IndicatorSegment();
	Segment[0]=0x73;
	Segment[2]=Table[IndData[1]] | 0x80;
	}
	



   void IndicatorN  (unsigned char R0)
    {
	unsigned int R1;
	switch(R0)
		{
	case 0:
	PORTD &=0xf7;	
	R1=NDiz;
	Alfa=180;
	OCR0=Alfa;

	break;
	case 1:
	PORTD &=0xf7;

	R1=URef+1000;
	Alfa=0xff;
	OCR0=Alfa;
	break;
	case 2:R1=PM+2000;
	Alfa=0xff;
	PORTD |=8;
	OCR0=Alfa;
	break;
	case 3:R1=TOG+3000;
	Alfa=165;
	PORTD |=8;
	OCR0=Alfa;
	break;
	case 4:R1=DP+4000;
	Alfa=160;
	OCR0=Alfa;
	break;
	case 5:R1=(RegS & 0xff)+5000;
	PORTC |=0x1e;
	Alfa=155;
	OCR0=Alfa;

	break;
	case 6:R1=(RegS>>8)+6000;
	PORTC |=0x1e;
	Alfa=150;
	OCR0=Alfa;
	break;
	case 7:R1=UABOld+7000;
	Alfa=145;
	OCR0=Alfa;
	break;
	case 8:R1=(RegS & 0xff)+8000;
	Alfa=140;
	OCR0=Alfa;
	break;
	case 9:R1=UABError+9000;
	Alfa=140;
	OCR0=Alfa;
	break;
	default:R1=55+2000;
	break;

		}
	DecSegm(R1);
	IndicatorSegment();						
	}

	





	
	

 




    void StabNDiz(void)
    {
	int R0;
	int R1;



	R1=NDizNew-NDizOld;

	R0=NDiz-NNom;


	if(abs(R1)>40)
	R1=R1<<1;

//	else if(abs(R1)>30)
//	R1=R1<<2;


	if(abs(R0)>30)
	R0=R0>>1;

	else
	R0=R0>>2;

	R1 +=R0;

	DPNom +=R1;


	if(DPNom>DPNomMax)
	DPNom=DPNomMax;
	if(DPNom<DPNomMinStab)
	DPNom=DPNomMinStab;

	
	}
    void StabNDizDin(void)
    {

	volatile	unsigned int R4;
	unsigned int R5;
	

	if((DP<DPNomMinStab100)||((DPNomMinStab100-DP)<30))
	DPNom=400;

	else if(NDizNewDin>=NDizOldDin)//++
			{
	R4=NDizNewDin-NDizOldDin;
	if(NDizNew>=NNom)
				{
	R5=NDizNew-NNom;
	if((R4>25)&&(R5>20)&&(!CtDif))

					{
	DPNom=DPNomMax-100;
	CtDif=500;
					}
				}
			}

	else
			{
	R4=NDizOldDin-NDizNewDin;//--
	if(NDizNew<NNom)
				{
	R5=NNom-NDizNew;
	if((R4>25)&&(R5>20)&&(!CtDif))
					{
	DPNom=DPNomMinStab100;
	DPNomMinStab=DPNomMinStab100;
	CtDif=500;
					}


				}
			}
										
	}

    void    AccountNDiz(void)
    {
    unsigned char R0;
    unsigned long R1;
    unsigned long R2;
    unsigned int R3[16];
    for(R0=0;R0<=15;++R0)
	R3[R0]=TDizTemp[R0];



	if(!CtOverLow)
	NDiz=0;
	else
	{
    TDiz=0;
	if(R3[0]>(R3[1]+1000))
		{
	if(R3[0]>(R3[15]+1000))
	R3[0]=R3[1];
		}
	if(R3[0]<(R3[1]-1000))
		{
	if(R3[0]<(R3[15]-1000))
	R3[0]=R3[1];
		}

	if(R3[15]>(R3[14]+1000))
		{
	if(R3[15]>(R3[0]+1000))
	R3[15]=R3[0];
		}
	if(R3[15]<(R3[14]-1000))
		{
	if(R3[15]<(R3[0]-1000))
	R3[15]=R3[0];
		}
    for(R0=1;R0<=14;++R0)
		{
		if(R3[R0]>(R3[R0+1]+1000))
			{
	if(R3[R0]>(R3[R0-1]+1000))
	R3[R0]=R3[R0+1];                                   
		    }
		if(R3[R0]<(R3[R0+1]-1000))
			{
	if(R3[R0]<(R3[R0-1]-1000))
	R3[R0]=R3[R0+1];                                   
		    }

		}
    for(R0=0;R0<=15;++R0)
	{
    TDiz +=R3[R0];
	_WDR();
	}
	R2=TDiz>>3;

//	R1=7272727/R2;
	R1=7682458/R2;
	NDiz=R1;
	if(NDiz<NDizMax)
	NDizMax=NDiz;
	if(RegS & 0x180)
	NDizMax=1600;

                                   
    }
	}
    void ChangeCtNumberInd(void)
{

    if((RegS & 0x200) && (!(RegSOld & 0x200)))
		{
    --CtNumberInd;

		}


	if(CtNumberInd>2)
	CtNumberInd=2;





}



	
    void ChangeRegimTest(void)
{
	unsigned char R0;
	R0=RegimTest;

    if((RegS & 0x80)&&(!(RegSOld & 0x80))) 
		{
    ++R0;

		}
    else if((RegS & 0x100)&&(!(RegSOld & 0x100)))
		{
    --R0;

		}
	if(R0>9)
	R0=0;
	RegSOld=RegS;
	RegimTest=R0;
}


    void ChangeRegimStatizm(void)
{

    if((RegS & 0x400)&&(!(RegSOld & 0x400))) 
		{
	if(RegimStatizm)
	RegimStatizm=0;
	else
	RegimStatizm=1;

		}

 
}
/*    void AccountNNomStatizm(void)
{
	unsigned int R0;
	R0=RomReceiveRS[7]; 
	R0 >>=1;
	if(R0>45)
	R0=45;
	R0=45-R0;
	NNomStatizm=NNomConst+R0;


}*/
/*    void AccountNNomStatizm(void)
{
	unsigned int R0;
	unsigned int R1;
	unsigned int R2;

	R2=DPStart-210;
	if(DPNom<=R2)
	R0=R2-DPNom;//Nom;
	else
	R0=0;
	R0>>=2;

		{
	R1=(NNomConst+22)>>4;
	R1=R1*R0;
	R1 /=156;
	NNomStatizm=(NNomConst+22)-R1;
		}

}*/
    void AccountNNomStatizm(void)
{
	unsigned int R0;
	unsigned int R1;
	unsigned int R2;
	unsigned char R3;
	R3=RomReceiveRS[7];
	if(R3<40)
	R3=40;
	if(R3>50)
	R3=50;
	R2=DPStart-210;
	if(DPNom<=R2)
	R0=R2-DPNom;//Nom;
	else
	R0=0;
	R0>>=2;
	R1=(NNomConst+45)>>3;
	R1=R1*R0;
	R1 /=156;
	NNomStatizm=(NNomConst+R3)-R1;

}





	void	ControlRBS(void)
	{

	if(NDiz>400)
		{
	RegimTime |=1;

		}

	else
		{

	RegimTime &=0xfe;			
		}
	}


	void	SetErrorCT(void)
	{

	if((!(RegS & 4))&&(NDiz<120))
	RegimError |=8;
	else
	RegimError &=0xfff7;

	}

	void	SetErrorDP(void)
	{
	unsigned int R0;
//	if(DP>=DPNom)
//	R0=DP-DPNom;
//	else
//	R0=DPNom-DP;
	R0=abs(DP-DPNom);
		{
	 if((R0>200)&&(Alfa<200))
	RegimError |=4;
	else
	RegimError &=0xfffb;
		}

	}	
    void    IndicatorWork(void)
    {

	if(!CtInd)
		{


	CtInd=100;
	IndicatorN(RegimTest);			  
		}
			  
	}




	void	SetRegimWork(void)
	{
	unsigned int R0;
	unsigned int R1;

	R1=Error & 0xafcf;

	if(RomReceiveRS[5] & 0x10)//blokirovka
	R1 &=0x2c8f;

	if((RegimWork==1)||(RegimWork==2))
		{
	if(!R1) 
	ControlRBS();

		}

	switch(RegimWork)
		{
	case 0:
// wait Kn Pusk	

	if(R1)
			{
		RegimWork=4;
		break;
			}
	switch(BitTest)	
		{
	case 0:
	if(DP<30)
			{
	RegimError |=0x400;
	break;
			}
	if(!CtStart)
		{		
	if(Alfa>115) 
			{

	_WDR();
	if(DP<(DPStart-350))
	RegimError &=0xfbff;
	else
	RegimError |=0x400;
	if(CtTest)
	DPOld=DP;
	if(!CtChangeAlfa)
				{
	CtChangeAlfa=10;
	--Alfa;

				}


	if(DP<DPOld)
				{
	if((DPOld-DP)>=10)
					{

	DPOld=DP;
	R0=DP>>6;
	TabMinAlfa[R0]=Alfa;
					}
				}				
			}
	else if(DP<(DPStart-350))
	BitTest=1;
		}
	break;

	case 1:

	 if(Alfa<175)
			{
	_WDR();


	if(!CtChangeAlfa)
				{
	CtChangeAlfa=10;
	++Alfa;
				}



	if(DP>DPOld)
				{
	if((DP-DPOld)>=10)
					{
	RegimError &=0xfbff;
	DPOld=DP;
	R0=DP>>6;
	TabMaxAlfa[R0]=Alfa;
					}
				}				

			}
	else
			{
	BitTest=4;
//	CtDP=0;
			}
	break;
	




	case 4:
	PORTD &=0xf7;
	Alfa=255;


//	UABStart=UABOld;
	if((!RegimStop) &&(!CtTime[1]))
	RegimWork=1;
	break;

	default:break;
		}

	RegimTime |=2;

	_SEI();


	break;
	
	
	case 1:


	if(R1 & 0xefcf)
	break;
	SetErrorCT();

	if((!(RegS & 4)) && (NDiz>150)&&(!(RegS & 0x400)))
				{

			RegimWork=2;
			DPNom=DPNom0;
			RegimTime &=0xfd;
			NNom=1000;
				}
			break;		

	case 2:
//Stab DP

//			SetErrorDP();


			if(R1)
			{

			RegimWork=4;

			}
		
			if(RegimStop)
			RegimWork=4;




			 if((NDiz>550)&&(NDizOld>550))

				{
			NNomConst=1500;
			RegimWork=3;
			DPNom=DPNom0+DeltaDP;
			TestO=0;

				}
/*			else if(NDiz>650)
				{


			DPNom=DPNom0+DeltaDP;
				}*/
			else
				{
			DPNom=DPNom0;
			NNom=1400;
				}

			if(!TestO)
				{
			NDizOld=NDizNew;
			NDizNew=NDiz;

			TestO=10;
				}



			break;
	case 3:
//Stab NDiz
			RegimError &=0xfff7;
			if(NDiz<400)
			RegimError |=0x4;//Error Reika
	
			if((PM<10)&&(NDiz>600))
			RegimError |=0x40;
			else
			RegimError &=0xffbf;





			if(R1 & 0xece)
			{
			RegimWork=4;
			}
										
			if(RegimStop)
			RegimWork=4;
			else
			{
			if(!TestO)
				{
					
			if(NNom<NNomStatizm)
			NNom +=1;
			else if(NNom>NNomStatizm)
			--NNom;
			NDizOld=NDizNew;
			NDizNew=NDiz;

			StabNDiz();


			TestO=15;
				}
			if(!TestODin)
				{

					

			NDizOldDin=NDizNewDin;
			NDizNewDin=NDiz;


			if(TOG>70)
			StabNDizDin();
			TestODin=6;
				}
			}
			RegimTime |=2;
			break;




	case 4:
//Ostanov
			Alfa=0xff;
			DPNom=DPNomMax;
			SetErrorDP();

	_SEI();



			RegimTime &=0xfd;
			CtTime[1]=CtTime0;

			if(NDiz<200)
				{

			RegimWork=0;
			RegimError &=0xfffe;
				}
			else
			RegimError |=1;

			break;


	default: break;
		}
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
	void	ClearError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
	CtError[R0]=CtError0[R0];
	Error=0;
	RegimError=0;

	}
	void	LoadTabAlfa(void)
	{												     						    
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
	TabMaxAlfa[R0]=170;
	TabMinAlfa[R0]=125;
		}
	}	
	void ControlStop(void)
	{
	if(RegS & 0x20)//MU
		{		
	
	if((RegS & 0x800)||(!(RegS & 8)))//stop
	RegimStop=0xff;
	else
	RegimStop=0;
		}
	else
		{
	if(RomReceiveRS[1] & 1)
	RegimStop=0;
	else
	RegimStop=0xff;
		}
	}

	void SetFromShu(void)
	{
	unsigned int R2;		
			R2=RomReceiveRS[3];
			R2 <<=8;
			R2 |=RomReceiveRS[2];
			if(R2<1425)
			R2=1425;
			if(R2>1575)
			R2=1575;
			NNomConst=R2;


			if(RomReceiveRS[1] & 8)
			RegimStatizm=1;
			else
			RegimStatizm=0;	
	}
	void	SetRegTransmit(void)
	{

	
	RegTransmitRS[2]=NDiz;
	RegTransmitRS[3]=NDiz>>8;
	RegTransmitRS[4]=Error;//RegS;//NDiz>>8;

	RegTransmitRS[5]=Error>>8;
	RegTransmitRS[6]=TOG;
	}																		     						    
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {

	DDRA=0;
	PORTA=0xf4;
    DDRB=0xee;
	PORTB |= 0xf3;// Control UAB

    DDRC=0xff;
    PORTC =0xff;


    DDRD=0x1a;
    PORTD=0xef;/*Reset=on*/

//  INIT SPI
    SPSR=0;//f/64
	SPCR=0x72;
	SPCR |=0x80;//enable Int SPI
//INIT USART
	InitUsart();

    MCUCR=MCUCR | 3;// Log1>0 Int0
    GICR |=0x40;//Enable INT0
    TCCR1A=0;//0x82;
    TCCR1B=0xc1;
	Alfa=0xff;
	DPNom=DPNomMax;

    TIMSK=TIMSK | 0x24;//enable Int capture1 

//    TIMSK=TIMSK | 0x4;//enable Int overlowT1 

		
    /*Interrupt1*/
//    MCUCR=0x8;// Log1>0 Int1

 
//    GICR |=(1<<INT1);/*Enable INT1*/

 
   	/*Timer0*/
 	TCCR0=0x69;//0x61;//1;//1;
	OCR0=0xff;
//    TIMSK |=1;/*Enable Int Overlow Timer0*/

   	/*Timer2*/
 	TCCR2=0x3;
    TIMSK |=(1<<TOIE2);/*Enable Int Overlow Timer2*/
    InitAd();
	CtSetka=0;
	URef=ReadEepromWord(8);
	UABError=ReadEeprom(0xa);

	PORTD |=0x10;//Reset=off



    _SEI();


//    ReadKn();	
				


 	RegimWork=0;
	NNom=1400;//1420;
	NDiz=1;
	ClearError();

    CtTDiz=15;



	NNomConst=1500;

	RegimStatizm=0;



	PORTD &=0xf7;

	CtTest=100;
	BitTest=0;
	PM=0;
	TOG=75;

	DPNom=DPNomMax;
	DPNomMinStab=DPNomMinStab0;
	LoadTabAlfa();
	Alfa=175;
	CtStart=20;
	CtErrorRS=NumberBlok+15;

	_CLI();
	OCR0=Alfa;
	_SEI();
	RegimStop=0;
	RegimTest=0;
//	CtDP=0;
  /*Work program*/     	 
    while(1)
    {
    _WDR();


	if(CtStart)
		{
	DPStart=DP;
	DPNomMax=DP;
	if(RegS & 0x2000)
	NumberBlok=5;
	else
	NumberBlok=2;

		}
	ControlStop();
	SetRegTransmit();
	if(RegS & 0x1)
	RegimError |=0x20;//ATM
	if(!(RegS & 0x40))
	RegimError |=0x80;//BK SU
	else 
	RegimError &=0xff7f;
	if(RegS & 0x10)
	RegimError |=0x10;//DZVF
	else
	RegimError &=0xffef;
	if(!(RegS & 0x1000))
	RegimError |=0x2000;//UOG
	else 
	RegimError &=0xdfff;

	if((RegS & 0x80)&&(RegS & 0x200))
	NDiz=1760;
			if(NDiz>NDizRaznos)
			RegimError |=2;
			else
			RegimError &=0xfffd;
			if(NDiz>(NDizRaznos-50))
			RegimError |=0x800;
			else
			RegimError &=0xf7ff;
	if(((RegS & 0x80) && (RegS & 0x100))||(RomReceiveRS[5] & 4))
	ClearError();
	SetError();


	CalcPM();
	CalcTOG();

	if(NDizEnd)
		{
	AccountNDiz();
	NDizEnd=0;
		}




	if(UABOld<23)
	RegimError |=0x1000;
	else
	RegimError &=0xefff;



	if(TOG>155)//105gr
	RegimError |=0x8000;
	else
	RegimError &=0x7fff;
	if(RegimStatizm)
	Segment[4] |=8;
	else
	Segment[4] &=0xf7;

	IndicatorWork();
//	if(RegS & 8)
//	SetRegimWork();

	ChangeRegimTest();
	if(RegS & 0x20)//MU
		{
	RegimError &=0xbfff;
	Error &=0xbfff;
		


	ChangeRegimStatizm();
		}
	else//SHU
		{
	SetFromShu();
	RegimError |=0x4000;
		}
    ChangeCtNumberInd();



	if(RegimStatizm)
	AccountNNomStatizm();
	else
	NNomStatizm=NNomConst;
	}

}





SIGNAL(SIG_ADC)
{
    unsigned int RegInt0;
    unsigned char R1;
    RegInt0=ADC;

	if(NumberAd ==1)
		{

	DP=RegInt0;


		}



	if(NumberAd !=1)
	{
	if(CtAd[NumberAd])
		{
	--CtAd[NumberAd];
	if(CtAd[NumberAd]<=CtAd0)
			{  	
	if(RegInt0<AdTempMin[NumberAd])
	AdTempMin[NumberAd]=RegInt0;
	if(RegInt0>AdTempMax[NumberAd])
	AdTempMax[NumberAd]=RegInt0;
			}
		}
	else
		{
	 AdResult[NumberAd]=(AdTempMax[NumberAd]+AdTempMin[NumberAd])>>1;
	 AdTempMin[NumberAd]=0xffff;
	 AdTempMax[NumberAd]=0;
	 CtAd[NumberAd]=CtAd0+2;


		}
	NumberAd=1;
	}

	else if(NumberAd==1)
	{

	if(!NumberAdSlow)
	NumberAdSlow=3;
	else
	NumberAdSlow=0;
	NumberAd=NumberAdSlow;

	}

	R1=ADMUX;

	 R1 = R1 & 0xe0;
	 R1 |= NumberAd;
	 ADMUX=R1;
	 ADCSRA |=(1<<ADSC);
 	
}



 SIGNAL(SIG_OVERFLOW2)/*128mks*/
{

    TIMSK=TIMSK | 0x20;//enable Int capture1
//	TIFR |=0x20;//clear flag ICR
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
	unsigned char R0;
	unsigned char R1;
	if(!CtByteAnod)
	{
	++CtByteAnod;	
	R1=CtSetka;
//	++R1;
	R0=(1<<R1);
	R0=~R0;
	SPDR=R0;
/*	switch(CtSetka)
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


		}*/


	}


	else
	{
//	if(EnableInd)
	PORTB &=0xfd;
	SPCR &=0x7f;//denable interrupt SPI

//	TCNT2=0xfe;
	TIFR |=0x40;//reset flag
	TIMSK |=0x40;
	TCCR2=0x4;
	MaskaSetka=0;
	}

}

 SIGNAL(SIG_INPUT_CAPTURE1)
{

	unsigned int R0;
//	unsigned int R1;
    TIMSK=TIMSK & 0xdf;//denable Int capture1
	if(((ICR1>=ICR1Old) && (CtOverLow==1))||(!CtOverLow))
    TDizTemp[CtTDiz]=0xffff;

    else
	    {
	R0=ICR1;
    TDizTemp[CtTDiz]=R0-ICR1Old;

		}

 
   ICR1Old=ICR1;
    if(CtTDiz)
    --CtTDiz;
    else
		{
    CtTDiz=15;
	NDizEnd=1;

	    }
    CtOverLow=2;

	TIFR |=4;//clear flag overlow
    TIMSK=TIMSK | 0x20;//enable Int capture1
} 
 SIGNAL(SIG_OVERFLOW1)/*8.0 Mc*/
    {
	unsigned char R0;
	if(CtChange)
	--CtChange;
	else
		{
	CtChange=250;
	if(RegimTest==8)
			{
	if(PORTC & 2)
	PORTC &=0xe5;
	else
	PORTC |=0x1e;
			}
	else
			{
	if(PORTC & 2)
	PORTC &=0xe9;
	else
	PORTC |=0x1e;
			}	
		}


	if(CtEnableKn)
	--CtEnableKn;
	else
		{
	CtEnableKn=7;

		}
	switch(CtEnableKn)
		{

	case 0:ReadKnPC0();
	if(CtMinusPM)
	--CtMinusPM;
	if(CtMinusTOG)
	--CtMinusTOG;
	if(CtPlusPM)
	--CtPlusPM;
	if(CtPlusTOG)
	--CtPlusTOG;


	break;

	case 1:	PORTC &=0x1e;
	PORTC |=0x1;
	break;

	case 2:ReadKnPC5();
	break;
	case 3:	PORTC &=0x1e;
	PORTC |=0x20;
	break;
	case 4:ReadKnPC6();
	break;
	case 5:	PORTC &=0x1e;
	PORTC |=0x40;
	break;
	case 6:ReadKnPC7();
	break;
	case 7:	PORTC &=0x1e;
	PORTC |=0x80;
	CtWellKn=4;
	break;
	default :CtEnableKn=7;
	break;
		}



	UABOld=(UABNew>>1);
	UABNew=0;
	if(CtChangeAlfa)
	--CtChangeAlfa;
	if(CtTest)
	--CtTest;




	if(CtDif)
	--CtDif;
	if(TestO)
	--TestO;
	if(TestODin)
	--TestODin;
	if(CtInd)
	--CtInd;
//	if(CtEnableKn)
//	--CtEnableKn;
	if(DelError)
	--DelError;
	else
		{
	if((CtStart)&&(!RegimStop))
	--CtStart;

	DelError=7;
	if(CtUsart)
	--CtUsart;
	else
			{

	TransmitUsart();
	CtUsart=(NumberBlok<<1)+15;
			}

	if(CtErrorRS)
	--CtErrorRS;
	else
			{
	InitUsart();
	CtErrorRS=NumberBlok+39;

			}


    for(R0=0;R0<=15;++R0)
		    {
    if(RegimError & (1<<R0))
 		       {
    if(CtError[R0])
    --CtError[R0];
 		       }
    else
    CtError[R0]=CtError0[R0];

 		   }
    for(R0=0;R0<=1;++R0)
		    {
    if(RegimTime & (1<<R0))
		        {
    if(CtTime[R0])
    --CtTime[R0];
 		       }
    else
    CtTime[R0]=CtTime0;

 		   }
		}

    if(CtOverLow)
    --CtOverLow;
    else
	NDiz=0;

    }
	SIGNAL(SIG_UART_RECV)
{
	LinkRS();

}
SIGNAL(SIG_INTERRUPT0)
{

	++UABNew;
	GICR &=0xbf;
	GIFR |=0x40;
	GICR |=0x40;
}
