
/*
Конфигурирование контроллера Ethernet и функции для обмена данными по SPI.
*/
/*
Расположение байт в буфере следующее:
в буфер записываются 16-битные слова, в каждом слове переставлены местами байты, то есть:
1, 0, 3, 2, 5, 4,... 1,0 - первое слово в буфере, 3,2 - второе и так далее (Little Endian представление данных).

Именно из-за этого если пакет содержит нечетное кол-во байт, то в буфер необходимо записывать на
один байт больше, а в ответном пакете на ICMP-запрос отправлять столько байт, сколько было в запросе.
Это расположение байт в слове касается только буферов приемника и передатчика. Управляющие регистры
имеют расположение байтов и битов то, которое заявлено в документации.

Например, если у нас IP-адрес 192.168.1.87 (C0.A8.01.57), то он будет записан в буфере при приеме пакета: A8.C0.57.01
*/
/*
По фронту CLK меняются данные, на срезе CLK данные считываются!
Протокол обмена данными по SPI в реализованной программе:

Чтение данных:
Запрос:
<CMD[8]>_<ADDR[15:8]>_<ADDR[7:0]>_<FIL[8]>...<FIL[8]>	Пока удерживается сигнал SFS и записываются поля FILL, то происходит считывание данных
Ответ:
<ERR[8]>_<ERR[8]>_<ERR[8]>_<ERR[8]>_<DATA[8]>...<DATA[8]>

Запсь данных:
<CMD[8]>_<ADDR[15:8]>_<ADDR[7:0]>_<DATA[8]>...<DATA[8]>	Пока удерживается сигнал SFS, данные записываются
Ответ:
<ERR[8]>_<ERR[8]>_<ERR[8]>_<ERR[8]>_..._<ERR[8]>


<CMD[8]> 0xFF - сброс устройства
         0x18 - выбор последовательного порта для работы
         0x24 - выбор параллельного порта для работы
         0x3С - запись блока данных
         0x42 - чтение блока данных

Все данные передаются старшим битом вперёд, старший байт - первый.

<ADDR[16]> адрес, начиная с которого мы хотим считать/записывать массив данных;
необходимо при чтении и записи в буферы приемника и передатчика следить
за граничными адресами (0x07FF - для приемника, 0x17FF - для передатчика),
так как буфер циклический

<FIL[8]> 0x00 заполнитель

<ERR[8]> код ошибки: 0x00 нет ошибки
                     0x01 неверный/неизвестный код команды
<DATA[8]> данные
*/

unsigned int Send_CMD_SPISEL(void);                 //функция выбора последовательного порта
int Reset_Eth(void);                                //функция сброса контроллера 5600ВГ1У
void EthCfg(void);                                  //функция конфигурирования контроллера 5600ВГ1У

int Write_Word(unsigned short, unsigned short); 	//функция записи слова по определенному адресу
unsigned short Read_Word(unsigned short);       	//функция чтения слова по определенному адресу

int WriteTxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для записи массива данных в буфер передатчика
int ReadTxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных из буфера передатчика

int WriteRxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для записи массива данных в буфер приемника
int ReadRxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных из буфера приемника

