
/*ArgusBA 8квт
с двиг Lombardgini
зубьев на колесе 108
*/

/* WDTON CKOPT
BOODLEVEL BODEN SUT1 CKSEL2
Копусов 
переделка на ВЕ4 проц 8 мгц
число зубьев 108 шт
*/  


 #include <compat/ina90.h>
 #include <avr/iom8535.h>
 #include <avr/interrupt.h>
 #include <math.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 #include <avr/pgmspace.h>
 
 
	volatile unsigned int RabReg0;
	unsigned int DeltaI;
	volatile unsigned int CtErrorBap;
	const unsigned int CtErrorBap0=99;
	unsigned char CtIGen;
	unsigned long IGenSum;
	unsigned long IGenSumTemp;
	unsigned int CtKalib;
	const unsigned int CtKalib0=300;
	unsigned char EnableKoef;
	// 	unsigned char	Led1Mig;
	unsigned char	RegLed1;
	unsigned char	Led1;
	unsigned char	Led2;
	unsigned char	CtPusk;
	unsigned char	RegimWork;
	volatile unsigned char	CtIntT1;
	
	volatile unsigned int	Error;
	volatile unsigned int	CtError[16];
	
	volatile const unsigned int	CtError0[16]={20,80,30,30,100,
													50,20,600,20,20,
											20,20,2500,20,20,50};//1=0.1s 
	
	volatile unsigned int	CtTime[16];
	volatile const unsigned int	CtTime0[16]={30,100,20,100,100,
													40,20,6000,3000,150,
											50,6000,100,100,100,20};
	
	
	volatile unsigned int	RegimTime; //
	volatile unsigned int	RegimError;
	unsigned char	SregTemp;
	unsigned char	KoefEeprom[8];
	const unsigned char		KoefMax[8]={80,100,100,200,70,200,100,145};
	const unsigned char		KoefMin[8]={2,  20,20, 2,  30,60, 2, 105};
	const unsigned char		Koef0[8]={ 10,  44,44,100, 51,130,30,145};
	
	unsigned char IKoefEeprom[6];
				
	const unsigned char IKoef0[6]={120,140,120,150,110,127}; 
	// Формула для расчета поправки - нулевая поправка при значении ячейки 127.
	// Начальные значения поправок. 1. при 30гр, 2 при 40, и так далее до 80 с шагом 10.
	// при температуре 20гр. поправка считается нулевой, так как в этой точке калибруется 0 ток при комнатной температуре.
 // нулевая поправка при значении в массиве=127.

	unsigned char	Kalib;
	unsigned int	UGen;
	unsigned int	IGen;
	unsigned char	DT;
	unsigned char	DM;
	unsigned char	DMT;
	
	unsigned char	TM;
	unsigned char	TBap;
	const unsigned char	TBap85=85;
	
	unsigned char	CtTest[3];
	const unsigned char NumberAdNew[12]={11,11,11,2,3,4,5,6,7,7,7,7};
	unsigned char	MyStatus;
	unsigned char	NumberAd;
	unsigned char	CtAd;
	const unsigned char	 CtAd0=63;
	unsigned int	AdTemp;
 	unsigned int	AdResult[12];

	unsigned char CtKn;
	
	unsigned char CtTDiz;
	unsigned char CtOverLow;
	unsigned long TDiz;
	unsigned int TDizTemp[16];
	
	unsigned int NDiz;
	unsigned int ICR1Old;
	unsigned char ControlTransmit;
	unsigned char ControlReceive;
	
	unsigned char CtErrorLink;

	const unsigned char CtErrorLink0=20;
	volatile unsigned char RegTransmit[17];
	volatile unsigned char RamReceive[6];
	volatile unsigned char RomReceive[6];
	volatile unsigned char CtReceive;
	const unsigned char CtReceive0=2;
	volatile unsigned char CtTransmit;
	const unsigned char CtTransmit0=16;
	unsigned int RegWait;
	
	unsigned char	RegS,RegSTemp,RegSOld;
 
 /*============================================*/

/*   void ReadKn(void)
 {

 unsigned char R0;
 R0=0;
 if(bit_is_set(PINB,PB0))
 R0 |=1;
 if(bit_is_clear(PINB,PB1))
 R0 |=2;
 if(bit_is_clear(PINB,PB2))
 R0 |=4;
 if(bit_is_clear(PINB,PB3))
 R0 |=8;
 if(bit_is_clear(PINB,PB5))
 R0 |=0x10;
 if(bit_is_clear(PINB,PB6))
 R0 |=0x20;
 if(bit_is_clear(PINB,PB7))
 R0 |=0x40;





 if(R0==RegSTemp)
		{
	if(CtKn)
	--CtKn;
	else
 RegS=R0;
		}
	else
	CtKn=5;
 RegSTemp=R0;

 
 }*/

void ReadKn(void)
	{
	
		unsigned char R0;
		R0=~PINB;
		R0 &=0xe;
		if(bit_is_set(PINB,PB0))
			R0 |=0x1;
		if(bit_is_clear(PINB,PB5))
			R0 |=0x10;
		if(bit_is_clear(PINB,PB6))
			R0 |=0x20;
		if(bit_is_clear(PINB,PB7))
			R0 |=0x40;
		if(R0==RegSTemp)
			{
			if(CtKn)
			--CtKn;
			else
				RegS=R0;
			}
		else
		CtKn=5;
		RegSTemp=R0;
	
	
	}

