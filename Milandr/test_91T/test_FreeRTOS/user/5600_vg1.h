


#ifndef __5600_VG1_H__
#define __5600_VG1_H__

#include "MDR32Fx.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_port.h"
#include "uip.h"

/* IP address configuration. */
extern const unsigned char uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3;	
extern uint8_t mac[];			


unsigned int Send_CMD_SPISEL(void);                 //������� ������ ����������������� �����
int Reset_Eth(void);                                //������� ������ ����������� 5600��1�
void EthCfg(uint8_t *macadr);                                  //������� ���������������� ����������� 5600��1�

int Write_Word(unsigned short, unsigned short); 	//������� ������ ����� �� ������������� ������
unsigned short Read_Word(unsigned short);       	//������� ������ ����� �� ������������� ������

int WriteTxBufferData(unsigned char*, unsigned short, unsigned short);	//������� ��� ������ ������� ������ � ����� �����������
int ReadTxBufferData(unsigned char*, unsigned short, unsigned short);	//������� ��� ������ ������� ������ �� ������ �����������

int WriteRxBufferData(unsigned char*, unsigned short, unsigned short);	//������� ��� ������ ������� ������ � ����� ���������
int ReadRxBufferData(unsigned char*, unsigned short, unsigned short);	//������� ��� ������ ������� ������ �� ������ ���������

int Read_Array_Of_Data(unsigned char*, unsigned short, unsigned short);	//������� ��� ������ ������� ������
int Write_Array_Of_Data(unsigned char*, unsigned short, unsigned short);//������� ��� ������ ������� ������

unsigned char Get_Char(void);  //������� ���������� ���� ������ �� SPI ������



#endif /* __5600_VG1__ */ 



