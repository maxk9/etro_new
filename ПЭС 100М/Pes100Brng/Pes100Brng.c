   
    

#include <compat/ina90.h>
#include <avr/iom8535.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>



unsigned int AlfaRez;
unsigned int AlfaI;
unsigned int UNomWork;
unsigned int AdTest1;
unsigned int AdTest;
unsigned int ISum;
static const unsigned int UNomMax = 2800;
static const unsigned int UNomMin = 2100;
unsigned char CtKn;
unsigned char CtErrorInt0;
unsigned char RegimError;
unsigned int CtError;
static const unsigned int CtError0 = 20000;
unsigned int Temperatura;
static const unsigned int Temperatura85 = 136;
static const unsigned int Temperatura80 = 156;
unsigned int CtStart;
unsigned char EndAd;
unsigned char CtEeprom;
unsigned int CtInitInd;

unsigned int UOs;
unsigned int UOsOld;

unsigned int RegInt0;
unsigned int Alfa;
static const unsigned int AlfaMax = 3200;
static const unsigned int AlfaMin = 800;

unsigned int Reg0;
unsigned char Reg1;

unsigned int Avaria;

unsigned char SregTemp;   

unsigned char IndData[6];


unsigned char NumberAd;
unsigned char CtAd;
static const unsigned char CtAd0 = 129;//3.0msec
unsigned char CtAdSum;
static const unsigned char CtAdSum0 = 16;

unsigned int	AdTemp[7];
unsigned int	AdTempMin[7];
unsigned int	AdTempResult[7];
unsigned int	AdResult[7];

unsigned char RegS,RegSTemp;

unsigned int UNom;

unsigned int CtUst;
static const unsigned int CtUst0 = 40;


       
/*============================================*/
 

 
 
void ReadKn(void)
{
	unsigned char Reg2;
	
	Reg2 = 0;
	
	if(bit_is_clear(PINB,PB0))
		Reg2 = Reg2 | 1;
	if(bit_is_clear(PINB,PB1))
		Reg2 = Reg2 | 2;
	if(bit_is_clear(PINB,PB2))
		Reg2 = Reg2 | 4;

	if(Reg2 == RegSTemp)
		{
		if(CtKn)
			--CtKn;
		else
			RegS = Reg2;
		}
	else
	CtKn = 5;

	RegSTemp = Reg2;
}
	
void DecSegm( unsigned int Arg0)
{
	unsigned int Arg1;

	if(Arg0>9999)
		Arg0 = 9999;
		
	Arg1 = Arg0/1000;
	IndData[3] = Arg1;
	Arg0 = Arg0-(Arg1*1000);
	Arg1 = Arg0/100;
	IndData[2] = Arg1;
	Arg0 = Arg0-(Arg1*100);
	Arg1 = Arg0/10;
	IndData[1] = Arg1;
	Arg0 = Arg0-(Arg1*10);
	IndData[0] = Arg0;

}

void WriteEeprom(unsigned char Arg4,unsigned char Arg5)
{
	_WDR();
	while(bit_is_set(EECR,EEWE));
	while(bit_is_set(SPMCR,SPMEN));

	EEARL = Arg4;
	EEARH = 0;
	EEDR = Arg5;
	SregTemp = SREG;
	_CLI();
	EECR |= (1<<EEMWE);
							 
	EECR |= (1<<EEWE);
	SREG = SregTemp;
	
	while(bit_is_set(EECR,EEWE));

	while(bit_is_set(SPMCR,SPMEN));
}



unsigned char ReadEeprom(unsigned char Arg6)
{		 	

	EEARH = 0;
	EEARL = Arg6;
	EECR = EECR | (1<<EERE);

	while(bit_is_set(EECR,EERE));

	return EEDR;
}

void InitAd(void)
{
	ADMUX = 0xc0;//AVCC=Ref
	SFIOR = SFIOR & 0x1b;//start from end AD
	CtAd = CtAd0;
	NumberAd = 0;
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);/*enable AD*/
	ADCSRA |= (1<<ADPS2);
	//ADCSRA |=(1<<ADPS1);
	ADCSRA |= (1<<ADATE);//Auto trigger enable
	ADCSRA |= (1<<ADPS0); /*32*/	 		
	ADCSRA |= (1<<ADIE);/* enable interrupt*/
	ADCSRA |= (1<<ADSC);/* Start*/
}