void InitUsart(void)
	{
	
		UCSRA=2;//двойная скорость
		UCSRA |=1;//only 9bit=1 multiproc mode
		UCSRB=0x1c;//11100 enable transmit 9bit
		UCSRC=6;//110   8 bit
		UBRRH=0;
		UBRRL=51;//207;//9600
		
		PORTD &=0xfb;//11111011 Out Transmit=off
		CtErrorLink=CtErrorLink0;
	}

void LinkPult(void) 
	{
	volatile unsigned char R0;
	volatile unsigned char R1;
	volatile unsigned char R2;
	volatile unsigned char R3;
	
	if(UCSRA &(1<<RXC))
		{
		
		
		R1=UCSRA;
		R2=UCSRB;
		R3=UCSRC;
		R0=UDR;
		
		if(R1 & 0x1c)//error
			{
		
			return;
			}
		if(R2 & 2)
			{
			CtReceive=CtReceive0;
			RamReceive[CtReceive]=R0;
			CtTransmit=CtTransmit0;
			ControlReceive=R0;
			ControlTransmit=0;
			--CtReceive;
			UCSRA &=0xfe;
						
			}
		else
		//Receive Data
			{
			if(CtTransmit==CtTransmit0)
				{
				RamReceive[CtReceive]=R0;
				if(!CtReceive)
					{
					if(R0==ControlReceive)
						{
						CtErrorLink=CtErrorLink0;
						for(R1=1;R1<=5;++R1)
						RomReceive[R1]=RamReceive[R1];
						}
				
					--CtTransmit;
				
					PORTD |=0x4;//enable transmit
					while(!(UCSRA & 0x20))
						_WDR();
					UCSRB  &=0xfe;//9bit=0
					UDR=RegTransmit[CtTransmit];
					ControlTransmit=RegTransmit[CtTransmit];
					--CtTransmit;
					}
				else
					{
				//CtReceive !=0
					ControlReceive +=R0;
					--CtReceive;
					}
				}
			else if(CtTransmit)
				{
				 while(!(UCSRA & 0x20))
					_WDR();
				UCSRB  &=0xfe;//9bit=0
				
				if(CtTransmit==1)
					UDR=ControlTransmit;
				else
					{
					UDR=RegTransmit[CtTransmit];
					ControlTransmit +=RegTransmit[CtTransmit];
					}
				--CtTransmit;
				}
			else
				{
				UCSRA |=1;
				PORTD &=0xfb;//denable transmit
				CtTransmit=CtTransmit0;
				}
			}
		}
	} 

void InitAd(void)
	{
	ADMUX=0xcb;//11001011 2.56 V
	CtAd=CtAd0;
	NumberAd=0;
	
	ADCSRA=0;
	ADCSRA |=(1<<ADEN);/*enable AD*/
	ADCSRA |=(1<<ADPS2);
	ADCSRA |=(1<<ADPS1);
	ADCSRA |=(1<<ADPS0); /*128*/	 		
	ADCSRA |=(1<<ADSC);/* Start*/
	}
 
unsigned char ReadEeprom(unsigned char Arg6)
	{
	EEARH=0;
	EEARL=Arg6;
	EECR=EECR | (1<<EERE);
	
	while(bit_is_set(EECR,EERE));
	
	return EEDR;
	}

void AccountUGen(void)
	{
	unsigned int R0;
	unsigned int R1;	  
	UGen=AdResult[7];
	UGen >>=8;
	R0=KoefEeprom[4];
	R1=UGen * R0; //
	R1 >>=7;
	UGen +=R1;
	}
/* void AccountUGen   (void)
 {
	unsigned int R0;
	unsigned int R1;	  
	R0=AdResult[7];
	R0 >>=8;
 R1=KoefEeprom[4];
	R1=R0 * R1;
	R1 >>=7;
	R0 +=R1;
	if(R0>UGen)
		{
	if((R0-UGen)>20)
	UGen=R0;
	else
	++UGen;
		}
	else if(R0<UGen)
		{
	if((UGen-R0)>20)
	UGen=R0;
	else
	--UGen;
		}						

	}*/
