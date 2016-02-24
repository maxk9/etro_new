
    #include <compat/ina90.h>
    #include <avr/iom8535.h>
    #include <avr/interrupt.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include <avr/pgmspace.h>
/*Avaria
0	KZ
1	U>250
2	Otkaz vozb.
3	U<205
4	I>1.2INom
5	I>Inom
6	KZ+T for off 50,100%
7 	NDiz<1300
*/
    unsigned char CtOff;
    unsigned char RegimOff;
    unsigned char CtKn;
    unsigned char CtEnableInd;
    const unsigned char CtEnableInd0=20;
    unsigned char RegSDiz;
    unsigned char RegSDizOld;

    unsigned char Regim;
    unsigned int RegIntInd0;
    unsigned char RegIntInd1;
    unsigned char CtInitInd;
	unsigned char CtTimeInd;
	unsigned char CtNumberInd;
	unsigned long IndicatorBit;
	unsigned char EnableInd;

    const    unsigned int INom100_1_15=384;
    const	unsigned int INom100_1_2=400;
	unsigned    int NDiz;
	unsigned    char TG;

    unsigned int RegInt0;
 	unsigned    char Reg101;
 	unsigned    int Reg100;


	unsigned    int	TGenTemp[16];
    unsigned    char CtTGen;
    unsigned    char CtOverLow;
    unsigned    char IndexInt;
	unsigned    int	UU;
	unsigned    int	UV;
	unsigned    int	UW;
	unsigned    int	UF;

	unsigned    int	IU;
	unsigned    int	IV;
	unsigned    int	IW;
	unsigned    int	IMax;
	unsigned    int	UMax;
	unsigned    int	UMin;
	unsigned    int	FGen;
    unsigned    char	CtAdSum;
	static const unsigned char CtAdSum0=16;


	unsigned    int	AdTempResult;

	unsigned    int	AdTempMax;
	unsigned    int	AdTempMin;
 	unsigned    int	AdResult[7];
    unsigned    char RegimError;
 	unsigned    int	CtError[8];
    static const unsigned  int CtError0[8]=
    {30,150,300,350,55000,200,100,200};
    static const unsigned  char KoefMax[9]=
    {240,240,240,240,240,240,100,240,240};
    static const unsigned  char KoefMin[9]=
    {40,40,40,40,40,40,2,2,40};
    static const unsigned  char Koef0[9]=
    {0x84,0x7c,0x7f,0x8d,0x7a,0x7a,5,0x89,0x7d};
 	unsigned    char	Koef[9];


	unsigned    int    ICR1U;
	unsigned    int    TGen;


	unsigned    int    Reg0;


 	unsigned    char             Avaria;
 	unsigned    char            RomReceive[9];
 	unsigned    char            RegTransmit[12];
    unsigned    char            NumberLink;
    unsigned    int             CtTWCR;
 	unsigned    char            RamReceive[12];
	const   unsigned    int     CtTWCR0=10000;//500;
    unsigned    char             CtErrorLink;
	const   unsigned    char     CtErrorLink0=30;    
	unsigned    char            CtTransmit;
	unsigned    char            CtReceive;

	unsigned    char    CtInd;
	 const unsigned    char	    CtInd0=8;


    unsigned    char    SregTemp;   


	unsigned    char    IndData[6];


	unsigned    char	NumberAd;
    unsigned    int	CtAd;

	static const unsigned    int	    CtAd0=120;//3msec
 
//    unsigned    char	MyStatus;

	unsigned    int	RegS,RegSTemp,RegSOld;
       
    /*============================================*/
 


 