void IndPar(void)
{
	unsigned char R0; 
	R0 = 200;
	while(R0--) _WDR(); 
	
	SPDR = IndData[3]+0x30;

	R0 = 200;
	while(R0--) _WDR(); 
	
	SPDR = IndData[2]+0x30;
	R0 = 200;
	while(R0--) _WDR(); 
	 
	SPDR = IndData[1]+0x30;

	R0 = 200;
	while(R0--) _WDR(); 
	
	SPDR = IndData[0]+0x30;
}


/*  void	ClearCursor(void)
    {
     unsigned int R0;     
    R0=300;
    while(R0--)         _WDR();
    SPDR=0xb;
    }*/
void SetCursor(unsigned char R2,unsigned char R1)
{
	unsigned int R0;  
	R0=300;
	while(R0--) _WDR();
	
	SPDR=0x1f;
	R0=300;
	while(R0--) _WDR();
	
	SPDR=0x24;
	R0=300;
	while(R0--) _WDR();
	
	SPDR=R2;
	R0=300;
	while(R0--) _WDR();
	
	SPDR=0;
	R0=300;
	while(R0--) _WDR();
	
	SPDR=R1;
	R0=300;
	while(R0--) _WDR();
	
	SPDR=0;

}




void SetSize(unsigned char R2,unsigned char R1)
{
	 unsigned int R0; 
	 
	R0 = 400;
	 while(R0--) _WDR(); 
	SPDR = 0x1f;
	
	R0 = 400;
	 while(R0--) _WDR();	 
	SPDR = 0x28;
	
	R0 = 400;
	 while(R0--) _WDR(); 
	SPDR = 0x67;
	
	R0 = 400;
	 while(R0--) _WDR(); 
	SPDR = 0x40;

	R0 = 400;
	 while(R0--) _WDR(); 
	SPDR = R2;

	R0 = 400;
	 while(R0--) _WDR(); 
	SPDR = R1;
}


void TransmitInd(unsigned char R1)
{
    unsigned int R10;

    if(R1>=192)
    R1=R1-64;
    R10=500;
     while(R10--) _WDR();   
    SPDR=R1;           
 }
     

unsigned int AccountAlfaDT(unsigned int R0)
{
	int R1;
	R1 = UOs-UOsOld;
	if(R1>0)
		{
			R1 = R1>>4;//1B=1bit
			if(R1>160)
				R1 = 160;
			R0 = R0+R1;//U-
		}
	else 
		if(R1<0)
		{
			R1 = ~R1;
			R1 = R1>>4;//1B=1bit
			if(R1>160)
				R1 = 160;
			R0 = R0-R1;//U+
		}
	return R0;
}      
 
 
unsigned int AccountAlfa(void)
{
    int R0;
    unsigned int AlfaTemp;

    AlfaTemp = Alfa;
    R0 = UOs-UNomWork;

    if(R0>=0)	
	{
	if((R0>200)&&(AlfaTemp<2550))
		AlfaTemp = 2550;
	else
		{
		R0=R0>>4;//1B=1bit
		if(R0>160)
			R0 = 160;

		AlfaTemp = AlfaTemp+R0;//U-
		AlfaTemp = AccountAlfaDT(AlfaTemp);
		}
	}
	else 
		if(R0<0)
		{
		R0=~R0;
		R0=R0>>4;//1B=1bit
		
		if(R0>160)
			R0=160;

		AlfaTemp = AlfaTemp-R0;//U+
		AlfaTemp = AccountAlfaDT(AlfaTemp);
		}

	if(AlfaTemp > AlfaMax)
		AlfaTemp = AlfaMax;

	if(AlfaTemp < AlfaMin)
		AlfaTemp = AlfaMin;
    
	return AlfaTemp;
}



unsigned int AccountAlfaStart(void)
{
    int R0;
    unsigned int AlfaTemp;

    AlfaTemp = Alfa;
    R0 = UOs-UNomWork;

    if(R0>=0)	
	{
	AlfaTemp += 10;
	}
	else if(R0<0)
	{
	AlfaTemp -= 10;;
	}


	if(AlfaTemp>AlfaMax)
		AlfaTemp = AlfaMax;

	if(AlfaTemp<AlfaMin)
		AlfaTemp = AlfaMin;
    
	return AlfaTemp;
}


