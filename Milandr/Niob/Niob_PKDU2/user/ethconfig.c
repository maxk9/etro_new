
/*
Конфигурирование контроллера Ethernet и функции для обмена данными по параллельному интерфейсу.
*/


#include "MDR32Fx.h"
#include "ethconfig.h"

/*
MAC адрес
3E-2C-49-74-9B-52*
F1-18-C8-98-D8-45
D9-9A-88-4B-43-78
54-69-B3-D9-B2-24
74-9A-89-2C-B4-03
87-F3-30-D1-67-CB
24-59-E4-ED-F1-BC
33-CA-57-BC-16-9A
34-6B-03-E7-44-B6
0C-B8-51-95-E5-05
99-6D-F8-C9-3F-60
94-64-B9-78-51-AB
*/

const unsigned char MAC_adress[6] = {0x52, 0x9B, 0x74, 0x49, 0x2C, 0x3E};//


//--------------------------------------------------------------------------------------
//Функция для настройки внешней системной шины
//Параметр:	нет
//Возвращаемых знаений нет
//--------------------------------------------------------------------------------------
void ExtBusCfg()
{
//	EXT_BUS_CNTRL->EXT_BUS_CONTROL = 0x2002;  //wait state=0 (3 HCLK), RAM mode
}

//--------------------------------------------------------------------------------------
//Функция для настройки портов контроллера 1986ВЕ91Т1
//Параметр:	нет
//Возвращаемых знаений нет
//-------------------------------------------------------------------------------------- 
void PortConfig()
{
//Data Bus[0:15]
// 		MDR_PORTA->ANALOG = 0xFFFF;
// 		MDR_PORTA->PWR = 0xAAAAAAAA;
// 		MDR_PORTA->FUNC = 0x55555555;

// //OE-WE & Buttons
// 		MDR_PORTC->ANALOG = 0xFFFF; // OE-WE
// 		MDR_PORTC->PWR = 0xAAAAAAAA; // OE-WE
// 		MDR_PORTC->FUNC = 0x00000014; // OE-WE
// //		PORTC->PD   = 0x00000000; // OE-WE//Светодиоды
// //		PORTC->RXTX = 0x0000;
// //		PORTC->OE = 0x0080;
// //LED

//         PORTD->RXTX = 0;
//         PORTD->OE = 0x7C00; //PORTD10 - PORTD14 - outputs
//         PORTD->FUNC = 0x00000000; //PORTD - ports
//         PORTD->ANALOG = 0xFF00; //PORTD - digital
//         PORTD->PWR = 0xAAAA0000; //PORTD8 - PORTD15 - fast edge

// //Address Bus[0:12]
// 		PORTE->ANALOG = 0xFFFF; // CS
// 		PORTE->PWR = 0xAAAAAAAA; // CS
// 		PORTE->RXTX = 0x0000;
// 		PORTE->OE = 0x0030;
// 		PORTE->FUNC = 0x01000000; // CS

// 		PORTF->ANALOG = 0xFFFF;
// 		PORTF->PWR = 0xFFFFFFFF;
// 		PORTF->FUNC = 0x55555555;

// 		PORTB->RXTX = 0;
//         PORTB->OE = 0x8000; //PORTD10 - PORTD14 - outputs
//         PORTB->FUNC = 0x00000000; //PORTD - ports
//         PORTB->ANALOG = 0xC000; //PORTD - digital
//         PORTB->PWR = 0xC0000000; //PORTD8 - PORTD15 - fast edge
}

//--------------------------------------------------------------------------------------
//Функция для настройки тактовых сигналов контроллера 1986ВЕ91Т1
//Параметр:	нет
//Возвращаемых знаений нет
//-------------------------------------------------------------------------------------- 
void ClkCfg()
{
//         RST_CLK -> HS_CONTROL = 0x00000001;
// 	
//         while((RST_CLK -> CLOCK_STATUS & 0x00000004) != 0x00000004)
// 			{}
// 			
// 		RST_CLK -> CPU_CLOCK = 0x00000102;
// 		//RST_CLK->PER_CLOCK = 0x63E00018;
// 		RST_CLK -> PER_CLOCK = 0xffffffff;
}
//--------------------------------------------------------------------------------------
//Функция для инициализации Ethernet-контроллера 5600ВГ1У
//Параметр:	нет
//Возвращаемых значений нет
//--------------------------------------------------------------------------------------
void EthCfg()
{
	int i;

	
	/*
// 	      My_MAC[0] = 0x0A;
//         My_MAC[1] = 0x1B;
//         My_MAC[2] = 0x2C;
//         My_MAC[3] = 0x3D;
//         My_MAC[4] = 0x4E;
//         My_MAC[5] = 0x5F;
	*/
		Ethernet->GCTRL = 0x8000;	//Reset 5600VG1U        
		for(i=0;i<1000;i++){}		//Little delay
		Ethernet->GCTRL = 0x4382;						   	
		Ethernet->MinFrame = 0x0040;
    	Ethernet->CollConfig = 0;
    	Ethernet->IPGTx = 0x000A;
    	Ethernet->MAC_ADDR_T = (MAC_adress[1]<<8)|MAC_adress[0];
    	Ethernet->MAC_ADDR_M = (MAC_adress[3]<<8)|MAC_adress[2];
    	Ethernet->MAC_ADDR_H = (MAC_adress[5]<<8)|MAC_adress[4];
        Ethernet->PHY_CTRL = 0x31D0;
		Ethernet->MAC_CTRL = 0x0200;  //Receive broadcast packet and packet with my MAC-address, littel endian ON
}