void ReadKn(void)
{

	unsigned    int Reg2;
	Reg2=0;
	
	if(bit_is_clear(PIND,PD3))
		Reg2=Reg2 | 1;

	if(bit_is_clear(PIND,PD5))
		Reg2=Reg2 | 2;

	if(bit_is_clear(PIND,PD7))
		Reg2=Reg2 | 4;

	if(bit_is_clear(PIND,PD4))
		Reg2=Reg2 | 8;

	if(bit_is_clear(PINC,PC5))
		Reg2=Reg2 | 0x10;

	if(bit_is_clear(PINC,PC6))
		Reg2=Reg2 | 0x20;

	if(bit_is_clear(PINB,PB5))
		Reg2=Reg2 | 0x40;

	if(bit_is_clear(PINB,PB6))
		Reg2=Reg2 | 0x80;
	if(bit_is_clear(PINB,PB7))
		Reg2=Reg2 | 0x100;

	if(Reg2 == RegSTemp)
	{
		if(CtKn)
			--CtKn;
		else
			RegS=Reg2;
	}
	else
		CtKn=5;

	RegSTemp=Reg2;
}





void DecSegm( unsigned int Arg0)
{
	unsigned int	Arg1;

	if(Arg0>9999)
		Arg0=9999;
		
	Arg1=Arg0/1000;
	IndData[3]=Arg1;
	Arg0=Arg0-(Arg1*1000);
	Arg1=Arg0/100;
	IndData[2]=Arg1;
	Arg0=Arg0-(Arg1*100);
	Arg1=Arg0/10;
	IndData[1]=Arg1;
	Arg0=Arg0-(Arg1*10);
	IndData[0]=Arg0;

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

	while	(bit_is_set(EECR,EERE));

	return EEDR;
}



void InitAd (void)
{
	ADMUX = 0xc0;//2.56=Ref
	SFIOR = SFIOR & 0x1f;//start from end AD
	CtAd = CtAd0;
	NumberAd = 0;

	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);/*enable AD*/
	ADCSRA |= (1<<ADPS2);
	//    ADCSRA |=(1<<ADPS1);
	ADCSRA |= (1<<ADATE);//Auto trigger enable
	ADCSRA |= (1<<ADPS0); /*32*/	 		
	ADCSRA |= (1<<ADIE);/* enable interrupt*/
	ADCSRA |= (1<<ADSC);/* Start*/
}

void IndPar(void)
{
	while(!(UCSRA & 0x20))_WDR();

	UDR=IndData[3]+0x30;


	while(!(UCSRA & 0x20))_WDR();

	UDR=IndData[2]+0x30;

	while(!(UCSRA & 0x20))_WDR();

	UDR=IndData[1]+0x30;

	while(!(UCSRA & 0x20))_WDR();
	
	UDR=IndData[0]+0x30;

}
void IndPar3(void)
{
	while(!(UCSRA & 0x20))	_WDR();
	
	UDR=IndData[2]+0x30;
	
	while(!(UCSRA & 0x20))	_WDR();
	
	UDR=IndData[1]+0x30;

	while(!(UCSRA & 0x20))	_WDR();
	
	UDR=IndData[0]+0x30;
}
void ClearInd(void)
{
	while(!(UCSRA & 0x20))//!RegInd)
	_WDR();
	UDR=0xc;
}

void SetCursor(unsigned char R0,unsigned char R1)
{
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=0x1f;
	
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=0x24;
	
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=R0;
	
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=0;
	
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=R1;
	
	while(!(UCSRA & 0x20))_WDR();
	
	UDR=0;

}