void InitIndStart(void)
{
	unsigned int R0;
	
	PORTD &=0xf7;
	R0=65000;
	while(R0--) _WDR();
	
	PORTD |=0x8;
	R0=5000;
	while(R0--) _WDR();
	
	SPDR=0x1b;
	R0=100;
	while(R0--) _WDR();
	
	SPDR=0x74;
	R0=100;
	while(R0--) _WDR();
	
	SPDR=17;//Kirilica 

	R0=100;
	while(R0--) _WDR();
	
	SPDR=0xc;//clear ind
}


void InitInd(void)
{
	unsigned int R0;
	
	PORTD &=0xf7;
	R0=1500;
	while(R0--) _WDR();
	
	PORTD |=0x8;
	R0=5000;
	while(R0--) _WDR();
	
	SPDR=0x1b;
	R0=100;
	while(R0--) _WDR();
	
	SPDR=0x74;
	R0=100;
	while(R0--) _WDR();
	
	SPDR=17;//Kirilica 
	R0=100;
	while(R0--) _WDR();  
	
	SPDR=0xc;//clear ind
}

   
void AccountUOs(void)
{
	unsigned long R0;
	
	R0=AdResult[0];
	R0=R0*45;
	R0=R0>>4;
	UOs=R0;//AdResult[0]>>4;
}
	   
unsigned int ChangeUst(int unsigned R0)   
{
	if(!CtUst)
		{
		if(RegS & 0x2)
			R0 +=1;
		else   
			if(RegS & 0x1)
				R0 -=1;

		CtUst=CtUst0;
		}
	return R0;    
}

/*	void	AccountTemper(void)
	{
	unsigned long R0;
	unsigned long R1;
	R0=AdResult[3]>>4;
	R1=AdResult[30]>>4;
	R0=R0*R1;
	R0=R0/250;
	Temperatura=R0;
	}*/	
void AccountISum(void)
{
	unsigned int R0;
	R0 = AdResult[4];
	R0 += AdResult[5];

	ISum = R0>>3;
}	
	
void AccountUNomWork(void)
{
	unsigned int R0;
	R0 = ISum;
	//	R0 =R0>>3;//140;
	UNomWork=UNom+R0;
}	
	
void AccountAlfaI(void)
{
unsigned int R0;

	R0=ISum;
	if((AlfaI !=400)&&(R0>=100))
		AlfaI = 600;
	else if(R0 >= 105)
			AlfaI = 600;
	else if((AlfaI != 600)&&(AlfaI != 300)&&(R0 < 100)&&(R0 >= 90))
			AlfaI = 400;
	else if((AlfaI == 600)&&(R0 <= 95)&&(R0 >= 90))
			AlfaI = 400;
	else if((AlfaI == 300)&&(R0>=95)&&(R0<100))
			AlfaI = 400;

	else if((AlfaI !=400)&&(AlfaI !=200)&&(R0<90)&&(R0>=80))
			AlfaI=300;
	else if((AlfaI ==400)&&(R0<=85)&&(R0>=80))
			AlfaI=300;
	else if((AlfaI ==200)&&(R0>=85)&&(R0<90))
			AlfaI=300;

	else if((AlfaI !=300)&&(AlfaI !=150)&&(R0<80)&&(R0>=70))
			AlfaI=200;
	else if((AlfaI ==300)&&(R0<=75)&&(R0>=70))
			AlfaI=200;
	else if((AlfaI ==150)&&(R0>=75)&&(R0<80))
			AlfaI=200;

	else if((AlfaI !=200)&&(AlfaI !=100)&&(R0<70)&&(R0>=60))
			AlfaI=150;
	else if((AlfaI ==200)&&(R0<=65)&&(R0>=60))
			AlfaI=150;
	else if((AlfaI ==100)&&(R0>=65)&&(R0<70))
			AlfaI=150;

	else if((AlfaI !=150)&&(AlfaI !=80)&&(R0<60)&&(R0>=50))
			AlfaI=100;
	else if((AlfaI ==150)&&(R0<=55)&&(R0>=50))
			AlfaI=100;
	else if((AlfaI ==80)&&(R0>=55)&&(R0<60))
			AlfaI=100;

	else if((AlfaI !=100)&&(AlfaI !=60)&&(R0<50)&&(R0>=40))
			AlfaI=80;
	else if((AlfaI ==100)&&(R0<=45)&&(R0>=40))
			AlfaI=80;
	else if((AlfaI ==60)&&(R0>=45)&&(R0<50))
			AlfaI=80;

	else if((AlfaI !=80)&&(AlfaI !=40)&&(R0<40)&&(R0>=30))
			AlfaI=60;
	else if((AlfaI ==80)&&(R0<=35)&&(R0>=30))
			AlfaI=60;
	else if((AlfaI ==40)&&(R0>=35)&&(R0<40))
			AlfaI=60;

	else if((AlfaI !=60)&&(AlfaI !=20)&&(R0<30)&&(R0>=20))
			AlfaI=40;
	else if((AlfaI ==60)&&(R0<=25)&&(R0>=20))
			AlfaI=40;
	else if((AlfaI ==20)&&(R0>=25)&&(R0<30))
			AlfaI=40;

	else if((AlfaI !=40)&&(AlfaI !=0)&&(R0<20)&&(R0>=10))
			AlfaI=20;
	else if((AlfaI ==40)&&(R0<=15)&&(R0>=10))
			AlfaI=20;
	else if((AlfaI ==0)&&(R0>=15)&&(R0<20))
			AlfaI=20;

	else if((AlfaI !=20)&&(R0<10))
			AlfaI=0;
	else if((AlfaI ==20)&&(R0<=5))
			AlfaI=0;

}	
		   