int Read_Array_Of_Data(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных
int Write_Array_Of_Data(unsigned char*, unsigned short, unsigned short);//функция для записи массива данных

int Write_Tx_Descriptor(unsigned short,_tx_current_descriptor*);        //функция записи дескриптора передатчика
unsigned short Read_Tx_Descriptor(_tx_current_descriptor*);             //функция чтения дескриптора передатчика

int Ready_Rx_Descriptor(__descriptor*);                      //функция записи дескриптора приемника
unsigned short Read_Rx_Descriptor(__descriptor*);            //функция чтения дескриптора приемника

unsigned short Read_Packet_Start_Address(__descriptor*);     //функция чтения стартового адреса принятого пакета
unsigned short Read_Packet_Length(__descriptor*);            //функция чтения длины пакета

unsigned char Get_Char(void);  //функция возвращает байт данных из SPI буфера

//************************************************************************************************************************
//************************************************************************************************************************
//************************************************************************************************************************

void EthCfg()
{
	Reset_Eth();

	Send_CMD_SPISEL();
				
	Write_Word(0x4382,0x1FDF);    //GCTRL = 0x4382;
	Write_Word(0x0200,0x1FC0);    //MAC_CTRL = 0x0200;
	Write_Word(0x0040,0x1FC1);    //MinFrame = 0x0040;
	Write_Word(0x0000,0x1FC3);    //CollConfig = 0;
	Write_Word(0x000A,0x1FC4);    //IPGTx = 0x000A;
//--- MAC-address: 0A.1B.2C.3D.4E.5F ---
// 	Write_Word(0x1B0A,0x1FC5);    //MAC_ADDR_T = 0x0A1B;
// 	Write_Word(0x3D2C,0x1FC6);    //MAC_ADDR_M = 0x2C3D;
// 	Write_Word(0x5F4E,0x1FC7);    //MAC_ADDR_H = 0x4E5F;
	Write_Word((uint16_t)(mac[1]<<8)|mac[0],0x1FC5);    //MAC_ADDR_T = 0x0A1B;
	Write_Word((uint16_t)(mac[3]<<8)|mac[2],0x1FC6);    //MAC_ADDR_M = 0x2C3D;
	Write_Word((uint16_t)(mac[5]<<8)|mac[4],0x1FC7);    //MAC_ADDR_H = 0x4E5F;
	Write_Word(0x31D0,0x1FCE);    //PHY_CTRL = 0x31D0;
}
//-----------------------------------------
//--- Function for send CMD to 5600VG1U ---
//-----------------------------------------
//Function send command 5600VG1U for select SPI to exchenge data
//Param.	-
//Return	0 - no error
//			1 - command not send
unsigned int Send_CMD_SPISEL()
{
	if((MDR_SSP1->SR&0x0002)==0x0002)
	{
		MDR_PORTB->RXTX = 0x0800;
		MDR_SSP1->DR = 0x18;
		while((MDR_SSP1->SR&0x0010)==0x0010){}      //wait until all transfer over
		MDR_PORTB->RXTX = 0x1800;
		while((MDR_SSP1->SR&0x0004)==0x0004) Get_Char();
		return 0;
	}
	return 1;	//return 1, if Command Select SPI execute with error
}

//-----------------------------------
//--- Function for Reset 5600VG1U ---
//-----------------------------------
int Reset_Eth()
{
	unsigned int a;
	MDR_PORTB->RXTX = 0x1000;
	for(a=0;a<1000;a++){}
	MDR_PORTB->RXTX = 0x1800;
	return 0;
}
//----------------------------------------------
//--- Function for Read Word From SPI Buffer ---
//----------------------------------------------
unsigned char Get_Char()
{
        return (unsigned char)MDR_SSP1->DR;
}
//------------------------------------------------
//--- Function for Read One Word from 5600VG1U ---
//------------------------------------------------
//Function for read word from 5600VG1U
//Param.	unsigned short Addr - Address of read word
//Return	value of read word
unsigned short Read_Word(unsigned short Addr)   //функция для режима работы SPI с конфигурацией размера слова: 8 бит
{
        unsigned char CounterS, CounterR;
        unsigned char SendArray[6]; 
        while((MDR_SSP1->SR & 4) == 4)    //Если буфер приемника не пуст, вычитываем оттуда все содержимое
        {
			Get_Char();
        }
//CounterS - счетчик количества байт, которые уже отправлены по SPI
//CounterR - счетчик количества байт, которые уже приняты по SPI

        CounterR = 0;
        CounterS = 0;

        SendArray[0]=0x42;                              //read cmd
        SendArray[1]=(unsigned char)(Addr >> 8);        //address
        SendArray[2]=(unsigned char)Addr;               //address
        SendArray[3]=0x00;                              //filler
        SendArray[4]=0x00;                              //filler
		SendArray[5]=0x00;

        MDR_PORTB->RXTX = 0x0800;  //SFS=0;
        while ((CounterS < 6) || (CounterR < 6))
        {
			if((MDR_SSP1->SR & 2) == 2)    //if FIFO Tx not full
			{
				MDR_SSP1->DR = SendArray[CounterS];
				CounterS++;
			}
			if((MDR_SSP1->SR & 4) == 4)         //if FIFO Rx not empty
			{
				if(CounterR < 4)
				{
					Get_Char();
				}
				else
				{
					SendArray[CounterR - 4] = Get_Char();
				}
				CounterR++;
			}
        }
		while((MDR_SSP1->SR&0x0010)==0x0010){}
        MDR_PORTB->RXTX = 0x1800;  //SFS=1;
        return ((SendArray[0]<<8)&0xFF00)|SendArray[1];
}

//-------------------------------------------
//--- Function For Write Word in 5600VG1U ---
//-------------------------------------------
//Value - записываемое значение
//Address - адрес, по которому будет записано слово
int Write_Word(unsigned short Value, unsigned short Address)     //функция для режима работы SPI с конфигурацией размера слова: 8 бит
{
        unsigned char Counter;
        unsigned char SendArray[5];
        while((MDR_SSP1->SR & 4) == 4)    //Если буфер приемника не пуст, вычитываем оттуда все содержимое
        {
			Get_Char();
        }

        SendArray[0] = 0x3C;
        SendArray[1] = (unsigned char)(Address >> 8);
        SendArray[2] = (unsigned char)Address;
        SendArray[3] = (unsigned char)(Value >> 8);
        SendArray[4] = (unsigned char)Value;

        Counter = 0;
        MDR_PORTB->RXTX = 0x0800;  //SFS=0;
        while(Counter < 5)
        {
            if((MDR_SSP1->SR & 0x0002) == 0x0002)    //if FIFO Tx not full
            {
                MDR_SSP1->DR = SendArray[Counter];
            	Counter++;
        	}
			if((MDR_SSP1->SR & 0x0004) == 0x0004)	Get_Char();	//if FIFO Rx not empty
        }
        while((MDR_SSP1->SR&0x0010)==0x0010){}      //wait until all transfer over
        MDR_PORTB->RXTX = 0x1800;
        while((MDR_SSP1->SR & 4) == 4)         //while FIFO Rx not empty
        {
			Get_Char();
        }
        return 0;
}
//--------------------------------------------------
//--- Function for Write data to memory 5600VG1U ---
//--------------------------------------------------
//Функция для записи массива данных
//Data - указатель на массив данных для записи
//Length - количество записываемых байт в буфер
//Address - стартовый адрес для записи массива
//Возвращает количество записанных байт
int Write_Array_Of_Data(unsigned char* Data, unsigned short Length, unsigned short Address)
{
	unsigned short Counter;
	unsigned char SendArray[3],i = 0;
//Counter - счетчик отправленных байт (функция возвращает значение этой переменной)
//Temp - переменная для временного хранения данных
		
        //--- SPI FIFO buffer has 8 bytes size ---
        while((MDR_SSP1->SR & 4) == 4)    //Если буфер приемника не пуст, вычитываем оттуда все содержимое
        {
			Get_Char();
        }
        SendArray[0] = 0x3C;
        SendArray[1] = (unsigned char)(Address >> 8);
        SendArray[2] = (unsigned char)Address;

        Counter = 0;
        MDR_PORTB->RXTX = 0x0800;
        while(Counter < Length+3)
        {
			if((MDR_SSP1->SR & 0x0002) == 0x0002)    //if FIFO Tx not full
			{
				if(Counter < 3)
				{
					MDR_SSP1->DR = SendArray[Counter];
				}
				else
				{
					//MDR_SSP1->DR = Data[Counter - 3];
					if(!i)
					{
						MDR_SSP1->DR = Data[Counter - 3 + 1];
						++i;
					}
					else
					{
						i=0;
						MDR_SSP1->DR = Data[Counter - 4];
					}
					
				}	
				Counter++;
			}
			if((MDR_SSP1->SR & 4) == 4)     //if FIFO Rx not empty
			{
				Get_Char();
			}
        }

        while((MDR_SSP1->SR&0x0010)==0x0010){}      //wait until all transfer over
        MDR_PORTB->RXTX = 0x1800;
        while((MDR_SSP1->SR & 4) == 4) //while FIFO Rx not empty
        {
			Get_Char();
        }
        return Counter - 3; //function return number of sent bytes
}
//-----------------------------------------------------
//--- Function For Write Data To Tx Buffer 5600VG1U ---
//-----------------------------------------------------
//Функция позволяет записывать данные в буфер передатчика
//Data - указатель на массив, из которого будут записываться данные
//Length - кол-во байт, необходимое записать
//Address - адрес в буфере, с которого необходимо записать данные
//Возможные адреса 0x1000 - 0x17FF
//Возвращает количество записанных данных
int WriteTxBufferData(unsigned char* Data, unsigned short Length, unsigned short Address)
{
//необходимо отслеживать, что адрес записи не превышает 0х17FF
	unsigned short CurrentLength;
	int Cnt,CurrentCnt;
	if(Address>0x17FF||Address<0x1000) return 0;	//неверное значение адреса, возвращаем 0
	if((Address+Length/2)>0x17FF)	//адрес измеряется в словах (16 бит), а данные в байтах (8 бит)!!!
	{
		CurrentLength = (0x1800 - Address)*2;	//сколько байт влезет в буфер до заполнения ((0x1800 - Address) - сколько слов влезет в буфер до заполнения).
		CurrentCnt = Write_Array_Of_Data(Data,CurrentLength,Address);
		Cnt = Write_Array_Of_Data(&Data[CurrentCnt],Length-CurrentLength,0x1000);
		Cnt += CurrentCnt;
	}
	else
	{
		Cnt = Write_Array_Of_Data(Data,Length,Address);
	}
	return Cnt;
}
//------------------------------------------------------
//--- Function For Read Data From Tx Buffer 5600VG1U ---
//------------------------------------------------------
//Функция позволяет считывать данные из буфера передатчика
//Data - указатель на массив, куда будут считаны данные
//Length - кол-во байт, необходимое считать
//Address - адрес в буфере, с которого необходимо прочитать данные
//Возможные адреса 0x1000 - 0x17FF
//Возвращает количество прочитанных байт
int ReadTxBufferData(unsigned char* Data, unsigned short Length, unsigned short Address)
{
	unsigned short CurrentLength;
	int Cnt,CurrentCnt;
	if(Address>0x17FF||Address<0x1000) return 0;	//неверное значение адреса, возвращаем 0
	if((Address+Length/2)>0x17FF)
	{
		CurrentLength = (0x1800 - Address)*2;	//сколько байт влезет в буфер до заполнения.
		CurrentCnt = Read_Array_Of_Data(Data,CurrentLength,Address);
		Cnt = Read_Array_Of_Data(&Data[CurrentCnt],Length-CurrentLength,0x1000);
		Cnt += CurrentCnt;
	}
	else
	{
		Cnt = Read_Array_Of_Data(Data,Length,Address);
	}
	return Cnt;
}
//--------------------------------------------
//--- Function For Read Data From 5600VG1U ---
//--------------------------------------------
//Функция для чтения массива данных
//Data - указатель на массив данных, куда будет производиться чтение
//Length - количество байт, которое необходимо считать из буфера
//Address - стартовый адрес для чтения массива
//Возвращает количество прочитанных байт
int Read_Array_Of_Data(unsigned char* Data, unsigned short Length, unsigned short Address)
{
        unsigned short CounterSendBytes, CounterReadBytes;
        unsigned char SendArray[3], i = 0, tmp8 = 0;
	//	unsigned int 
        //--- SPI FIFO buffer has 8 bytes size
        while((MDR_SSP1->SR & 4) == 4)    //Если буфер приемника не пуст, вычитываем оттуда все содержимое
        {
			Get_Char();
        }

        SendArray[0] = 0x42;
        SendArray[1] = (unsigned char)(Address >> 8);
        SendArray[2] = (unsigned char)Address;

        CounterSendBytes = 0;
        CounterReadBytes = 0;
        MDR_PORTB->RXTX = 0x0800;
        while ((CounterSendBytes < Length + 4) || (CounterReadBytes < Length + 4))
        {
			if((MDR_SSP1->SR & 0x0002) == 0x0002)    //if FIFO Tx not full
			{
				if(CounterSendBytes < 3)
				{
					MDR_SSP1->DR = SendArray[CounterSendBytes];
				}
				else
				{
					MDR_SSP1->DR = 0x00;
				}
				CounterSendBytes++;
			}
			if((MDR_SSP1->SR & 0x0004) == 0x0004)    //if FIFO Rx not empty
			{
				if(CounterReadBytes < 4) //if CounterReadBytes < 9 we receive <ERR> character
				{
					Get_Char();
				}
				else
				{
					//Data[CounterReadBytes-4] = Get_Char();
					
					if(!i)
					{
						tmp8 = Get_Char();
						++i;
					}
					else
					{
						i=0;
						Data[(CounterReadBytes-5)] = Get_Char();
						Data[(CounterReadBytes-5)+1] = tmp8;
					}
				}
				CounterReadBytes++;
			}
        }
        while((MDR_SSP1->SR&0x0010)==0x0010){}      //wait until all transfer over
        MDR_PORTB->RXTX = 0x1800;
        while((MDR_SSP1->SR & 4) == 4)         //while FIFO Rx not empty
        {
			Get_Char();
        }
        return CounterReadBytes-4; //function return number of read bytes
}
//------------------------------------------------------
//--- Function For Read Data From Rx Buffer 5600VG1U ---
//------------------------------------------------------
//Функция позволяет считать данные из буфера приемника
//Data - указатель на массив, куда будут считаны данные
//Length - кол-во байт, необходимое считать
//Address - адрес в буфере, с которого необходимо прочитать данные
//Возможные адреса 0x0000 - 0x07FF
//Возвращает количество считанных байт
int ReadRxBufferData(unsigned char* Data, unsigned short Length, unsigned short Address)
{
	unsigned short CurrentLength;
	int	Cnt,CurrentCnt;
	if(Address>0x07FF) return 0;
	Cnt = 0;
	if((Address+Length/2)>0x07FF)
	{
		CurrentLength = (0x0800 - Address)*2;  //сколько байт влезет в буфер до заполнения ((0x0800 - Address) - сколько слов влезет в буфер до заполнения).
		CurrentCnt = Read_Array_Of_Data(Data,CurrentLength,Address);
		Cnt = Read_Array_Of_Data(&Data[CurrentCnt],Length-CurrentLength,0x0000);
		Cnt += CurrentCnt;
	}
	else
	{
		Cnt = Read_Array_Of_Data(Data,Length,Address);
	}
	return Cnt;
}
//-----------------------------------------------------
//--- Function For Write Data To Rx Buffer 5600VG1U ---
//-----------------------------------------------------
//Функция позволяет считать данные из буфера приемника
//Data - указатель на массив, откуда будут записываться данные
//Length - кол-во байт, необходимое записать
//Address - адрес в буфере, с которого необходимо записывать данные
//Возможные адреса 0x0000 - 0x07FF
//Возвращает количество записанных данных
int WriteRxBufferData(unsigned char* Data, unsigned short Length, unsigned short Address)
{
	unsigned short CurrentLength;
	int	Cnt,CurrentCnt;
	if(Address>0x07FF) return 0;
	Cnt = 0;
	if((Address+Length/2)>0x07FF)
	{
		CurrentLength = (0x0800 - Address)*2;
		CurrentCnt = Write_Array_Of_Data(Data,CurrentLength,Address);
		Cnt = Write_Array_Of_Data(&Data[CurrentCnt],Length-CurrentLength,0x0000);
		Cnt += CurrentCnt;
	}
	else
	{
		Cnt = Write_Array_Of_Data(Data,Length,Address);
	}
	return Cnt;
}

//------------------------------------------------
//--- Function For Write Data In Tx Descriptor ---
//------------------------------------------------
int Write_Tx_Descriptor(unsigned short PacketLen,_tx_current_descriptor* Desc)
{
        Write_Word(PacketLen,Desc->TxCurrentDescriptor->StartAddress+1);
        Write_Word(Desc->FirstEmptyByte,Desc->TxCurrentDescriptor->StartAddress+3);

        if(Desc->TxCurrentDescriptor->LastDesc == 1) Write_Word(0xC000,Desc->TxCurrentDescriptor->StartAddress);
		else Write_Word(0x8000,Desc->TxCurrentDescriptor->StartAddress);

		Desc->FirstEmptyByte = ((Desc->FirstEmptyByte+(PacketLen/2))&0x07FF)|0x1000;
        return 0;
}

//--------------------------------------------------
//--- Function For Read Result For Transfer Data ---
//--------------------------------------------------
unsigned short Read_Tx_Descriptor(_tx_current_descriptor* Desc)
{
        if((Read_Word(Desc->TxCurrentDescriptor->StartAddress) & 0x8000) == 0x8000) return 1;   //пакет не отправлен
        else return 0;                                                  						//пакет отправлен
}

//--------------------------------------------------------
//--- Function For Write Ready Status in Rx Descriptor ---
//--------------------------------------------------------
int Ready_Rx_Descriptor(__descriptor* Desc)
{
		if(Desc->LastDesc == 1) Write_Word(0xC000,Desc->StartAddress);	//установка готовности дескриптора к приему пакета
		else Write_Word(0x8000,Desc->StartAddress); 					//установка готовности дескриптора к приему пакета
        return 0;
}

//----------------------------------------------------------
//--- Function For Read Packet Status From Rx Descriptor ---
//----------------------------------------------------------
unsigned short Read_Rx_Descriptor(__descriptor* Desc)
{
        if((Read_Word(Desc->StartAddress) & 0x8000) == 0x8000) return 1;        //буфер готов к приему пакета
        else return 0;                                                  		//пакет принят
}
//--------------------------------------------------------------
//--- Function For Read Descriptor Status From Rx Descriptor ---
//--------------------------------------------------------------
unsigned short Read_Rx_Dsc_All(__descriptor* Desc)
{
		return Read_Word(Desc->StartAddress);	
}
//----------------------------------------------------------
//--- Function For Read Packet Length From Rx Descriptor ---
//----------------------------------------------------------
unsigned short Read_Packet_Length(__descriptor* Desc)
{
        return Read_Word(Desc->StartAddress+1);
}

//-----------------------------------------------------------------
//--- Function For Read Packet Start Address From Rx Descriptor ---
//-----------------------------------------------------------------
unsigned short Read_Packet_Start_Address(__descriptor* Desc)
{
        return Read_Word(Desc->StartAddress+3);
}