/*
 void AccountIGen   (void)
 {
	unsigned long R0;
	R0=AdResult[11]>>4;
	R0=R0*KoefEeprom[5];
	R0=R0>>8;
	if(CtIGen)
		{
	--CtIGen;
	IGenSum +=R0;
		}
	else
		{
	IGen=IGenSum>>3;
	IGenSum=0;
	CtIGen=8;
		}

	}*/
 //Без компенсации температурного дрейфа

 void AccountIGen(void)
	{
	unsigned long R0;
	
	R0=AdResult[11]>>4;
	R0=R0*KoefEeprom[5];
	R0=R0>>8;
	if(CtIGen)
		{
		--CtIGen;
		IGenSum +=R0;
		}
	else
		{
		IGenSumTemp=IGenSum>>3; //вычисление среднего.
		IGen=IGenSumTemp;
		IGenSum=0;
		CtIGen=8;// установка счетчика для умножения на 8 для выборки
		}
	//if (TBap>=21&&TBap<=30)  {IGen=IGenSumTemp+IKoefEeprom[0]-127;}
	if (TBap>=30&&TBap<=40)  {IGen=IGenSumTemp+IKoefEeprom[0]-127;};
	if (TBap>=41&&TBap<=50)  {IGen=IGenSumTemp+IKoefEeprom[1]-127;};
	if (TBap>=51&&TBap<=60)  {IGen=IGenSumTemp+IKoefEeprom[2]-127;};
	if (TBap>=61&&TBap<=70)  {IGen=IGenSumTemp+IKoefEeprom[3]-127;};
	if (TBap>=71&&TBap<=80)  {IGen=IGenSumTemp+IKoefEeprom[4]-127;};
	if (TBap>=81&&TBap<=90)  {IGen=IGenSumTemp+IKoefEeprom[5]-127;};
	//if (TBap>=91&&TBap<=100) {IGen=IGenSumTemp+IKoefEeprom[6]-127;}; 
	if (IGen>500) {IGen=0;}; //Если результат вычисления тока отрицательный.
	}

/*   void AccountIGen   (void)
 {
	unsigned long R0;
	R0=AdResult[11]>>4;
	R0=R0*KoefEeprom[5];
	R0=R0>>8;
	if(R0>IGen)
		{
	if((R0-IGen)>20)
	IGen=R0;
	else
	++IGen;
		}
	else if(R0<IGen)
		{
	if((IGen-R0)>20)
	IGen=R0;
	else
	--IGen;
		}



	if(CtIGen)
		{
	--CtIGen;
	IGenSum +=R0;
		}
	else
		{
	IGen=IGenSum>>3;
	IGenSum=0;
	CtIGen=8;
		}

	}*/
 unsigned char AccountP(unsigned char R2,unsigned char R3)
	{
	unsigned int R0;
	unsigned int R1;
	unsigned char R4;
	R0=AdResult[R2]>>8;
	R1=KoefEeprom[R3];
	R1 *=R0;
	R1 >>=8;
	R0 +=R1;
	if(R0>278)
	R0=0;
	else if(R0>=164)
		{
	R0=278-R0;
	R0 *=90;
	R0 >>=8;
		}
	else	if(R0>=150)
		{
	R0=164-R0;
	R0 *=91;
	R0 >>=8;
	R0 +=40;
		}
	else
		{
	R0=150-R0;
	R0 *=137;
	R0 >>=6;
	R0 +=45;
		}
	
	R4=R0;
	return R4;
	}
	
 void AccountTM(void)
	{
	unsigned long R0;
	//	unsigned long R1;
	R0=AdResult[3];
	R0=R0/91;
	//	R1=110;//KoefEeprom[0];
	//	R1 *=R0;
	//	R1 >>=8;
	//	R0 +=R1;
	//	R0=R0/91;
	R0 +=KoefEeprom[0];
	if(R0>=570)
	R0 -=570;
	else
	R0=0;
	
	TM=R0;
	}
	
void CalcTBap(void)
	{
/* 100gr	0.61k	0bitTab1
	96		0.74	3
	90		0.94 	9
	80		1.25	17
	76		1.45	22
	70		1.80	30
	60		2.52	47		4
	50		4.06	80		20
	46		4.5		88		24
	44		5.07	98		30
	40		5.8		111		36
	35		7.25					4				
	32		8.0	
	16		15.5	233				29
	*/






	unsigned long R0;
	unsigned long R1;
	unsigned int R2;
 static const unsigned  char Tab1[39]={99,98,97,96,95,94,93,92,91,90,//0
								   88,87,86,85,83,82,81,80,79,78,//10
								   77,76,76,75,74,73,73,72,71,70,//20
								   70,69,68,68,67,66,66,65,65};//30

 static const unsigned  char Tab2[39]={ 64,63,62,61,60,59,58,57,56,55,//0
								   55,54,54,53,53,52,52,51,51,50,//10
								   50,49,48,47,46,46,46,45,45,45,//20
								   44,43,42,41,41,40,40,39,39};//30



 static const unsigned  char Tab3[30]={39,38,37,36,35,34,33,32,31,30,//0
								   29,28,27,26,25,24,23,22,21,21,//10
								   20,20,19,19,18,18,17,17,16,16};//20




	R0=AdResult[5];//T
	R1=AdResult[2]>>8;//5.0 B
	R0=R0/R1;//40-200 bit
	R1=156+KoefEeprom[3];
	R0=R0*R1;
	R0=R0>>8;
	if(R0>=16)
	R0-=16;
	else
	R0=0;
	if(R0>300)
	R0=300;
	R2=R0;
	if(R2<39)
		{
	TBap=Tab1[R2];

		}
	else
		{
	R2-=39;
	R2>>=1;
	if(R2<39)
			{
	TBap=Tab2[R2];

			}
	else
			{
	R2-=39;
	R2>>=1;
	if(R2<=29)
				{
	TBap=Tab3[R2];

				}
	else
				{
	TBap=0;
				}




			}
	

		}	
		
	}