/*++++++++++++++++++++++++++++++++++++++++++*/
int main(void)
{
	//	IndAvaria();    

	//if DDRX,X=1 PORTX,X=OUT
	DDRA = 0;
	PORTA = 0;

	DDRB = 0xa0;
	PORTB = 0xff;// Control UAB

	DDRC = 0x24;
	PORTC = 0xff;/*VT3=off*/

	DDRD = 0xb9;
	PORTD = 0xff;/*imp=off,Reset=on*/

	InitAd();
	//Init PWM

	ICR1 = 3200;
	Alfa = 3200;
	AlfaRez = 3200;
	OCR1A = AlfaRez;
	TCCR1A = 0x82;
	TCCR1B = 0x019;

	TIMSK = TIMSK | 0x4;//enable Int overlowT1

	CtStart = 65000;//пауза перед запуском
	while(CtStart--) _WDR();
	
	PORTD &= 0xef;/*imp=on*/
	MCUCR &= 0xfc;
	GIFR |= (1<<INTF0);/*reset INT0*/
	GICR |= (1<<INT0);/*Enable INT0*/
	Avaria = 0;
	//  INIT SPI
	SPCR = 0x72;//f/64
	SPSR = 0;//f/64
	
	InitIndStart();
	
	CtInitInd = 100;
	
	RegimError = 0;
	CtError = CtError0;
	Avaria = 0;	

	_SEI();
	
	CtEeprom = 0;
	CtUst = CtUst0;
	Reg0 = ReadEeprom(1);	
	Reg0 = Reg0<<3;
	UNom = 1800+Reg0;
	
	if(UNom>UNomMax)
		UNom = UNomMax;
	if(UNom<UNomMin)
		UNom = UNomMin;
		
	CtStart = 200;
	CtErrorInt0 = 5;
	CtKn = 5;	

	while(1)
	{
		_WDR();
		
		AccountUNomWork();
		
		if(CtStart)//пауза перед запуском
		{
			--CtStart;
			RegimError = 0;
			CtError = CtError0;
			Avaria = 0;
		}
		
		if(EndAd) //готовые данные АЦП по всем каналам
		{

			RegimError &= 0xfe;
			
			if(!CtError)
				Avaria = Avaria | 0x2;

			UOsOld = UOs;
			AccountUOs();
			
			if((!Avaria) && (RegS & 4))
			{
				if(CtStart)
					Alfa = AccountAlfaStart();
				else
					Alfa = AccountAlfa();
				if(Alfa >= AlfaI)
					AlfaRez = Alfa-AlfaI;
				else
					AlfaRez = Alfa;
			}
			else
			{
				AlfaRez = ICR1;
				Alfa = ICR1;
			}
			
			OCR1A = AlfaRez;
			UNom = ChangeUst(UNom);
			
			if(UNom>UNomMax)
				UNom = UNomMax;
			if(UNom<UNomMin)
				UNom = UNomMin;

			Reg0 = UNom-1800;
			Reg0 = Reg0>>3;
			Reg1 = Reg0;
			
			if(Reg1 != ReadEeprom(1))
			{
				WriteEeprom(1,Reg1);
				++CtEeprom;
			}
			
			AccountISum();
			AccountAlfaI();

			EndAd = 0;
		}

		ReadKn();
		
		if(!CtInitInd)
		{
			DecSegm(UNom);
			SetCursor(0,0);
			IndPar();

			DecSegm(ISum);//Avaria);
			SetCursor(32,0);
			IndPar();

			DecSegm(UNomWork);//AdTest1);//Result[4]>>3);//Temperatura);
			SetCursor(64,0);
			IndPar();
			DecSegm(Alfa);//AdResult[5]>>3);//RegS);
			SetCursor(0,1);
			IndPar();

			DecSegm(UOs);
			SetCursor(32,1);
			IndPar();

			DecSegm(AlfaRez);
			SetCursor(64,1);
			IndPar();
			CtInitInd = 2;
		}
	}
}