void TransmitInd(unsigned char R1)
{
	if(R1>=192)
		R1=R1-64;
	while(!(UCSRA & 0x20))_WDR();    
	
	UDR=R1;           
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

unsigned char Transmit(void)
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
		TWDR=RegTransmit[CtTransmit];
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
			TWDR=RegTransmit[CtTransmit];
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

unsigned char Receive(void)
{
	if(!WaitTWCR())
		return 0;

	if((TWSR & 0xf8) == 0x8)//Well Start
	{
		TWDR=5;//0x5;//SLA+R
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



void ReceiveTransmitMaster(void)
{
	//ControlSum for Transmit
	RegTransmit[0]=0;

	for(Reg0=1;Reg0<=10;++Reg0)
	{
	//    RegTransmit[Reg0][0]=3;
		RegTransmit[0] +=RegTransmit[Reg0];
	//    RegTransmit[Reg0][1]=4;
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
	{
		_WDR();
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
			for(Reg0=1;Reg0<=8;++Reg0)
				RomReceive[Reg0]=RamReceive[Reg0];
				
			CtErrorLink=CtErrorLink0;
		}
	}
	else
	{
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	}
}
void LoadRegTransmit(void)
{
	RegTransmit[1] = RegS;    
	RegTransmit[2] = Avaria;
	RegTransmit[3] = CtInd;
	RegTransmit[4] = Koef[8];//NDiz
	RegTransmit[5] = Koef[7];
} 
  
int unsigned AccountU(unsigned char Arg0)
{
	unsigned int R0;
	unsigned int R1;
	//unsigned char R2;
	//unsigned int  R3;

	R0=AdResult[Arg0]>>2;
	//R0=R0>>2;
	//R2=Arg0;
	R1=Koef[Arg0]*R0;
	//R1=R0*R1;
	R1=R1>>8;
	R0 += R1;
	//R3=R0;
	return R0; 
}

int unsigned AccountI(unsigned char Arg0)
{
	unsigned int R0;
	unsigned int R1;
	unsigned char R2;
	unsigned int  R3;

	R0=AdResult[Arg0]>>1;
	R2=Arg0;
	R1=Koef[Arg0];
	R1=R0*R1;
	R1=R1>>8;
	R0 +=R1;
	R3=R0;
	return R3; 
}


void ChangeCtInd(void)
{
	if((RegS & 0x30) != (RegSOld & 0x30))
	{
	if((RegS & 0x10) && (!(RegSOld & 0x10)))
		++CtInd;
	else if((RegS & 0x20) && (!(RegSOld & 0x20)))
		--CtInd;
	RegSOld=RegS;
	}

}


void Kalibrovka(unsigned char Arg0)
{
//Arg0=CtInd

	unsigned char Arg2;
	if((RegSDiz  & 0xc0) != (RegSDizOld & 0xc0))
	{
		if((RegSDiz & 0x40) && (!(RegSDizOld & 0x40)))
			++CtInd;
		else if((RegSDiz & 0x80) && (!(RegSDizOld & 0x80)))
			--CtInd;
		RegSDizOld=RegSDiz;
		if(CtInd==0xff)
			CtInd=CtInd0;
		else if(CtInd>CtInd0)
				CtInd=0;

	}
	if((RegS & 0x30) != (RegSOld & 0x30))
	{
		if((RegS & 0x10) && (!(RegSOld & 0x10)))
			++Koef[CtInd];
		else if((RegS & 0x20) && (!(RegSOld & 0x20)))
			--Koef[CtInd];
		if(Koef[CtInd] !=ReadEeprom(CtInd+1))
		{

		Arg2=Koef[CtInd];
		WriteEeprom((CtInd+1),Arg2);
		}
		RegSOld=RegS;
	}
}
void AccountIMax(void)
{
	IMax=IU;
	if(IV>IMax)
		IMax=IV;
	if(IW>IMax)
		IMax=IW;
}   
void AccountUMin(void)
{
	UMin=UU;
	if(UV<UMin)
		UMin=UV;
	if(UW<UMin)
		UMin=UW;
} 

void AccountUMax(void)
{
	UMax=UU;
	if(UV>UMax)
		UMax=UV;
	if(UW>UMax)
		UMax=UW;
}

void SetAvaria(void)
{
	unsigned char R0;   
	for(R0=0;R0<=7;++R0)
	{
		if(!(CtError[R0]))
			Avaria |=(1<<R0);

	}
	for(R0=4;R0<=5;++R0)
	{
		if(CtError[R0]==CtError0[R0])
		{
			Avaria = ~Avaria;
			Avaria |=(1<<R0);
			Avaria = ~Avaria;
		}				
	}

	if(Avaria & 4)
	Avaria &=0xfff7;


}


void ResetAvaria(void)
{
	unsigned char R0;
	Avaria=0;
	RegimError=0;      
	for(R0=0;R0<=7;++R0)
		CtError[R0]=CtError0[R0];
}
/*
Avaria
0   KZ
1   U>UMax
2   No Vozb
3   U<Umin
4	Peregruz 1.2 
5	Peregruz 1.0
6	KZ + T
7
*/


void SetRegimError(void)
{
	if((UMin<100) && (IMax>250))//KZ
		RegimError |=1;
	else
		RegimError &=0xfe;
		
	if(UMax>250)
		RegimError |=2;
	else
		RegimError &=0xfd;
		
	if(!(PORTC & 8))//vozb=0n
	{
		if(NDiz<1300)
			RegimError |=0x80;
		else
			RegimError &=0x7f;
		if(UMax<100)
			RegimError |=4;
		else
			RegimError &=0xfb;
		if(!(Avaria & 4))
		{
			if(UMin<205)
				RegimError |=8;
			else
				RegimError &=0xf7;
		}
		else
			RegimError &=0xf7;
	}
	else
		RegimError &=0xf3;
		
	if(IMax>INom100_1_2)
		RegimError |=0x10;
	else if(IMax>INom100_1_15)
		{
		RegimError |=0x20;
		RegimError &=0xef;
		}
		else
			RegimError &=0xcf;
			
	if(Avaria & 1)//KZ
		RegimError |=0x40;
	else
		RegimError &=0xbf;

}

void VozbOnOff(void)
{
	unsigned char R0;
	if(RegSDiz & 8)//prot=on
		R0=0xbf;
	else
		R0 = 0xbc;
	if(Avaria & 0x4f)
		PORTC |= 8;//vozb=off
	else if(RomReceive[5] & 1)//stop from diz prov
			PORTC |= 8;
		else if(RomReceive[1] & R0)//Avaria Diz
				PORTC |=8;
			else if(!(RegS & 8))//norma=off
					PORTC |=8;
				else if(NDiz>1450)
						PORTC &=0xf7;//Vozb=on
}


void FiderOnOff(void)
{
	unsigned char R0;
	if(RegSDiz & 8)//prot=on
		R0=0xbf;
	else
		R0=0xbc;

	if(Avaria & 0xde)
		RegimOff=55;
	else if(RomReceive[5] & 5)//stop from diz prov
			RegimOff=55;
		else if(RomReceive[1]  & R0)//Avaria Diz
				RegimOff=55;
			else if(NDiz<1000)
					RegimOff=55;
				else if(!(RegS & 8))//norma=off
						RegimOff=55;
					else
						RegimOff=0;

	if(!CtOff)
		PORTC &=0xeb;
	else if((UMin>205) && (RomReceive[4]>=37))
			PORTC |=0x14;




	if(Avaria & 0xde)
		PORTC &=0xeb;
	else if(RomReceive[5] & 5)//stop from diz prov
			PORTC &=0xeb;
		else if(RomReceive[1]  & R0)//Avaria Diz
				PORTC &=0xeb;
			else if(NDiz<1000)
					PORTC &=0xeb;
				else if(!(RegS & 8))//norma=off
						PORTC &=0xeb;
					else if((UMin>205) && (RomReceive[4]>=37))
							PORTC |=0x14;
}



void ClearPoz(unsigned char R0)
{
	static unsigned const char  Tab[]="         ";
	unsigned char R2;
	unsigned char R3;

	for(R2=0;R2<=R0;++R2)
		{
			R3=Tab[R2];
			TransmitInd(R3);
		}

}



void CalcNDiz(void)
{
	unsigned int R0;

	R0=RomReceive[3]<<8;
	R0=R0 & 0x700;
	R0=R0|RomReceive[2];
	NDiz=R0;

}
void IndNDiz(void)
{
	static const char  Tab[]="N ���";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(1);

	for(R0=0;R0<=4;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(NDiz);
	IndPar();
}


void IndIU(void)
{
	static const char  Tab[]="IU";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(IU);
	IndPar3();
}
void IndIV(void)
{
	static const char  Tab[]="IV";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(IV);
	IndPar3();
}

void IndIW(void)
{
	static const char  Tab[]="IW";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(IW);
	IndPar3();
}

void IndUU(void)
{
	static const char  Tab[]="UU";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(UU);
	IndPar3();
}

void IndUV(void)
{
	static const char  Tab[]="UV";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(UV);
	IndPar3();
}

void IndUW(void)
{
	static const char  Tab[]="UW";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(UW);
	IndPar3();
}

void IndUF(void)
{
	static const char  Tab[]="U�";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(4);

	for(R0=0;R0<=1;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(UF);
	IndPar3();
}

void IndTG(void)
{
	static const char  Tab[]="���";
	unsigned char R0,R1;

	SetCursor(0,0);
	ClearPoz(3);

	for(R0=0;R0<=2;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);
	DecSegm(RomReceive[4]);
	IndPar3();
}

void IndicatorKalibrovka(void)
{
	switch(CtInd)
	{
	case 0:IndIU();
		break;
	case 1:IndIV();
		break;
	case 2:IndIW();
		break;

	case 3:IndUU();
		break;
	case 4:IndUV();
		break;
	case 5:IndUW();
		break;
	case 6:IndUF();
		break;
	case 7:IndTG();
		break;
	case 8:IndNDiz();
		break;
	default:
		break;

	}
	ClearPoz(2);
}


void IndUMin(void)
{
	static const char  Tab[]="U� < ���.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(2);
	for(R0=0;R0<=9;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);

}
void IndUMax(void)
{
	static const char  Tab[]="U� > ���.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(2);
	for(R0=0;R0<=9;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);

}

void IndKZ(void)
{
	static const char  Tab[]="�� ��������";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(1);
	for(R0=0;R0<=10;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(1);

}

void IndPeregruz(void)
{
	static const char  Tab[]="����������";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(1);
	for(R0=0;R0<=9;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(2);

}

void IndTemp37(void)
{
	static const char  Tab[]="��� < 37";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=7;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(3);

}

void IndP10(void)
{
	static const char  Tab[]="ABAP.PM";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(4);
	for(R0=0;R0<=6;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(3);

}

void IndTemp95(void)
{
	static const char  Tab[]="ABAP.���";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=7;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(3);

}

void IndTM(void)
{
	static const char  Tab[]="ABAP.TM";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(4);
	for(R0=0;R0<=6;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(3);

}

void IndPusk(void)
{
	static const char  Tab[]="����";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(5);
	for(R0=0;R0<=3;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(5);

}

void IndStop(void)
{
	static const char  Tab[]="�������";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(4);
	for(R0=0;R0<=6;++R0)
	{

		R1=Tab[R0];
		TransmitInd(R1);
	}
	ClearPoz(3);

}
void IndRashol(void)
{
	static const char  Tab[]="������.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(4);
	for(R0=0;R0<=6;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(3);

}

void IndNorma(void)
{

	SetCursor(0,0);
	ClearPoz(2);
	DecSegm((IU*77)>>8);
	IndPar3();
	ClearPoz(0);
	DecSegm((IV*77)>>8);
	IndPar3();
	ClearPoz(0);
	DecSegm((IW*77)>>8);
	IndPar3();
	ClearPoz(5);

}
void IndKZF(void)
{
	static const char  Tab[]="����� ���";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=8;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(2);

}
void IndDU(void)
{
	static const char  Tab[]="����.���.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=8;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(2);

}
void IndMU(void)
{
	static const char  Tab[]="MECT.���.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=8;++R0)
		{

			R1=Tab[R0];
			TransmitInd(R1);
		}
	ClearPoz(2);

}


void IndRaznos(void)
{
	static const char  Tab[]="������";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(4);
	for(R0=0;R0<=6;++R0)
	{

		R1=Tab[R0];
		TransmitInd(R1);
	}
	ClearPoz(5);

}
void IndNoVozb(void)
{
	static const char  Tab[]="��T BO��.";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=8;++R0)
	{

		R1=Tab[R0];
		TransmitInd(R1);
	}
	ClearPoz(4);

}

void IndNoStop(void)
{
	static const char  Tab[]="��T �������A";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(1);
	for(R0=0;R0<=11;++R0)
	{

		R1=Tab[R0];
		TransmitInd(R1);
	}
	ClearPoz(2);
}


void IndNoPusk(void)
{
	static const char  Tab[]="��T ����A";
	unsigned char R0,R1;
	SetCursor(0,0);
	ClearPoz(3);
	for(R0=0;R0<=8;++R0)
	{

		R1=Tab[R0];
		TransmitInd(R1);
	}
	ClearPoz(2);
}


void IndicatorWork(void)
{
	IndicatorBit=0;
	IndicatorBit |=(RomReceive[1]& 0xbf);
	if(RomReceive[5] & 4)//temp stop
		IndicatorBit |=0x20000;
	else
		IndicatorBit &=0xdffff;
		
	if(NDiz<400)
		IndicatorBit &=0xdffff;//temp stop=off
		
	if((RomReceive[4]<37) && (UMin>=205) &&(!( PORTC & 0x14)))	
		IndicatorBit |=0x1000;
	else
		IndicatorBit &=0xfefff;

	if(RomReceive[5] & 1) //stop
	{
		IndicatorBit |=0x2000;
		IndicatorBit &=0xdefff;//temp stop=off,T<37=off
	}
	else
		IndicatorBit &=0xfdfff;
		
	if(IndicatorBit & 0x20)//Bad Stop
		IndicatorBit &=0xfcfff;//Stop=off,T<37=off

	if(Avaria & 1)
		IndicatorBit |=0x100;
	else
		IndicatorBit &=0xffeff;


	if(Avaria & 2)
	{
		IndicatorBit |=0x200;
	}
	else
		IndicatorBit &=0xffdff;
	if(Avaria & 4)
		IndicatorBit |=0x400;
	else
		IndicatorBit &=0xffbff;
	if(Avaria & 8)
		IndicatorBit |=0x800;
	else
		IndicatorBit &=0xff7ff;
	if(Avaria & 0xb0)
		IndicatorBit |=0x40;
	else
		IndicatorBit &=0xfffbf;
	if(!(RegS & 8))//norma KZF=off
		IndicatorBit |=0x80000;
	else
		IndicatorBit &=0x7ffff;

	if(RomReceive[5] & 2)//Pusk
		IndicatorBit |=0x4000;
	else
		IndicatorBit &=0xfbfff;

	if(RegS & 2)//MU
	{
		IndicatorBit =0x10000;

	}
	else
	{
		IndicatorBit &=0xeffff;

		if(!IndicatorBit)
		{
			if((PORTC & 8)||(!(PORTC & 0x10)))//Vozb=off
				IndicatorBit |=0x8000;//DU
			else
			{
				IndicatorBit &=0xf7fff;//DU=off				
				IndicatorBit |=0x40000;
			}
		}
		else
			IndicatorBit &=0xa7fff;
	}

	if(EnableInd)
	{

	switch(CtNumberInd)
	{
	case 0:	IndTM();
	break;
	case 1:	IndTemp95();//
	break;
	case 2:	IndP10();
	break;
	case 3:	IndRaznos();
	break;
	case 4:	IndNoPusk();
	break;
	case 5: IndNoStop();
	break;
	case 6:	IndPeregruz();
	break;
	case 7:	IndNoPusk();
	break;
	case 8:	IndKZ();
	break;
	case 9: IndUMax();
	break;
	case 10:IndNoVozb();
	break;
	case 11:IndUMin();
	break;
	case 12: IndTemp37();
	break;
	case 13:IndStop();
	break;
	case 14:IndPusk();
	break;
	case 15:IndDU();
	break;
	case 16: IndMU();
	break;
	case 17: IndRashol();
	break;
	case 18: IndNorma();
	break;
	case 19: IndKZF();
	break;

	default:

	break;
	}


	}

}	

void InitInd(void)
{
	unsigned int R0;
	
	PORTD &=0xfb;//reset
	
	R0=50;
	while(R0--) _WDR();
	
	PORTD |=0x4;
	
	while(PIND & 1) _WDR();//busy
	
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR=0x1b;
	
	while(!(UCSRA & 0x20)) _WDR();+
	
	UDR=0x74;
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR=17;//Kirilica 
	
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR=0xc;//clear ind   
}

void InitIndStart(void)
{
	unsigned int R0;
	
	PORTD &= 0xfb;//reset
	
	R0 = 5000;
	while(R0--) _WDR();
	
	PORTD |= 0x4;
	
	while(PIND & 1) _WDR();//busy
	
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR = 0x1b;
	
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR = 0x74;
	
	while(!(UCSRA & 0x20)) _WDR();
	
	UDR = 17;//Kirilica 
	
	while(!(UCSRA & 0x20)) _WDR();

	UDR = 0xc;//clear ind
} 	

/*++++++++++++++++++++++++++++++++++++++++++*/


int main(void)
{
	//if DDRX,X=1 PORTX,X=OUT

	DDRB=0;
	PORTB |= 0xff;

	DDRC=0x1c;
	PORTC |=0xff;

	DDRD=0x6;
	PORTD=0xff;

	//INIT TWI;
	TWBR=0x7f;//F TWI

	//Init capture1
	TCCR1B=0x82;//0.5mkc,falling edge
	TIMSK=TIMSK | 0x4;//enable Int overlowT1
	//    TIMSK=TIMSK | 0x20;//enable Int capture1 prov

	//INIT USART
	UCSRA=2;//������� ��������
	UCSRB=8;//enable transmit
	UCSRC=6;//8 bit
	UBRRH=0;
	UBRRL=51;
	InitIndStart();


	InitAd();
	_SEI();
	//Init Comparator
	SFIOR &= 0xf7;//PB2,PB3
	ACSR = 0;
	ACSR |= 0x3;//rising edge
	//    ACSR |=0x8;//enable interrupt prov

	ClearInd();  
	ResetAvaria();
	RegimOff=0;
	CtOff=50;
	
	for(Reg101 = 0; Reg101 <= 8; ++Reg101)
		Koef[Reg101]=ReadEeprom(Reg101+1);
		
	for(Reg101 = 0; Reg101 <= 8; ++Reg101)
		{
			if(Koef[Reg101]>KoefMax[Reg101])
				Koef[Reg101]=Koef0[Reg101];
				
			if(Koef[Reg101]<KoefMin[Reg101])
				Koef[Reg101]=Koef0[Reg101];
		}

	RomReceive[4]=0;
	RomReceive[6]=0;

	RegSDiz=0;
	RegSDizOld=0;
	CtKn=5;
	
	for(Reg101=0;Reg101<=6;++Reg101)
	{
		ReadKn();
		Reg0=600;
		while(Reg0)
			--Reg0;
	}

	if((RegS & 0x10) && (RegS & 0x20) && (RegS & 2))//MU)
		Regim=55;
		
	while(RegS & 0x30)
	{
		_WDR();
		ReadKn();
	}
	
	while(1)
	{
		_WDR();

		RegSDiz = RomReceive[6];
		
		if(!CtInitInd)
		{
			InitInd();
			CtInitInd=200;
		}

		if(!CtErrorLink)
		{
		//INIT TWI;
			TWBR=0;//F TWI
			TWAR=0;
			TWCR =0;
			TWSR=0xf8;

			TWBR=0x7f;//F TWI

			CtErrorLink = CtErrorLink0;
		}
		ReadKn();


		LoadRegTransmit();//++RegTransmit
		ReceiveTransmitMaster();
		UU = AccountU(3);
		UV = AccountU(4);
		UW = AccountU(5);
		UF = AccountU(6);
		IU = AccountI(0);
		IV = AccountI(1);
		IW = AccountI(2);
		AccountIMax();
		AccountUMin();
		AccountUMax();
		
		//    CalcFGen();
		CalcNDiz();
		
		if(RegS & 4)//CA
			ResetAvaria();
			
		if(!(RegS & 2))//DU
			{
				Regim = 0;
				SetRegimError();
				SetAvaria();
				VozbOnOff();
				FiderOnOff();
			}
		else
			{
				ResetAvaria();
				PORTC |= 0x1c;
			}

		if(!Regim)
			IndicatorWork();
		else
			{
			Kalibrovka(CtInd);
				{
				IndicatorKalibrovka();
				}
			}
			
		Reg0 = 600;
		while(Reg0)
			--Reg0;
	}
}


SIGNAL(SIG_ADC)
{
	RegInt0=ADC;

	if(CtAd)
	{
		--CtAd;
		if(CtAd<(CtAd0-2))
		{		
			if(RegInt0>AdTempMax)
				AdTempMax=RegInt0;
				
			if(RegInt0<AdTempMin)
				AdTempMin=RegInt0;
		}
	}
	else if(CtAdSum)
		{
			--CtAdSum;
			RegInt0 = AdTempMax - AdTempMin;
			if(RegInt0 >= 5)
				RegInt0 -= 5;
			else
				RegInt0 = 0;
				
			AdTempResult += RegInt0;
			AdTempMax = 0;
			AdTempMin = 0xffff;
			CtAd = CtAd0+2;
		}
		else
		{
			AdResult[NumberAd] = AdTempResult>>4;

			CtAd = CtAd0+2;
			CtAdSum = CtAdSum0;
			AdTempResult = 0;
			AdTempMax = 0;
			AdTempMin = 0xffff;
			ADCSRA |= (1<<ADSC);
			switch(NumberAd)
				{
				case 0:NumberAd=6;
					break;
				case 1:NumberAd=0;
					break;
				case 2:NumberAd=1;
					break;
				case 3:NumberAd=2;
					break;
				case 4:NumberAd=3;
					break;
				case 5:NumberAd=4;
					break;
				case 6:NumberAd=5;
					break;
				default:NumberAd=0;
					break;
				}
			ADMUX = ADMUX & 0xe0;
			ADMUX = ADMUX | NumberAd;
			CtAd = CtAd0;
		}
} 

/* SIGNAL(SIG_INPUT_CAPTURE1)
{
//   ACSR |=0x8;//enable interrupt comparator
if(!CtOverLow)
TGen=0xffff;
else
{
TGenTemp[CtTGen]=ICR1-ICR1U;
ICR1U=ICR1;
}
if(CtTGen)
--CtTGen;
else
CtTGen=15;
CtOverLow=2;

}*/

SIGNAL(SIG_OVERFLOW1)/*26.2 Mc*/
{
	if(CtInitInd)
		--CtInitInd;
	if(RegimOff == 55)
	{
		if(CtOff)
			--CtOff;
	}
	else
		CtOff=50;

	/*    if(CtOverLow)
	--CtOverLow;
	else
	{

	TGenTemp[CtTGen]=0xffff;
	if(CtTGen)
	--CtTGen;
	else
	CtTGen=15;
	}*/

	for(IndexInt=0; IndexInt<=3; ++IndexInt)
	{
		if(RegimError & (1<<IndexInt))
		{
			if(CtError[IndexInt])
				--CtError[IndexInt];
		}
		else
			CtError[IndexInt]=CtError0[IndexInt];
	}
	
	if(RegimError & 0x10)//1.2 I100
	{
		if(CtError[4]>=360)
			CtError[4] -=360;
		else
			CtError[4] =0;
		if(CtError[5])
			--CtError[5];
	}
	else if(RegimError & 0x20)//1.0 I100
		{
			if(CtError[5])
				--CtError[5];
		}
		else if(!(RegimError & 0x30))
			{
				if(CtError[4]<CtError0[4])
					++CtError[4];
				if(CtError[5]<CtError0[5])
					++CtError[5];
			}

	if(RegimError & (1<<6))//for K1,K2 KZ
	{
		if(CtError[6])
			--CtError[6];
	}
	else
		CtError[6]=CtError0[6];
		
	if(RegimError & (1<<7))//N<1300
	{
		if(CtError[7])
			--CtError[7];
	}
	else
		CtError[7]=CtError0[7];



	if(CtErrorLink)
		--CtErrorLink;


	if(CtTimeInd)
		--CtTimeInd;
	else
	{
		if(CtNumberInd)
			--CtNumberInd;
		else
			CtNumberInd = 19;
			
		if(CtNumberInd >= 16)
		{
			RegIntInd1 = CtNumberInd-16;
			RegIntInd0 = IndicatorBit>>16;
		}
		else
		{
			RegIntInd1 = CtNumberInd;
			RegIntInd0 = IndicatorBit;
		}
		if(RegIntInd0 & (1<<RegIntInd1))
		{
			EnableInd = 1;
			CtTimeInd = 50;
		}
		else
		{
			CtTimeInd = 0;
			EnableInd = 0;
		}
	}
}


   

