/* TWI-Master*/
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
    unsigned char RomReceiveOld;
    unsigned char RegimTest;
    unsigned char CtErrorTransmit;
    unsigned    char            TestRS;
    unsigned    int            TestAd[5];
    unsigned    char            TestTwi[3];
    unsigned    char            ErrorLuk;
    unsigned    char            RegimErrorLuk;
    unsigned    int            CtErrorLuk[2];
    const unsigned    int      CtErrorLuk0=1200;

    unsigned    char            CtTimeClose;
    unsigned    char            CtTimeOpen;
    const unsigned    char      CtTimeClose0=30;
    const unsigned    char      CtTimeOpen0=30;
    unsigned int Error;
	volatile    unsigned int CtError[13];
    const unsigned int CtError0=250;


    unsigned char CtErrorRS;

    const unsigned char NumberBlok=0;
    unsigned char NumberReceiveRS;
    unsigned char ErrorLink;
    unsigned char CtUsart;


    unsigned char RegTransmitRS[9];
    unsigned char RamReceiveRS[9];
    unsigned char RomReceiveRS[9][8];

    unsigned char	ControlTransmitRS;
    unsigned char ControlReceiveRS;


	volatile unsigned char CtReceiveRS;

    const unsigned char CtReceiveRS0=8;//4;


    unsigned int URef;
	unsigned    int         	TM;
	unsigned    char         	TMRez;
    unsigned    char    CtPlusTM;
    unsigned    char    CtMinusTM;
    unsigned    int            AdTempMin;
    unsigned    int            AdTempMax;

	unsigned    int	AdResult;
     unsigned int CtStart;

    unsigned int RegInt0;
 	unsigned    char            Reg101;
 	unsigned    int             Reg100;



 
    unsigned    int RegimError;



	unsigned    int    Reg0;



 	unsigned    char            RomReceive[9][3];
 	unsigned    char            RegTransmit[12][3];
    unsigned    char            NumberLink;
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=10000;//500;
    unsigned    char             CtErrorLink[6];
	const   unsigned    char     CtErrorLink0=20; 
	const   unsigned    char     CtErrorLink1=100;	   
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;

    unsigned    int	CtAd;

	static const unsigned    int	    CtAd0=1024;//3msec
 
//    unsigned    char	MyStatus;

	unsigned    char	RegS,RegSTemp,RegSOld;
       
    /*============================================*/
 

 
 
void    ReadKn(void)
    {
    unsigned    char R0;
    R0=0;

    if(bit_is_clear(PINA,PA3))
    R0=R0 | 1;

    if(bit_is_clear(PINA,PA4))
    R0=R0 | 2;

    if(bit_is_clear(PINA,PA5))
    R0=R0 | 4;

    if(R0==RegSTemp)
		{

    RegS=R0;
		}
    RegSTemp=R0;
    }





void InitAd   (void)
    {
    ADMUX=0xc0;//2.56=Ref
    SFIOR=SFIOR & 0x1f;//start from end AD
 	CtAd=CtAd0;


	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
    ADCSRA |=(1<<ADPS1);

	ADCSRA |=(1<<ADPS0); /*32*/	 		
	ADCSRA |=(1<<ADIE);/* enable interrupt*/
    ADCSRA |=(1<<ADSC);/* Start*/
    }






unsigned char WaitTWCR(void)
    {
   while(!(TWCR & 0x80))
        {

    if(CtTWCR)
		{
    --CtTWCR;
	_WDR();
		}
    else
            {
    CtTWCR=CtTWCR0;
    return 0;//error
            }
        }
    CtTWCR=CtTWCR0;
    return 1;
    }