SIGNAL(SIG_ADC)
{
	unsigned char R0;
	unsigned int R1;
	
	++AdTest;
	PORTC |= 4;
	RegInt0 = ADC;
	//	if(NumberAd==6)
	//	AdTest=RegInt0;
	if(CtAd < CtAd0)
	{
		if(RegInt0 > AdTemp[NumberAd])
			AdTemp[NumberAd] = RegInt0;
		if(RegInt0 < AdTempMin[NumberAd])
			AdTempMin[NumberAd] = RegInt0;
	}
	if(CtAd)
		--CtAd;
	else if(CtAdSum)
	{
		--CtAdSum;
		if(!NumberAd)
			AdTempResult[NumberAd] += AdTemp[NumberAd];
		else
		{
			if(AdTemp[NumberAd] >= AdTempMin[NumberAd])
				R1 = AdTemp[NumberAd] - AdTempMin[NumberAd];
			else
				R1 = 0;
				
			AdTempResult[NumberAd] += R1;
		}
		AdTemp[NumberAd] = 0;
		AdTempMin[NumberAd] = 0x3ff;
		CtAd = CtAd0+2;
	}
	else
	{
		if(!NumberAd)
			EndAd = 1;
			
		AdResult[NumberAd] = AdTempResult[NumberAd];
		AdTemp[NumberAd] = 0;
		CtAd = CtAd0+2;
		CtAdSum = 1;//CtAdSum0;
		AdTempResult[NumberAd] = 0;
		ADCSRA |= (1<<ADSC);
		
		switch(NumberAd)
		{
			case 0:NumberAd = 5;
				break;
			case 4:NumberAd = 0;
				break;
			case 5:NumberAd = 4;
				break;
			default:NumberAd = 0;
				break;
		}
		//		NumberAd=6;
		R0 = ADMUX;
		R0 = R0 & 0xe0;
		R0 = R0 | NumberAd;
		ADMUX = R0;
		CtAd = CtAd0+2;
	}
	//   ADCSRA |=(1<<ADSC);/* Start*/
	PORTC &= 0xfb;    	
}
	
	
SIGNAL(SIG_INTERRUPT0)
{
	if((Alfa != ICR1) && (!(PIND & 4)))
	{
		PORTD |= 0x10;
		if(CtErrorInt0)
		{
			--CtErrorInt0;
			PORTD &= 0xef;
		}
		else
		{				
			GICR = GICR & 0xbf;/*Denable INT0*/
			Avaria = Avaria | 1;/*IIn>IMax*/
			AlfaRez = ICR1;
			Alfa = ICR1;
			OCR1A = AlfaRez;
		}
	}
}


SIGNAL(SIG_OVERFLOW1)/*32.7 Mc*/
{
	if(CtInitInd)
		--CtInitInd;
		
	CtErrorInt0 = 5;
	
	if(RegimError & 1)
	{
		if(CtError)
			--CtError;
	}
	else
		CtError = CtError0;
		
	if(CtUst)
		--CtUst;
}