void WriteEeprom(unsigned char Arg4,unsigned char Arg5)
	{
	_WDR();
	
	while(bit_is_set(EECR,EEWE));
	while(bit_is_set(SPMCR,SPMEN));
	
	EEARL=Arg4;
	EEARH=0;
	EEDR=Arg5;
	SregTemp=SREG;
	_CLI();
	EECR |=(1<<EEMWE);
	EECR |=(1<<EEWE);
	SREG=SregTemp;				
	while(bit_is_set(EECR,EEWE));
	
	while(bit_is_set(SPMCR,SPMEN));
	}

void ChangeKoef(void)
	{
	unsigned char R0;
	unsigned char R1;
	
	R0=RomReceive[1];
	if(R0<=5)
		{
		if(EnableKoef==55)
			{		
			if((RomReceive[2] & 0x5f)==4)
				{
				if((R0==1)||(R0==2)||(R0==3))
					{
					++KoefEeprom[R0];
					if(KoefEeprom[R0]>KoefMax[R0])
						KoefEeprom[R0]=KoefMax[R0];
					}
				else 
					{	
					if((R0==5) && (NDiz<150))
						R0=7;
					else if((R0==5) && (RomReceive[2] & 0x20))
							R0=6;
					--KoefEeprom[R0];
					if(KoefEeprom[R0]<KoefMin[R0])
						KoefEeprom[R0]=KoefMin[R0];
					}
				}
			else if((RomReceive[2] & 0x5f) ==8)
				{
			if((R0==1)||(R0==2)||(R0==3))
					{
					--KoefEeprom[R0];
					if(KoefEeprom[R0]<KoefMin[R0])
						KoefEeprom[R0]=KoefMin[R0];
					}
			else
					{
					if((R0==5) && (NDiz<150))
						R0=7;
					else 
						if((R0==5) && (RomReceive[2] & 0x20))
							R0=6;
					++KoefEeprom[R0];
					if(KoefEeprom[R0]>KoefMax[R0])
						KoefEeprom[R0]=KoefMax[R0];
					}
				}
			
			R1=ReadEeprom(R0+1);
			if(R1 != KoefEeprom[R0])
			WriteEeprom((R0+1),KoefEeprom[R0]);
			EnableKoef=0;
			}
		}
	}

void LoadKoef(void)
	{
	unsigned char R0;
	unsigned char R1;
	for(R0=0;R0<=7;++R0)
		{
		R1=ReadEeprom(R0+1);
		if((R1>KoefMax[R0])||(R1<KoefMin[R0]))
			R1=Koef0[R0];
			
		KoefEeprom[R0]=R1;
		}
	}

void LoadIKoef(void) //Загрузка таблицы поправок тока от температуры
	{
	unsigned char R0;
	unsigned char R1; 
	for(R0=8;R0<=13;++R0) //Начинается с адреса EEPROM 9 (Адрес начальной ячейки 0х2011
	// 0 ячейка не используется
		{
		R1=ReadEeprom(R0+1); //первые 8 ячеек уже заняты. чтение с 9 по 14 ячейку, 6 значений 
		if(R1==0xFF)  //Запись коэффициента из EEPROM если оно не 0xFF и не 0. 
		R1=IKoef0[R0-8]; 	//иначе - коэффициенты по умолчанию
		IKoefEeprom[R0-8]=R1;
		}
	} 
	
void AccountNDiz(void)
	{
	
	unsigned long R1;
	unsigned char R0;
	
	if(!CtOverLow)
		NDiz=0;
	else
		{
		R1=0;
		for(R0=0;R0<=15;++R0)
			{
			R1 +=TDizTemp[R0];
			}
		R1>>=4;
		if(R1>62330)
			NDiz=0;
		else
			{
			R1=8726400/R1; // было R1=8726400/R1; 108 зубьев на колесе
			NDiz=R1;
			}
		}
	}
 
void ChangeCtError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{	
		if(RegimError & (1<<R0))
			{
			if(CtError[R0])	
				--CtError[R0];
			}
		else 
			if(CtError[R0]<CtError0[R0])
				++CtError[R0];
		}
	}
	
void ChangeCtTime(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{	
		if(RegimTime & (1<<R0))
			{
			if(CtTime[R0])	
				--CtTime[R0];
			}
		else
			CtTime[R0]=CtTime0[R0];
		}					
	}	
	
		

void ResetCtTime(void)
	{	
	unsigned char R0;
	
	for(R0=0;R0<=15;++R0)
		CtTime[R0]=CtTime0[R0];					
	}	
	
		
