#ifndef __ETHCONFIG_H_
#define __ETHCONFIG_H_

//Управляющие регистры Ethernrt-контроллера 5600ВГ1У
typedef struct
{
	unsigned int MAC_CTRL;
    unsigned int MinFrame;
    unsigned int MaxFrame;
    unsigned int CollConfig;
    unsigned int IPGTx;
    unsigned int MAC_ADDR_T;
    unsigned int MAC_ADDR_M;
    unsigned int MAC_ADDR_H;
    unsigned int HASH0;
    unsigned int HASH1;
    unsigned int HASH2;
    unsigned int HASH3;
    unsigned int INT_MSK;
    unsigned int INT_SRC;
    unsigned int PHY_CTRL;
    unsigned int PHY_STAT;
    unsigned int RXBF_HEAD;
    unsigned int RXBF_TAIL;
    unsigned int dammy0;
    unsigned int dammy1;
    unsigned int STAT_RX_ALL;
    unsigned int STAT_RX_OK;
    unsigned int STAT_RX_OVF;
    unsigned int STAT_RX_LOST;
    unsigned int STAT_TX_ALL;
    unsigned int STAT_TX_OK;
    unsigned int base_RxBF;
    unsigned int base_TxBF;
    unsigned int base_RxBD;
    unsigned int base_TxBD;
    unsigned int base_RG;
    unsigned int GCTRL;
} _ethernet;


extern const unsigned char MAC_adress[6];



//Стартовый адрес буфера приемника
#define BASE_ETH_RXBuffer		0x60000000  //Absolut ADDRESS = 0x0000

//Стартовый адрес буфера дескрипторов принимаемых пакетов
#define BASE_ETH_RXDescBuffer	0x60002000  //Absolut ADDRESS = 0x0800

//Стартовый адрес передатчика
#define BASE_ETH_TXBuffer		0x60004000  //Absolut ADDRESS = 0x1000

//Стартовый адрес буфера дескрипторов передаваемых пакетов
#define BASE_ETH_TXDescBuffer	0x60006000  //Absolut ADDRESS = 0x1800

//Стартовый адрес управляющих регистров Ethernrt-контроллера 5600ВГ1У
#define BASE_ETHERNET			0x60007F00  //Absolut ADDRESS = 0x1FC0

#define Ethernet              ((_ethernet *)BASE_ETHERNET)

#define RxBuffer              ((unsigned int *)BASE_ETH_RXBuffer)
#define RxDescriptor          ((unsigned int *)BASE_ETH_RXDescBuffer)

#define TxBuffer              ((unsigned int *)BASE_ETH_TXBuffer)
#define TxDescriptor          ((unsigned int *)BASE_ETH_TXDescBuffer)

//Стартовый адрес буфера приемника
#define BASE_ETH_RXBuffer		0x60000000  //Absolut ADDRESS = 0x0000

//Стартовый адрес буфера дескрипторов принимаемых пакетов
#define BASE_ETH_RXDescBuffer	0x60002000  //Absolut ADDRESS = 0x0800

//Стартовый адрес передатчика
#define BASE_ETH_TXBuffer		0x60004000  //Absolut ADDRESS = 0x1000

//Стартовый адрес буфера дескрипторов передаваемых пакетов
#define BASE_ETH_TXDescBuffer	0x60006000  //Absolut ADDRESS = 0x1800

//Стартовый адрес управляющих регистров Ethernrt-контроллера 5600ВГ1У
#define BASE_ETHERNET			0x60007F00  //Absolut ADDRESS = 0x1FC0

#define Ethernet              ((_ethernet *)BASE_ETHERNET)

#define RxBuffer              ((unsigned int *)BASE_ETH_RXBuffer)
#define RxDescriptor          ((unsigned int *)BASE_ETH_RXDescBuffer)

#define TxBuffer              ((unsigned int *)BASE_ETH_TXBuffer)
#define TxDescriptor          ((unsigned int *)BASE_ETH_TXDescBuffer)



#define NUMRXDESCRIPTOR	16	//количество дескрипротов принимаемых пакетов (минимум 1, максимум 32)
#define NUMTXDESCRIPTOR	16	//количество дескрипротов отправляемых пакетов (минимум 1, максимум 32)

#define PRINTSTATUSON	1	//компилляция программы с подключением отладочной информацией (1 - включено, 0 - отключено)


//Структура для дескриптора принимаемых пакетов
typedef struct
{
	unsigned int	StartAddress;			//начальный адрес дескриптора в памяти контроллера 1986ВЕ91Т1
	unsigned short	Status;					//стату дескриптора
	unsigned short	PacketLength;			//длина принятого пакета
	unsigned short	PacketStartAddressH;	//адрес в буфере, где расположен первый байт пакета (старшая часть, всегда 0х0000)
	unsigned short	PacketStartAddressL;	//адрес в буфере, где расположен первый байт пакета (младшая часть)
	unsigned short	LastDesc;				//признак последнего дескриптора (1 - последний дескриптор, 0 - не последний дескриптор)
} _rx_descriptor;

//Структура для работы с текущим дескриптором принимаемых пакетов
typedef struct
{
	_rx_descriptor* RxCurrentDescriptor;	//указатель на текущий дескриптор принимаемых пакетов
	unsigned short Number;					//номер текущего дескриптора (принимает значения от 0 до NUMRXDESCRIPTOR-1)
} _rx_current_descriptor;

//Структура для дескриптора отправляемых пакетов
typedef struct
{
	unsigned int	StartAddress;			//начальный адрес дескриптора в памяти контроллера 1986ВЕ91Т1
	unsigned short	Status;					//стату дескриптора
	unsigned short	PacketLength;			//длина принятого пакета
	unsigned short	PacketStartAddressH;	//адрес в буфере, где расположен первый байт пакета (старшая часть, всегда 0х0000)
	unsigned short	PacketStartAddressL;	//адрес в буфере, где расположен первый байт пакета (младшая часть)
	unsigned short	LastDesc;				//признак последнего дескриптора (1 - последний дескриптор, 0 - не последний дескриптор)
} _tx_descriptor;

//Структура для работы с текущим дескриптором отпарвляемых пакетов
typedef struct
{
	_tx_descriptor* TxCurrentDescriptor;	//указатель на текущий дескриптор отправляемых пакетов
	unsigned short Number;					//номер текущего дескриптора (принимает значения от 0 до NUMTXDESCRIPTOR-1)
	unsigned int FirstEmptyWord;			//адрес первого свободного слова в буфере передатчика
} _tx_current_descriptor;






void EthCfg(void);                                  //функция конфигурирования контроллера 5600ВГ1У
																			  
int Write_Tx_Descriptor(unsigned short,_tx_current_descriptor*); //функция записи дескриптора передатчика
unsigned short Read_Tx_Descriptor(_tx_descriptor*); //функция чтения дескриптора передатчика

int Ready_Rx_Descriptor(_rx_descriptor*);           //функция записи дескриптора приемника
unsigned short Read_Rx_Descriptor(_rx_descriptor*); //функция чтения дескриптора приемника

unsigned short Read_Packet_Start_Address(_rx_descriptor*);     //функция чтения стартового адреса принятого пакета
unsigned short Read_Packet_Length(_rx_descriptor*);            //функция чтения длины пакета

void ExtBusCfg(void);
void PortConfig(void);
void ClkCfg(void);



#endif /*__CONFIG_H_*/