unsigned char    Transmit(void)
    {

    if(!WaitTWCR())
    return 0;

    if(((TWSR & 0xf8) == 0x8)||((TWSR & 0xf8) == 0x10))//start
            {
    TWDR=(NumberLink<<1)+4;//0x4;//SLA+W
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);


            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
    if(!WaitTWCR())
    return 0;


   if((TWSR & 0xf8)==0x18)//Adres+ACK
            {
    CtTransmit=10;
   TWDR=RegTransmit[CtTransmit][NumberLink];
   TWCR=(1<<TWINT)|(1<<TWEN);

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
TT:    if(!WaitTWCR())
    return 0;

   if((TWSR & 0xf8)==0x28)//Data+ACK
            {
    if(CtTransmit)
                {
    --CtTransmit;
   TWDR=RegTransmit[CtTransmit][NumberLink];
   TWCR=(1<<TWINT)|(1<<TWEN);
   goto TT;
                }
    else
                {

    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 1;

               }

            }
    else
            {


    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }
    }

unsigned char    Receive(void)
    {
    if(!WaitTWCR())
    return 0;

    if((TWSR & 0xf8) == 0x8)//Well Start
            {
    TWDR=(NumberLink<<1)+5;//0x5;//SLA+R
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
//	++TestLink;

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;

            }

    if(!WaitTWCR())
    return 0;

   if((TWSR & 0xf8)==0x40)//Adres+ACK
            {
    CtReceive=11;

   RamReceive[CtReceive]=TWDR;
   TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);

            }
    else
            {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
    return 0;

            }
RR:  if(!WaitTWCR())
    return 0;
 
   if((TWSR & 0xf8)==0x50)//Data+ACK
            {
    if(CtReceive)
                {
   
    --CtReceive;
   RamReceive[CtReceive]=TWDR;


   TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
   goto RR;
                }
    else
                {
    TWCR=(1<<TWINT)|(1<<TWEN);
 
    goto RR;

                }

            }
    else
            {
 
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 1;

            }

    }



    void    ReceiveTransmitMaster(void)
{
//ControlSum for Transmit
    RegTransmit[0][0]=0;
    RegTransmit[0][1]=0;
    RegTransmit[0][2]=0;
    for(Reg0=1;Reg0<=10;++Reg0)
    {

    RegTransmit[0][0] +=RegTransmit[Reg0][0];

    RegTransmit[0][1] +=RegTransmit[Reg0][1];
    RegTransmit[0][2] +=RegTransmit[Reg0][2];

    }

//Transmit
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
    CtTWCR=CtTWCR0;
    if(!Transmit())
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    Reg0=5000;

    }
    else
    {

    Reg0=5000;//
    }
    while(Reg0)
	{_WDR();
    --Reg0;
	}
//for Ind ControlSum
    Reg100=0;
    for(Reg0=1;Reg0<=10;++Reg0)
    Reg100=Reg100+RamReceive[Reg0];
    Reg100=RamReceive[0];