void SetRegimError(void)
	{
	unsigned int R0;
	
	if(TM<=10)//-20gr
		RegimError |=0x800;
	else 
		if(TM>=12)
		{
			RegimError &=0xf7ff;
			Error &=0xf7ff;
		}
	if((TBap>=TBap85) || (Error & 0x1000))
		RegimError |=0x1000;
	else
		RegimError &=0xefff;
		
	R0=KoefEeprom[7]<<1;
	if(IGen>=R0)
		{
		RegimError |=0x8000;
		RegimError &=0xffdf;
		Error  &=0xffdf;
		}

	else if(IGen>=(R0-10))
		{
		RegimError |=0x20;
		RegimError &=0x7fff;
		}
	else
		{
		RegimError &=0x7fdf;		
		Error &=0xffdf;
		}

	if(NDiz>3300)
		{	
		RegimError |=1;//NDiz>3300
		RegimError &=0xfffd;
		}
	else
		{
		RegimError &=0xfffe;

		if((NDiz<2850) && (NDiz>400))
			RegimError |=2;//NDiz<2850
		else 
			if((NDiz<2850) && (UGen>100) &&(!(RegimWork & 1)))
				RegimError |=2;
		else
			RegimError &=0xfffd;
		}
	

	RegimError &=0xffbf;

	if((NDiz>=2850) && (UGen<250))
		RegimError |=0x4;
	else
		RegimError &=0xfffb;

	if((NDiz>=2850) && (NDiz<=3300))
		{
		if((UGen<250) || (UGen>300))
			RegimError |=8;
		else
			RegimError &=0xfff7;			
		}
	else
		RegimError &=0xfff7;
	
	if(RegS & 4)//TM 120gr
		{
		RegimError |=0x100;
		RegimError &=0xfdff;
		}

	else
		{
		RegimError &=0xfeff;

		if(TM>=140)//TM 1110gr
			{
			RegimError |=0x200;
			}
		else
			{
			RegimError &=0xfdff;
			Error &=0xfdff;
			}
		}

	if(RegS & 8)//DM
		RegimError |=0x400;
	else
		 if((!CtTime[5]) &&(DM<=4))//DM
			RegimError |=0x400;
		else 
			RegimError &=0xfbff;

	if((NDiz>2800)&&(DT<=5))//DT
		RegimError |=0x2000;
	else 
		RegimError &=0xdfff;

	if(RegS & 1)//Vozd Filtr
		RegimError |=0x4000;
	else
		RegimError &=0xbfff;
	}
	
void SetError(void)
	{
	unsigned char R0;
	unsigned int R1;
	unsigned int R2;
	for(R0=0;R0<=15;++R0)
		{	
	if(!CtError[R0])
	Error |=(1<<R0);

		}
	R1=0x1d9f;
	R2=0x1d9f;
//	if(!CtErrorBap)
//	R1 |=0x1000;
 if(RomReceive[2] & 0x80)//Protect=on
		{
 R1=0xc93;
 R2=0xc9f;
		}
	if(Error & R1)
	PORTB &=0xef;
	else
	PORTB |=0x10;
	
	if((!(Error & R2)) && (UGen>=250))
	PORTD &=0xef;
	else
	PORTD |=0x10;			
							
	}	
void ResetError(void)
	{
	unsigned char R0;
	for(R0=0;R0<=15;++R0)
		{
	CtError[R0]=CtError0[R0];
	Error=0;
	RegimError=0;
		}					
	}	

void SetRegimWork(void)
	{
	unsigned int R0;
	unsigned int R1;
	R0=0xffbf;
	R1=0xc9f;
	if(RomReceive[2] & 0x80)//Protect=on
		{
	R0=0xafbf;
	R1=0xc93;
		}
	
	if(NDiz>2500)
	RegimTime |=(1<<5);// for DM
	else
	RegimTime &= 0xffdf;
	
	if(bit_is_set(RegimWork,0))//Stop
	RegimWork=RegimWork & 0xf7;//Rashol=off
	else if(((!(CtTime[4])))&&(TM<=125))
	RegimWork=RegimWork & 0xf7;
	else if(!(CtTime[7])) //10min
	RegimWork=RegimWork & 0xf7;
	else if(NDiz>2000)
	RegimWork |=(1<<3);//Rashol
	
	if(bit_is_set(RegimWork,2))
		{
	RegimTime |=(1<<4);//10s Rashol
	RegimTime |=(1<<7);//10min Rashol
		}
	else
	RegimTime=RegimTime & 0xff6f;
	
	
	
	if(bit_is_set(RegimWork,0))//Stop
	{
	RegimWork=RegimWork & 0xef;//Pusk=off
	if(NDiz<150)
	{
	RegimTime |=(1<<3);//10 sec after N<400
	if(!(CtTime[3]))
	{
	RegimWork=RegimWork & 0xfe;
	RegimTime &=0xfff7;
	}
	}
	else
	RegimTime &=0xfff7;
	}
	
	else
	//Stop=off
	{
	RegimTime &=0xfff7;
	
	
	
	
	if(bit_is_set(RegimWork,1))//Pusk
	{
	
	if(NDiz>400)
	{
	RegimTime |=(1<<2);//2 sec after N>400
	
	}
	else
	RegimTime &=0xfffb;
	if(NDiz>2850)
	RegimWork &=0x3d;//Pusk=off
	
	
	
	}
	
	else
	{
	//no pusk
	RegimTime &=0xfffb;
	if((RomReceive[2] & 1) && (!(Error & 0x100))) //Pusk
	{
	RegimWork=RegimWork & 0xfb;//Temp Stop=off
	RegimWork=RegimWork & 0xf7;//Rashol=off
	
	
	if(!(Error & R0))//TM>-20
					{
	if(NDiz<150)
						{
	
	RegimTime |=0x200;//25sec
	RegimWork |=(1<<5);//TM Pusk
	if(TM>=35)//>5
							{
	RegimTime |=0x400;
	RegimWork |=(1<<4);//TPN Pusk
							}
						}
					}
				}
	if(!(Error & R0))//TM>-20
				{
	if(!CtTime[9])
					{
	RegimTime |=0x400;
	RegimWork |=(1<<4);//TPN Pusk
					}
	if(!(CtTime[10]))
					{
						
	RegimWork |=(1<<1);//Pusk
	RegimWork &=0xcf;//TM Pusk=off
					}
				}
	
	RegimTime &=0xfff7;
	}
	
	
	
	
	if((RomReceive[2] & 2) || (RegS & 2))//Stop
	{
	RegimWork=RegimWork & 0xcd;//Pusk=off
	RegimTime &=0xf0ff;
	if(NDiz>150)
	RegimWork |=(1<<2);//Temp Stop
	}
	else if(Error & R1)
	{
	RegimWork=RegimWork & 0xed;//Pusk=off
	if(NDiz>150)
	RegimWork |=(1<<2);//Temp Stop
	
	}
	
	
	if((bit_is_set(RegimWork,2)) && (NDiz>150))
	{ 
	if(bit_is_clear(RegimWork,3))
	{
	RegimWork |=(1<<0);// Stop
	RegimWork=RegimWork & 0xfb;//Temp Stop=off
	}
	}
	
	}
	if(RegimWork & 0x10)//TempPusk
		{
	if(!(RegimWork & 2))//Pusk=off
			{
	
	if(!CtTime[10])
				{
	RegimWork &=0xef;
	RegimWork |=0x2;
				}
			}
		}
	if((RegimWork & 7) || (NDiz>400))
		{
	RegimWork &=0xef;
	RegimTime &=0xf9ff;
	CtTime[9]=CtTime0[9];
	CtTime[10]=CtTime0[10];
		}
	
	
	}