//--------------------------------------------------------------------------------------
//Функция для подготовки дескрипторов передатчика к отправке пакетов
//Параметр:	PacketLen - размер пакета в байтах,
//			TxCurrentDesc - указатель на текущий дескриптор отправляемых пакетов
//Возвращаемых значений нет
//--------------------------------------------------------------------------------------
int Write_Tx_Descriptor(unsigned short PacketLen, _tx_current_descriptor* TxCurrentDesc)
{
	unsigned int* MyDesc;
	MyDesc = (unsigned int*)(TxCurrentDesc->TxCurrentDescriptor->StartAddress);

	*(MyDesc+1) = PacketLen;
	*(MyDesc+3) = (TxCurrentDesc->FirstEmptyWord>>2)&0x0000FFFF;

	if(TxCurrentDesc->TxCurrentDescriptor->LastDesc == 1) 
		*MyDesc = 0xC000;
	else 
		*MyDesc = 0x8000;

	TxCurrentDesc->FirstEmptyWord += (PacketLen/2)<<2;	//адрес первого свободного 16-разрядного слова в буфере
												//сдвг PacketLen на 1 разряд, так как в передаваемом пакете
												//необходимо указывать размер в байтах, а в буфере - 16-разрядные слова!
	if(TxCurrentDesc->FirstEmptyWord > 0x60005FFC)
	{
		TxCurrentDesc->FirstEmptyWord -= 0x00006000;
		TxCurrentDesc->FirstEmptyWord |= 0x60004000;
	}
    return 0;
}

//--------------------------------------------------------------------------------------
//Функция для определения статуса отправляемого пакета
//Параметр:	TxDesc - указатель на дескриптор отправляемых пакетов
//Возвращает:	1 - пакет не отправлен
//				0 - пакет отправлен 
//--------------------------------------------------------------------------------------
unsigned short Read_Tx_Descriptor(_tx_descriptor* TxDesc)
{
		unsigned int* MyDesc;
		MyDesc = (unsigned int*)(TxDesc->StartAddress);
        if((*MyDesc & 0x8000) == 0x8000) 
			return 1;            //пакет не отправлен
        else 
			return 0;                                              //пакет отправлен
}

//--------------------------------------------------------------------------------------
//Функция для установки признака готовности к приему пакета дескриптором принимаемых пакетв
//Параметр:	RxDesc - указатель на дескриптор принимаемых пакетов
//Возвращает:	0
//--------------------------------------------------------------------------------------
int Ready_Rx_Descriptor(_rx_descriptor* RxDesc)
{
		unsigned int* MyDesc;
		MyDesc = (unsigned int*)(RxDesc->StartAddress);
	
		if(RxDesc->LastDesc == 1) 
			*MyDesc = 0xC000;	//установка готовности дескриптора к приему пакета
		else 
			*MyDesc = 0x8000;						//установка готовности дескриптора к приему пакета
		
        return 0;
}

//--------------------------------------------------------------------------------------
//Функция для определения наличия принятого пакета
//Параметр:	RxDesc - указатель на дескриптор принимаемых пакетов
//Возвращает:	1 - пакет не принят
//				0 - пакет принят
//--------------------------------------------------------------------------------------
unsigned short Read_Rx_Descriptor(_rx_descriptor* RxDesc)
{
		unsigned int* RDescPointer;
		RDescPointer = (unsigned int*)RxDesc->StartAddress;
	
        if( (*RDescPointer & 0x8000) == 0x8000) 
			return 1;	
        else 
			return 0;                                      
}

//--------------------------------------------------------------------------------------
//Функция для получения размера приянтого пакета
//Параметр:	RxDesc - указатель на дескриптор принимаемых пакетов
//Возвращает:	размер пакета в байтах
//--------------------------------------------------------------------------------------
unsigned short Read_Packet_Length(_rx_descriptor* RxDesc)
{
		unsigned int* RDescPointer;
		RDescPointer = (unsigned int*)(RxDesc->StartAddress + 4);		//получим адрес длины пакета
        return (unsigned short)(*RDescPointer);       //Temp;
}

//--------------------------------------------------------------------------------------
//Функция для получения адреса, по которому расположен пакета в буфере приемника
//Параметр:	RxDesc - указатель на дескриптор принимаемых пакетов
//Возвращает:	адрес пакета в буфере
//--------------------------------------------------------------------------------------
unsigned short Read_Packet_Start_Address(_rx_descriptor* RxDesc)
{
        unsigned int* RDescPointer;
		RDescPointer = (unsigned int*)(RxDesc->StartAddress + 12);	//получим адрес стартового адреса пакета
        return (unsigned short)(*RDescPointer);
}