//Receive
   TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);//Start
   CtTWCR=CtTWCR0;
    if(Receive())
    {

//ControlSum
    Reg101=0;
    for(Reg0=1;Reg0<=10;++Reg0)
    Reg101=Reg101+RamReceive[Reg0];
    if(Reg101==RamReceive[0])
        {

    switch(NumberLink)
            {
    case 0:    for(Reg0=1;Reg0<=8;++Reg0)
				{
    RomReceive[Reg0][0]=RamReceive[Reg0];
	RamReceive[Reg0]=1;
				}

	RamReceive[0]=0xff;
    CtErrorLink[0]=CtErrorLink0;
    CtErrorLink[3]=CtErrorLink1;
	++TestTwi[0];
    break;

    case 1:    for(Reg0=1;Reg0<=10;++Reg0)
				{
    RomReceive[Reg0][1]=RamReceive[Reg0];
	RamReceive[Reg0]=1;
				}
    CtErrorLink[1]=CtErrorLink0;
    CtErrorLink[4]=CtErrorLink1;
	RamReceive[0]=0xff;
	++TestTwi[1];
    break;

    case 2:    for(Reg0=1;Reg0<=10;++Reg0)
				{
    RomReceive[Reg0][2]=RamReceive[Reg0];
	RamReceive[Reg0]=1;
				}
    CtErrorLink[2]=CtErrorLink0;
    CtErrorLink[5]=CtErrorLink1;
	RamReceive[0]=0xff;
	++TestTwi[2];
    break;
    default:break;
			}
        }
    }
    else
    {
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);


    }

} 
void    LoadRegTransmit(void)
    {
	unsigned char R0;
	unsigned char R1;
	R1=0;

	R0=0;
  	if(RomReceiveRS[1][3] & 2)//KG1=on
    R0 |=1;
  	if(RomReceiveRS[1][6] & 2)//KG2=on
    R0 |=2;
  	if((RomReceiveRS[1][3] & 4)||(RomReceiveRS[1][6] & 4))//KSet=on
    R0 |=0x20;
  	if((RomReceiveRS[6][3] & 4)||(RomReceiveRS[6][6] & 4))//Set Norma
    R0 |=0x10;
 	if(RomReceiveRS[1][1] & 4)//WorkPCH1
    R0 |=4;
 	if(RomReceiveRS[1][4] & 4)//WorkPCH2
    R0 |=8;
 	if(RomReceiveRS[6][6] & 8)//Rezerv=on
    R0 |=0x40;
 	if(RomReceiveRS[6][3] & 8)//Rezerv=on
    R0 |=0x40;
    RegTransmit[1][1] =RegimTest;//R0;
    RegTransmit[1][2] =RegimTest;//R0;
 

  


	if(RomReceiveRS[1][1] & 2)//ErrorPCH1
    RegTransmit[3][1] |=4;
	else
    RegTransmit[3][1] &=0xfb;

	if(RomReceiveRS[1][4] & 2)//ErrorPCH2
    RegTransmit[3][1] |=8;
	else
    RegTransmit[3][1] &=0xf7;
	if(RomReceive[3][2] & 0x80)
    RegTransmit[3][1] |=0x80;//Luki=close
	else
    RegTransmit[3][1] &=0x7f;


	if((RomReceiveRS[3][2]>=1)||(RomReceiveRS[3][5]>=1))
	RegTransmit[5][1]=1;//Ognet=off
	else
	RegTransmit[5][1]=0;//Ognet=on
    RegTransmit[6][1]=6;    
    RegTransmit[7][1]=7;//Avaria;
    RegTransmit[8][1]=8;//CtInd;
    RegTransmit[9][1]=9;//Koef[8];//NDiz
    RegTransmit[10][1]=10;//Koef[7];




 
    RegTransmit[4][2]=4;//Koef[8];//NDiz
    RegTransmit[5][2]=5;//Koef[7];
    RegTransmit[6][2]=6;    
    RegTransmit[7][2]=7;//Avaria;
    RegTransmit[8][2]=8;//CtInd;
    RegTransmit[9][2]=9;//Koef[8];//NDiz
    RegTransmit[10][2]=10;//Koef[7];

	if((RomReceiveRS[1][4] & 0x10)&&(!(RomReceiveRS[1][1] & 0x20)))
    R1 |=1;//Zamena=on
	else if((RomReceiveRS[1][1] & 0x10)&&(!(RomReceiveRS[1][4] & 0x20)))
    R1 |=1;//Zamena=on	
		
	if((RomReceive[4][2] & 0xf8)||(RomReceive[5][2] & 0x1f))//G1 Error
	R1|=0x20;	
	if((RomReceive[4][2] & 0x66)||(RomReceive[5][2] & 0xe0)||(RomReceive[6][2] & 0x7))//G2 Error
	R1|=0x40;
	if(RomReceive[6][1] & 8)//FVU
	R1 |=0x60;
	if(RomReceiveRS[4][3] & 0x20)//MSHU
	R1 |=0x20;
	if(RomReceiveRS[4][6] & 0x20)//MSHU
	R1 |=0x40;
	if(RomReceive[3][2] & 0x20)//Pogar
	R1 |=0x8;
	if(RomReceiveRS[6][3] & 0x20)//Izol
	R1 |=0x10;
	if(RomReceiveRS[6][6] & 0x20)//Izol
	R1 |=0x10;

	RegTransmit[4][1]=R1;
 
    }    




 
 

		  



	void	ResetError(void)
	{
	unsigned char R0;

	Error=0;
	ErrorLuk=0;

	for(R0=0;R0<=8;++R0)
	CtError[R0]=CtError0;
	CtError[12]=CtError0;
	for(R0=0;R0<=1;++R0)
	CtErrorLuk[R0]=CtErrorLuk0;
	for(R0=0;R0<=5;++R0)
	CtErrorLink[R0]=CtErrorLink1;
	RegimError=0;
	RegTransmit[3][1] |=0x20;
	RegTransmit[3][2] |=0x1;
	}			
	void	SetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=8;++R0)
		{
	if(!CtError[R0])
	Error |=(1<<R0);
		}
	if(!(CtError[12]))
	Error |=0x1000;

	if(!CtErrorLink[3])
	Error |=0x200;
	if(!CtErrorLink[4])
	Error |=0x400;
	if(!CtErrorLink[5])
	Error |=0x800;

	if(RomReceive[3][2] & 0x20)//Pogar
	RegTransmit[3][1] |=0x40;
	else if(RegTransmit[2][1] & 0x3)//Avaria
	RegTransmit[3][1] |=0x40;
	else if(RomReceive[4][2] & 0xfe)
	RegTransmit[3][1] |=0x40;
	else if(RomReceive[5][2])
	RegTransmit[3][1] |=0x40;
	else if(RomReceive[6][2] & 7)
	RegTransmit[3][1] |=0x40;

	else if(RomReceiveRS[4][2])
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[5][2])
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[4][5])
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[5][5])
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[5][3] & 0x80)
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[5][6] & 0x80)
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[6][3] & 0x40)
	RegTransmit[3][1] |=0x40;
	else if(RomReceiveRS[6][6] & 0x40)
	RegTransmit[3][1] |=0x40;
	else if(RomReceive[6][1] & 0x8)//FVU
	RegTransmit[3][1] |=0x40;

	else if(Error  & 0x180)
	RegTransmit[3][1] |=0x40;


	else
	RegTransmit[3][1] &=0xbf;

	}
    
	void CalcTM(void)
	{
	unsigned long R0;

	R0=AdResult;
	R0 *=URef;
	R0 >>=8;
	TM=R0;
	if(RomReceive[7][1] & 0x10)
	TM +=30;							
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


		
	void	ControlLuk(void)
	{
/*
0	Luk1
1	Luk2
2	PCH1
3	PCH2
4	LR1
5	LG1
6	LR2
7	LG2
8	Luk4
*/
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	R1=RegTransmit[1][2];
	R2=RegTransmit[2][2];
	R0=0;
	if(RomReceiveRS[3][2]>=2)
	R0 |=1;
	if(RomReceiveRS[3][5]>=2)
	R0 |=2;

	if(RomReceive[2][1]& 2)//FVU=on
		{
	R1=0;
	R2 &=0xfe;

		}
	else
		{


	if(RomReceiveRS[5][6] & 2)//T>=75 & n>400
	R1 |=0x40;//LR2=on
	else if(RomReceiveRS[5][6] & 0x20)//T<=60 ||n<400
	R1 &=0xbf;//LR2=off
	if(RomReceiveRS[5][3] & 2)//T>=75 && n>400
	R1 |=0x10;//LR1=on
	else if(RomReceiveRS[5][3] & 0x20)//T<=60 || n<400
	R1 &=0xef;//LR1=off	
			
	if(RomReceiveRS[5][6] & 1)//SHU2 N>400
	R1 |=0x80;//LG2=on
	else
	R1 &=0x7f;//LG2=off

	if(RomReceiveRS[5][3] & 1)//SHU1 N>400
	R1 |=0x20;//LG1=on
	else
	R1 &=0xdf;
	if(RomReceiveRS[1][1] & 8)//LPH1
			{
	R1 |=0x4;//LPCH1=on
	R0 |=8;
			}
	else
	R1 &=0xfb;//LPCH1=close
	if(RomReceiveRS[1][4] & 8)//LPH2
			{
	R1 |=0x8;//LPCH2=on
	R0 |=0x10;
			}
	else
	R1 &=0xf7;//LPCH2=close

	if(R0)
			{
	R1 |=3;
	R2 |=1;
			}
	else
			{
	R1 &=0xfc;
	R2 &=0xfe;
			}



		
		}
	RegTransmit[1][2]=R1;
	RegTransmit[2][2]=R2;			
		
								
	}	




	

	void LoadIndicator(void)
	{	
 	unsigned char R0;
 	unsigned char R1;
 	unsigned char R2;
 	unsigned char R3;
 	unsigned char R4;
	R1=RomReceive[4][2];

    R2=RomReceive[5][2];
    R3=RomReceive[6][2];
	R0=0;
	R4=0;
	if(Error & 0x48)
		{
	R0=6;
	R4=1;
		}
    RegTransmit[1][0]=0;    
	RegTransmit[2][0]=RegimTest;//R0;
	R0=0;
	if(R1 & 6)
		{
	R0=1;//luk1
	R4=1;
		}
	RegTransmit[3][0]=RomReceive[4][1] & 0xfc;//egS;//RomReceive[7][1];//R0;
	R0=0;
	if(R1 & 0x18)
		{
	R0=2;//luk2
	R4=1;
		}
	RegTransmit[4][0]=RegS;//R0;
	R0=0;
	if(R1 & 0x60)
		{
	R0=19;//lPch1
	R4=1;
		}
	RegTransmit[5][0]=RomReceive[2][2];//0;
	R0=0;
	if(R1 & 0x80)
		{
	R0=20;//lPch2
	R4=1;
		}
	if(R2 & 0x1)
		{
	R0=20;//lPch2
	R4=1;
		}

	RegTransmit[6][0]=((Error>>8) & 0xe) ;//URef;//0;

	R0=0;
	if(R2 & 0x18)
		{
	R0=12;//lg1
	R4=1;
		}
	else if(R2 & 0x6)
		{
	R0=14;//lr1
	R4=1;
		}
	RegTransmit[7][0]=URef>>8;//R0;	
	R0=0;
	if(R3 & 0x1)
		{
	R0=13;//lg2
	R4=1;
		}
	else if(R2 & 0x80)
		{
	R0=13;//lg2
	R4=1;
		}
	else if(R2 & 0x60)
		{
	R0=15;//lr2
	R4=1;
		}
	else if(ErrorLuk & 3)
		{
	R0=5;//lN3(Vent)
	R4=1;
		}
	if(RegimTest<=1)
	RegTransmit[8][0]=RomReceive[4][1] & 3;//R0;
	else
	RegTransmit[8][0]=URef;		
	R0=0;
	if(R3 & 0x6)
		{
	R0=10;//luk4
	R4=1;
		}
	if(RegimTest<=1)
	RegTransmit[9][0]=RomReceive[7][1];//0;
	else
	RegTransmit[9][0]=TM>>2;
	R0=0;
	if(RomReceive[6][1] & 8)
	R0=18;
	else if(RomReceiveRS[4][3] & 0x20)//MSHU
	R0=9;
	else if(RomReceiveRS[4][6] & 0x20)//MSHU
	R0=9;
	else if((!R4)&&(RomReceiveRS[1][3] & 0x20))
	R0=4;
	else if((!R4)&&(RomReceiveRS[1][6] & 0x20))
	R0=4;

	else if((!R4)&&(RomReceive[3][1] & 0x20))
	R0=16;
	else if((!R4)&&(!(RomReceive[3][1] & 0x20)))
	R0=3;		
	RegTransmit[10][0]=PIND & 1;//Error>>8;//R0;			
	}	

/*	void LoadIndicator(void)
	{	
 
    RegTransmit[1][0]=0;//CtErrorRS;//39;    
    RegTransmit[2][0]=RomReceiveRS[5][6];//CtErrorLink[3];//4;
    RegTransmit[3][0]=RomReceiveRS[6][6];//CtErrorLink[4];//5;
	RegTransmit[4][0]=RomReceiveRS[5][3];//AdResult>>2;//TM;//4;
    RegTransmit[5][0]=RomReceiveRS[6][3];//TestAd[0];//TestTwi[0];
    RegTransmit[6][0]=RegTransmit[3][1];//omReceiveRS[4][3];//TestAd[0]>>8;//Twi[1];
    RegTransmit[7][0]=RomReceiveRS[4][6];//URef;//AdResult;//TestAd[2];//Twi[2];
     RegTransmit[8][0]=Error;//TestRS;//URef>>8;//AdResult>>8;//RomReceive[2][1];
    RegTransmit[9][0]=RegimError;//RegS;//RomReceive[4][1];//Error>>8;//CtErrorRS;
    RegTransmit[10][0]=TM;//RomReceive[3][2];	
	}*/	



	void SetErrorLuk(void)
	{
	unsigned char R0;
	for(R0=0;R0<=1;++R0)
		{
	if(!CtErrorLuk[R0])
	ErrorLuk |=(1<<R0);			
		}
	}
	void SetRegimErrorLuk(void)
	{
	if(!(PORTB & 8))
	RegimErrorLuk |=1;
	else
	RegimErrorLuk &=0xfe;
	if(!(PORTB & 0x10))
	RegimErrorLuk |=2;
	else
	RegimErrorLuk &=0xfd;
	}
    void ChangeRegimTest(void)
{
	unsigned char R0;
	unsigned char R1;
	unsigned char R2;
	R0=RegimTest;
	R1=RomReceive[7][1];
	R2=RomReceiveOld;

    if((R1 & 0x1)&&(!(R2 & 0x1))) 
		{
    ++R0;

		}
    else if((R1 & 0x2)&&(!(R2 & 0x2)))
		{
    --R0;

		}
	if(R0>9)
	R0=0;
	RomReceiveOld=R1;
	RegimTest=R0;
}	
    void ControlOut(void)
{	
	if(!RegimTest)
		{
	PORTB |=0x1c;
	PORTD |=0xa;

		}
	else if(RegimTest==1)
		{	
	PORTB |=0x1c;		
	PORTB &=0xfb;
	PORTD |=0x8;
	PORTD &=0xfd;
		
		}
	else if(RegimTest==2)
		{	
	PORTB |=0x1c;		
	PORTB &=0xf7;
	PORTD &=0xf7;		
		}		
	else if(RegimTest==3)
		{	
	PORTB |=0x1c;		
	PORTB &=0xef;
	PORTD &=0xf7;		
		}
}		
				
																   
/*++++++++++++++++++++++++++++++++++++++++++*/


    int main(void)
    {
	DDRA=0;
	PORTA=0;
 
    DDRB=0xbc;
	PORTB |= 0xff;

    DDRC=0x1c;
 //   PORTC |=0xff;

    DDRD=0xba;
    PORTD=0xff;

//INIT TWI;
    TWBR=0x7f;//F TWI
    CtStart=65000;
	while(CtStart--)	_WDR();
//Init capture1
    TCCR1B=0x82;//0.5mkc,falling edge
    TIMSK=TIMSK | 0x4;//enable Int overlowT1





//  INIT SPI
    SPCR=0x72;//f/64
    SPSR=0;//f/64



    InitAd();


    MCUCR=0;


	TCCR0=5;// /1024

	ASSR=0;

	TCCR2=2;// T=0,5 mkc

	ASSR=0;





	NumberLink=1;

	CtUsart=NumberBlok;
	ResetError();
	URef=ReadEepromWord(8);
	if(URef>275)
	URef=256;
	if(URef<230)
	URef=256;

    _SEI();

					             
     while(1)
    {

     _WDR();
	 ChangeRegimTest();
	 ControlOut();
	CalcTM();
	LoadIndicator();
	SetRegimErrorLuk();
	SetErrorLuk();


	if(RomReceiveRS[5][3] & 4)
	ResetError();
	else if(RomReceiveRS[5][6] & 4)
	ResetError();
	else
		{
	RegTransmit[3][1] &=0xdf;
	RegTransmit[3][2] &=0xfe;
		}
	SetError();

	if(RomReceiveRS[6][3] & 0x80)
		{
	RegTransmit[1][2]=0;
	RegTransmit[2][2] &=0xfe;
		}
	else
	ControlLuk();







	 if(!CtErrorLink[0])
		{
//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;

    TWBR=0x7f;//F TWI

    CtStart=65000;
	while(CtStart--)	_WDR();
	CtErrorLink[0]=CtErrorLink0;
	CtErrorLink[1]=CtErrorLink0;
	CtErrorLink[2]=CtErrorLink0;
		}
	 if(!CtErrorLink[1])
		{
//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;

    TWBR=0x7f;//F TWI
    CtStart=65000;
	while(CtStart--)	_WDR();

	CtErrorLink[0]=CtErrorLink0;
	CtErrorLink[1]=CtErrorLink0;
	CtErrorLink[2]=CtErrorLink0;
		}
	 if(!CtErrorLink[2])
		{
//INIT TWI;
    TWBR=0;//F TWI
   TWAR=0;
   TWCR =0;
	TWSR=0xf8;

    TWBR=0x7f;//F TWI
    CtStart=65000;
	while(CtStart--)	_WDR();
	CtErrorLink[0]=CtErrorLink0;
	CtErrorLink[1]=CtErrorLink0;
	CtErrorLink[2]=CtErrorLink0;
		}
    ReadKn();

   if(NumberLink)
    --NumberLink;
    else
    NumberLink=2;






    LoadRegTransmit();

   ReceiveTransmitMaster();






    }
  
}
SIGNAL(SIG_ADC)
{
    unsigned int RegInt0;
	unsigned char R1;

    RegInt0=ADC;


	if(CtAd)
	{
	--CtAd;
	if(CtAd<=CtAd0)
		{
	if(RegInt0<AdTempMin)
	AdTempMin=RegInt0;
	if(RegInt0>AdTempMax)
	AdTempMax=RegInt0;		

		}
	}
	else
	 {
	 AdResult=(AdTempMax+AdTempMin)>>1;
	 AdTempMin=0xffff;
	 AdTempMax=0;
	 CtAd=CtAd0+2;


	 }

	R1=ADMUX;
	R1 &=0xe0;
	 ADMUX = R1;

	 ADCSRA |=(1<<ADSC);
  	
}



 SIGNAL(SIG_OVERFLOW1)/*32 Mc*/
{

	unsigned char R0;
	for(R0=0;R0<=1;++R0)
		{
	if(RegimTest>1)
			{
	if(CtErrorLuk[R0])
	--CtErrorLuk[R0];
			}
	else
	CtErrorLuk[R0]=CtErrorLuk0;
		}



	if(!(PORTB & 0x10))//Luk3=(off=on)
	CtTimeClose=CtTimeClose0;
	else if(CtTimeClose)
	--CtTimeClose;
	if(!(PORTB & 0x8))//Luk3=(on=on)
	CtTimeOpen=CtTimeOpen0;
	else if(CtTimeOpen)
	--CtTimeOpen;




	for(R0=0;R0<=6;++R0)
		{
	if(CtError[R0])
	--CtError[R0];
		}
	for(R0=7;R0<=8;++R0)
		{
	if(RegimError & (1<<R0))
			{
	if(CtError[R0])
	--CtError[R0];
			}
	else
	CtError[R0]=CtError0;
		}
	if(RegimError & 0x1000)
			{
	if(CtError[12])
	--CtError[12];
			}
	else
	CtError[12]=CtError0;


	for(R0=0;R0<=5;++R0)
		{
	if(CtErrorLink[R0])
	--CtErrorLink[R0];
		}





}



  