void ControlStarter(void)
	{
	if(RegimWork & 0x2)//Pusk
		{
		if(!(CtTime[1]))//10 sec
			RegimTime |=(1<<0);//3 sec
			if(bit_is_set(RegimTime,0))
			{
			if(CtTime[0])
			 {
				PORTC &=0xf7;//CT=on
				RegimTime &=0xfffd;//Ct10s=off
				CtTime[1]=CtTime0[1];
			 }
			else
			 {
				RegimTime &=0xfffe;
				PORTC |=8;//CT=off
				if(!CtPusk)
					{
					Error |=(1<<7);//no Pusk
					RegimWork &=0xfd;//Pusk=off
					}
				else
					{
					--CtPusk;
					RegimTime |=2;//10 sec
					if(!(CtTime[1]))
						{
						RegimTime &=0xfffd;
						RegimTime |=1;//3 sec 
						CtTime[0]=CtTime0[0]; 
						}
					else
					CtTime[0]=0;
					}
			 }
			}
		}
	else	
		{
		CtPusk=2;
		RegimTime |=2;//10 sec
		CtTime[1]=0;
		PORTC |=8;//CT=off
		}
	}	
void ControlVozb(void)
	{
	if(NDiz>=400)
		{
	RegimTime |=0x40;
	if(!(CtTime[6]))
	PORTC |=0x10;
	else if(UGen>100)
	PORTC |=0x10;
	else if(CtTime[6])
	PORTC &=0xef;//vozb=on
		}
	else
		{
	RegimTime &=0xffbf;
	PORTC |=0x10;
		}	
	}
void ControlStop(void)
	{
	if((Error & 0x401)&&(NDiz>150))
		{
	PORTC &=0xf9;
	RegimWork |=1;//Stop
	return;
		}
	
	
	if(!(RegimWork & 5))
		{
	CtTime[4]=CtTime0[4];
	CtTime[7]=CtTime0[7];
		}
	if(!(PORTC & 2))//Stop
		{
	if(NDiz>150)
			{
	RegimError |=0x10;
	
			}
	else
			{
	RegimError &=0xffef;
	
			}
	if(!(CtTime[3]))
	PORTC |=6;//AO=off,PO=off
	else	if(Error & 0x10)
	PORTC &=0xfb;
		}
	else
		{
	RegimError &=0xffef;
	if(RegimWork & 0x1)
	PORTC &=0xfd;//STOP
		}   
	}
	
void ControlKG(void)
	{
	unsigned int R0;
	R0=0x9d9f;
	if(RomReceive[2] & 0x80)
	R0=0xc93;
	if(!(PORTC & 1))//KG=on
		{
	Led2 |=0x20;
	Led1 &=0x7f;
		}
	else
		{
	Led2 &=0xdf;
	if(RegLed1 & 0x80)
	Led1 |=0x80;
		}
	if(!(PORTC & 1))//KG=on
	RegimTime |=0x8000;
	if((Error & R0) || ((UGen<240) && (NDiz<2600)) ||(RegS & 0x10))
		{
	Led2 &=0xdf;
	RegLed1 &=0x7f;
	Led1 &=0x7f;
	PORTC |=1;//kg=off
		}
	else if(RegimWork & 0x5)
		{
	
	RegLed1 &=0x7f;
		}
	
	
	else if((NDiz>=2850) && ((UGen>=250)||(RomReceive[2] & 0x80)))
		{
		 
	if((TM>=70) || (RomReceive[2] & 0x80))//40gr
	
			{
	if((PORTC & 2) && (PORTC & 4) && (!(RegS & 0x10)))
				{
	if((!(RomReceive[2] & 0x20)) && (CtTime[15]))//Avt Upr
					{
	PORTC &=0xfe;//KG=on
	RegLed1 |=0x80;
					}
	else
	RegLed1 |=0x80;
				}
			}
	
		}
	
	
	
	if(RegimWork & 0x5)
	PORTC |=1;//KG=off
	else if(Error & R0)
	PORTC |=1;//KG=off
	else if((RomReceive[2] & 8) || (RegS & 0x40))
	PORTC |=1;//KG=off
	else if((Led1 & 0x80)&&((RomReceive[2] & 4) || (RegS & 0x20)))
	PORTC &=0xfe;//KG=on		   
	}
	
