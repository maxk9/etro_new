#ifndef __ETHCONFIG_H_
#define __ETHCONFIG_H_

//����������� �������� Ethernrt-����������� 5600��1�
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



//��������� ����� ������ ���������
#define BASE_ETH_RXBuffer		0x60000000  //Absolut ADDRESS = 0x0000

//��������� ����� ������ ������������ ����������� �������
#define BASE_ETH_RXDescBuffer	0x60002000  //Absolut ADDRESS = 0x0800

//��������� ����� �����������
#define BASE_ETH_TXBuffer		0x60004000  //Absolut ADDRESS = 0x1000

//��������� ����� ������ ������������ ������������ �������
#define BASE_ETH_TXDescBuffer	0x60006000  //Absolut ADDRESS = 0x1800

//��������� ����� ����������� ��������� Ethernrt-����������� 5600��1�
#define BASE_ETHERNET			0x60007F00  //Absolut ADDRESS = 0x1FC0

#define Ethernet              ((_ethernet *)BASE_ETHERNET)

#define RxBuffer              ((unsigned int *)BASE_ETH_RXBuffer)
#define RxDescriptor          ((unsigned int *)BASE_ETH_RXDescBuffer)

#define TxBuffer              ((unsigned int *)BASE_ETH_TXBuffer)
#define TxDescriptor          ((unsigned int *)BASE_ETH_TXDescBuffer)

//��������� ����� ������ ���������
#define BASE_ETH_RXBuffer		0x60000000  //Absolut ADDRESS = 0x0000

//��������� ����� ������ ������������ ����������� �������
#define BASE_ETH_RXDescBuffer	0x60002000  //Absolut ADDRESS = 0x0800

//��������� ����� �����������
#define BASE_ETH_TXBuffer		0x60004000  //Absolut ADDRESS = 0x1000

//��������� ����� ������ ������������ ������������ �������
#define BASE_ETH_TXDescBuffer	0x60006000  //Absolut ADDRESS = 0x1800

//��������� ����� ����������� ��������� Ethernrt-����������� 5600��1�
#define BASE_ETHERNET			0x60007F00  //Absolut ADDRESS = 0x1FC0

#define Ethernet              ((_ethernet *)BASE_ETHERNET)

#define RxBuffer              ((unsigned int *)BASE_ETH_RXBuffer)
#define RxDescriptor          ((unsigned int *)BASE_ETH_RXDescBuffer)

#define TxBuffer              ((unsigned int *)BASE_ETH_TXBuffer)
#define TxDescriptor          ((unsigned int *)BASE_ETH_TXDescBuffer)



#define NUMRXDESCRIPTOR	16	//���������� ������������ ����������� ������� (������� 1, �������� 32)
#define NUMTXDESCRIPTOR	16	//���������� ������������ ������������ ������� (������� 1, �������� 32)

#define PRINTSTATUSON	1	//����������� ��������� � ������������ ���������� ����������� (1 - ��������, 0 - ���������)


//��������� ��� ����������� ����������� �������
typedef struct
{
	unsigned int	StartAddress;			//��������� ����� ����������� � ������ ����������� 1986��91�1
	unsigned short	Status;					//����� �����������
	unsigned short	PacketLength;			//����� ��������� ������
	unsigned short	PacketStartAddressH;	//����� � ������, ��� ���������� ������ ���� ������ (������� �����, ������ 0�0000)
	unsigned short	PacketStartAddressL;	//����� � ������, ��� ���������� ������ ���� ������ (������� �����)
	unsigned short	LastDesc;				//������� ���������� ����������� (1 - ��������� ����������, 0 - �� ��������� ����������)
} _rx_descriptor;

//��������� ��� ������ � ������� ������������ ����������� �������
typedef struct
{
	_rx_descriptor* RxCurrentDescriptor;	//��������� �� ������� ���������� ����������� �������
	unsigned short Number;					//����� �������� ����������� (��������� �������� �� 0 �� NUMRXDESCRIPTOR-1)
} _rx_current_descriptor;

//��������� ��� ����������� ������������ �������
typedef struct
{
	unsigned int	StartAddress;			//��������� ����� ����������� � ������ ����������� 1986��91�1
	unsigned short	Status;					//����� �����������
	unsigned short	PacketLength;			//����� ��������� ������
	unsigned short	PacketStartAddressH;	//����� � ������, ��� ���������� ������ ���� ������ (������� �����, ������ 0�0000)
	unsigned short	PacketStartAddressL;	//����� � ������, ��� ���������� ������ ���� ������ (������� �����)
	unsigned short	LastDesc;				//������� ���������� ����������� (1 - ��������� ����������, 0 - �� ��������� ����������)
} _tx_descriptor;

//��������� ��� ������ � ������� ������������ ������������ �������
typedef struct
{
	_tx_descriptor* TxCurrentDescriptor;	//��������� �� ������� ���������� ������������ �������
	unsigned short Number;					//����� �������� ����������� (��������� �������� �� 0 �� NUMTXDESCRIPTOR-1)
	unsigned int FirstEmptyWord;			//����� ������� ���������� ����� � ������ �����������
} _tx_current_descriptor;






void EthCfg(void);                                  //������� ���������������� ����������� 5600��1�
																			  
int Write_Tx_Descriptor(unsigned short,_tx_current_descriptor*); //������� ������ ����������� �����������
unsigned short Read_Tx_Descriptor(_tx_descriptor*); //������� ������ ����������� �����������

int Ready_Rx_Descriptor(_rx_descriptor*);           //������� ������ ����������� ���������
unsigned short Read_Rx_Descriptor(_rx_descriptor*); //������� ������ ����������� ���������

unsigned short Read_Packet_Start_Address(_rx_descriptor*);     //������� ������ ���������� ������ ��������� ������
unsigned short Read_Packet_Length(_rx_descriptor*);            //������� ������ ����� ������

void ExtBusCfg(void);
void PortConfig(void);
void ClkCfg(void);



#endif /*__CONFIG_H_*/