void ControlLed(void)
	{

	if(Error & 0x8000)//I>290
	Led2 |=0x10;
	else
	Led2 &=0xef;


 if(Error & 0x900)
		{
	Led1 |=1;

		}
	else if(Error & 0x200)//TM>105
		{


	Led1 &=0xfe;
		}
	else
		{



	Led1 &=0xfe;
		}
	if(Error & 0x400)//DM
	Led1 |=2;
	else
	Led1 &=0xfd;

	if(Error & 0x2000)//DT
	Led1 |=4;
	else
	Led1 &=0xfb;

	if(Error & 3)//NDiz
	Led1 |=8;
	else
	Led1 &=0xf7;

	if(Error & 0xc)//UGen
	Led2 |=8;
	else
	Led2 &=0xf7;

	if(TBap>=TBap85)
	Led1 |=0x10;
	else
	Led1 &=0xef;

	if(Error & 0x4000)//Vozd Filtr
	Led1 |=0x20;
	else
	Led1 &=0xdf;

	if(RegimWork & 5)
		{
	Led2 |=0x2;
	Led2 &=0xfe;
		}
	else
		{
	Led2 &=0xfd;
	if(RegimWork & 0x32)
	Led2 |=1;
	else
	Led2 &=0xfe;
		}

	if(Led1 & 0x80)//PNR
	PORTD &=0xdf;
	else
	PORTD |=0x20;

	if(RegimWork & 0x10)
	PORTC &=0x5f;//TPN,PV
	else if(RegimWork & 0x20)//PV
		{
	PORTC &=0xdf;
	PORTC |=0x80;
		}
	else
	PORTC |=0xa0;




	}
void ControlVent(void)
	{
	if((NDiz<400) ||(TM<=90))
	PORTC |=0x40;
	else if(TM>=95)//65gr
	PORTC &=0xbf;

	}

void ChangeCtKalib(void)
	{
	if(Kalib==55)
		{
	if(RomReceive[2] & 0x4c)
	CtKalib=CtKalib0;
		}
	else
	CtKalib=CtKalib0;
	if(CtKalib)
	--CtKalib;
	if(!CtKalib)
	Kalib=0;
	}							

void ControlZvonok(void)
	{

	if(!(RegimWork & 0x10))
	Error &=0xffbf;
	else if((CtTime[9]==0)||(CtTime[10]==0))
	Error &=0xffbf;
	else if((CtTime[9]<45)||(CtTime[10]<45))
	Error |=0x40;
	else
	Error &=0xffbf;

	}
/*++++++++++++++++++++++++++++++++++++++++++*/


int main(void)
{
	DDRB=0b10;
	PORTB |= 0xff;// Control UAB
	DDRC=0xff;
	PORTC |=0xff;
	DDRD=0b10111110;
	PORTD=0b11111011;//transmit=off
	DDRA=0;
	PORTA=0;
	_WDR();
	InitAd();
	ADCSRA |=(1<<ADIE);//enable interrupt AD
	
	SPCR=0b1001000;//f/64
	SPSR=0;//f/64
		
	/*TIMER1*/
	TIMSK=TIMSK | 0b10100;//enable Int capture1
	TCCR1B=0b11000001;//0.0625mkc
	TIMSK=TIMSK | 0b100;//enable Int overlowT1
	
	/*TIMER2*/	
	TCCR2 =1;//clkT2S/(No prescaling)
	TCNT2=5;//4ms
	
	/*
	Bit 5 – TICIE1: Timer/Counter1, Input Capture Interrupt Enable
	Bit 3 – OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
	*/
	TIFR |=0b101000;
	
	CtKn=5;
	
	LoadKoef();
	LoadIKoef(); //загрузка поправок для тока из Eeprom
	ReadKn();
	RegWait=300;
	
	while(RegWait--)
	_WDR();
	
	ReadKn();
	InitUsart();
	UCSRB |=0b1010000;//enable int receive
	RegSOld=RegS;	
	
	_SEI();
	Kalib=44;		
	RomReceive[1]=0xff;
	ResetError();	
	
	RegimTime=0;
	CtIntT1=25;
	RegimWork=0;
	ResetCtTime();
	
	RomReceive[2]=0;
	CtTDiz=15;
						 
	/*Work program*/  	 
	while(1)
	{
		_WDR();
		if(NDiz<400)
		{
			CtTime[15]=CtTime0[15];
			RegimTime &=0x7fff;
		}
		DeltaI=KoefEeprom[6];
		
		if(RegS & 0x10)
		{
		if((RomReceive[1]==1)||(RomReceive[1]==2))
			{
			if((RomReceive[2] & 0x50)==0x50)
				Kalib=55;
			}
		}
		
		if(Kalib==55)
			ChangeKoef();
		
		SetRegimError();
		SetError();
		ControlLed();
		ControlZvonok();
		
		if(!(RegS & 0x10))//Dist upr
		{
			SetRegimWork();
			ControlStarter();
			ControlVozb();
			ControlKG();
			ControlStop();
			ControlVent();
			Led2 &=0xfb;
		}
		else
		{
		if(Kalib==55)
			Error|=0x40;
		else
			Error &=0xffbf;
		//	ResetError();	
		
		///	RegimTime=0;
		RegimWork=0;
		Led1 &=0x7f;
		Led2 |=4;
		Led2 &=0xdf;
		//	Led1Mig &=0x70;
		PORTB |=0x10;
		PORTC =0xff;
		PORTD |=0xb0;
		}
		
		if(RomReceive[2] & 0x10)
			ResetError();
		//	else if(RegS & 0x10)
		//	ResetError();
		if(MyStatus)
		{
			AccountNDiz();
			AccountUGen();
			if(UGen>=200)
				AccountIGen();
			else
				IGen=0;
			
			DMT=AccountP(6,2);
			
			if(DMT>DT)
				++DT;
			else if(DMT<DT)
				--DT;
				
			DMT=AccountP(4,1);
			if(DMT>DM)
				++DM;
			else if(DMT<DM)
				--DM;
			
			AccountTM();
			//	AccountTBap();	
			CalcTBap();
			MyStatus=0;
		}
		
		RegTransmit[15]=Led2;
		RegTransmit[14]=Led1;
		RegTransmit[13]=TBap;
		RegTransmit[12]=NDiz>>8;
		RegTransmit[11]=NDiz;
		RegTransmit[10]=TM;
		RegTransmit[9]=DM;
		RegTransmit[8]=DT;
		
		if(NDiz<150)
		{
			RabReg0=KoefEeprom[7]<<1;
			RegTransmit[7]=RabReg0>>8;
			RegTransmit[6]=RabReg0;
		}
		else
		{
			RegTransmit[7]=IGen>>8;
			RegTransmit[6]=IGen;
		}
		
		RegTransmit[5]=UGen>>8;
		RegTransmit[4]=UGen;
		RabReg0=Error>>8;//
		
		if(TBap>=TBap85)
			RabReg0 |=0x10;
		
		RegTransmit[3]=RabReg0;
		RegTransmit[2]=Error;
		//	RegTransmit[1]=Error;
		//	RegTransmit[0]=20;
		ReadKn();

	}
}

/* Timer/Counter1 Capture Event */
 SIGNAL(SIG_INPUT_CAPTURE1)
{

	if((ICR1>=ICR1Old) && (CtOverLow==1))
		TDizTemp[CtTDiz]=0xffff;
	else if(!CtOverLow)
			TDizTemp[CtTDiz]=0xffff;
	else
		{
		TDizTemp[CtTDiz]=ICR1-ICR1Old;
		if(TDizTemp[CtTDiz]<100)
			TDizTemp[CtTDiz]=0xffff;
		ICR1Old=ICR1;
		}
		
	if(CtTDiz)//число измерений 15
		--CtTDiz;
	else
		CtTDiz=15;
		
 CtOverLow=2;
}

/* Timer/Counter1 Overflow */
 SIGNAL(SIG_OVERFLOW1)/*4 Mc*/
{
	if(CtIntT1)
	--CtIntT1;
	else
		{
		ChangeCtError();
		ChangeCtTime();
		ChangeCtKalib();
		CtIntT1=25;
		EnableKoef=55;
		}
		
	if(CtOverLow)
		--CtOverLow;
	else
		NDiz=0;
	
	if(CtErrorLink)
	--CtErrorLink;
	else
		{
		InitUsart();
		UCSRB |=0x80;//enable int receive
		}
}
/* USART, RX Complete */
SIGNAL(SIG_UART_RECV)
{
	LinkPult();
}
/* ADC Conversion Complete */
SIGNAL(SIG_ADC)
{
 unsigned int RegInt0;
// unsigned int RegInt1;
 RegInt0=ADC;
	
	if(CtAd)
	{
	--CtAd;
	if(CtAd<=CtAd0)
		{
	//	if(RegInt0>AdTemp)
		if(NumberAd==11)
			{
			if(RegInt0>300)
				RegInt0=0;
			else if(RegInt0 >DeltaI)
				RegInt0=RegInt0-DeltaI;
			else 
				RegInt0=0;
			}
	//	RegInt1=AdTemp+RegInt0;
	//	if(RegInt1>=RegInt0)
		AdTemp +=RegInt0;
		}
	}
	else
	 {
	 AdResult[NumberAd]=AdTemp;
	 AdTemp=0;
	 CtAd=CtAd0+2;
	 if(NumberAd==11)
	 	MyStatus=MyStatus | 1; //сформировать отчет
	
	 NumberAd=NumberAdNew[NumberAd];
	 }

	RegInt0=ADMUX;
	RegInt0 &=0xe0;
	RegInt0 |=NumberAd;
	ADMUX=RegInt0;

	 ADCSRA |=(1<<ADSC);
}
