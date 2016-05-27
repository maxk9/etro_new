
/*
�����:
����:  
������:	

���������, ����������� ICMP-�������� �� ���������������� 1986��91�1 � ���������� 5600��1�.


1986��91�1: ������� ������������ ����������������: 16 ��� .

������������ 5600��1�:	MAC-�����: 
          				����� ����������������� ������� � � ������ ����������� MAC-������

          				IP-�����: 192.168.100.87
          				���������� �� ICMP-��������� ����� ��������� �� 1450 ���� � ����� ������
						���������� �� UDP-��������� ����� ��������� �� 1450 ���� � ����� ������
						
Modbus
| ��� �-�� | ������ |
 �� 65 �� 72
						
*/

#include <string.h>

#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "ethconfig.h"

#include "udp_frame.h"	//������ ����� UDP
	



//��������� ��� ������ � ��������� Ethernet-��������
typedef struct
{
        uint8_t	   Data[1600];       //������ ������
        uint16_t	Length;          //������ ��������� ������
        uint16_t	Address;         //�����, �� �������� ���������� ����� � ������ ���������
        uint8_t		Remote_IP[4];     //��������� IP-����� (IP-����� ����������� ��������� ������)
        uint8_t		Remote_MAC[6];    //��������� MAC-����� (MAC-����� ����������� ��������� ������)

#if PRINTSTATUSON
		uint32_t CounterTxPacket;	//������� ������������ �������
		uint32_t CounterRxPacket;	//������� �������� �������
#endif	//PRINTSTATUSON

} _ethernet_packet;
						   
int Read_Packet(_ethernet_packet*);			//������� ��� ���������� ��������� ������
unsigned short CheckSum_IP(uint8_t*);		//������� ��� ���������� ����������� ����� IP-������
unsigned short CheckSum_ICMP(_ethernet_packet*);	//������� ��� ���������� ����������� ����� ICMP-������
void InitTxDescriptor(void);						//������� ������������� ������������ ������������ �������
void InitRxDescriptor(void);						//������� ������������� ������������ ����������� �������
int Answear_ARP(_ethernet_packet*);					//������� ������������ ������ �� ARP-������
int Answear_ICMP(_ethernet_packet*,unsigned char*);	//������� ������������ ������ �� ICMP-������
int Send_UDP(_ethernet_packet*, unsigned char*);
unsigned short CheckSum_UDP(uint8_t*); //

#if PRINTSTATUSON
void Request_Status(_ethernet_packet*);
#endif	//PRINTSTATUSON

//������ ������������ ������������ �������
_tx_descriptor ATxDescriptor[32] =  {{0x60006000,0x0000,0,0,0,0},{0x60006010,0x0000,0,0,0,0},{0x60006020,0x0000,0,0,0,0},{0x60006030,0x0000,0,0,0,0},
									 {0x60006040,0x0000,0,0,0,0},{0x60006050,0x0000,0,0,0,0},{0x60006060,0x0000,0,0,0,0},{0x60006070,0x0000,0,0,0,0},
									 {0x60006080,0x0000,0,0,0,0},{0x60006090,0x0000,0,0,0,0},{0x600060A0,0x0000,0,0,0,0},{0x600060B0,0x0000,0,0,0,0},
									 {0x600060C0,0x0000,0,0,0,0},{0x600060D0,0x0000,0,0,0,0},{0x600060E0,0x0000,0,0,0,0},{0x600060F0,0x0000,0,0,0,0},
									 {0x60006100,0x0000,0,0,0,0},{0x60006110,0x0000,0,0,0,0},{0x60006120,0x0000,0,0,0,0},{0x60006130,0x0000,0,0,0,0},
									 {0x60006140,0x0000,0,0,0,0},{0x60006150,0x0000,0,0,0,0},{0x60006160,0x0000,0,0,0,0},{0x60006170,0x0000,0,0,0,0},
									 {0x60006180,0x0000,0,0,0,0},{0x60006190,0x0000,0,0,0,0},{0x600061A0,0x0000,0,0,0,0},{0x600061B0,0x0000,0,0,0,0},
									 {0x600061C0,0x0000,0,0,0,0},{0x600061D0,0x0000,0,0,0,0},{0x600061E0,0x0000,0,0,0,0},{0x600061F0,0x4000,0,0,0,1}};

//������ ������������ ����������� �������
_rx_descriptor	ARxDescriptor[32] = {{0x60002000,0x8000,0,0,0,0},{0x60002010,0x8000,0,0,0,0},{0x60002020,0x8000,0,0,0,0},{0x60002030,0x8000,0,0,0,0},
									 {0x60002040,0x8000,0,0,0,0},{0x60002050,0x8000,0,0,0,0},{0x60002060,0x8000,0,0,0,0},{0x60002070,0x8000,0,0,0,0},
									 {0x60002080,0x8000,0,0,0,0},{0x60002090,0x8000,0,0,0,0},{0x600020A0,0x8000,0,0,0,0},{0x600020B0,0x8000,0,0,0,0},
									 {0x600020C0,0x8000,0,0,0,0},{0x600020D0,0x8000,0,0,0,0},{0x600020E0,0x8000,0,0,0,0},{0x600020F0,0x8000,0,0,0,0},
									 {0x60002100,0x8000,0,0,0,0},{0x60002110,0x8000,0,0,0,0},{0x60002120,0x8000,0,0,0,0},{0x60002130,0x8000,0,0,0,0},
									 {0x60002140,0x8000,0,0,0,0},{0x60002150,0x8000,0,0,0,0},{0x60002160,0x8000,0,0,0,0},{0x60002170,0x8000,0,0,0,0},
									 {0x60002180,0x8000,0,0,0,0},{0x60002190,0x8000,0,0,0,0},{0x600021A0,0x8000,0,0,0,0},{0x600021B0,0x8000,0,0,0,0},
									 {0x600021C0,0x8000,0,0,0,0},{0x600021D0,0x8000,0,0,0,0},{0x600021E0,0x8000,0,0,0,0},{0x600021F0,0xC000,0,0,0,1}};

										 
_tx_current_descriptor TxCurrentDesc;	//��������� ��������� _tx_current_descriptor
_rx_current_descriptor RxCurrentDesc;	//��������� ��������� _rx_current_descriptor
_ethernet_packet Packet;				//��������� ��������� _ethernet_packet
uint16_t Temp, TypeOfProtocol;	//Temp - ��������� ����������, TypeOfProtocol - ��� ��������� ������, ���������� � Eth2 �����.
uint8_t My_IP[4];		//����������, ��������� ��� IP � MAC-������
int UDP_Port = 0;
uint8_t Receive_IP[4];			//IP-����� ��������� ������
uint8_t ICMP_Packet[1500], UDP_Packet[1500];  		//����� ��� ������������ ��������� ICMP-������.

#if PRINTSTATUSON
_ethernet* MyEth;						//��������� �� ��������� _ethernet (������ ��� �������� ��������� ��������� Ethernet-����������� 5600��1�)
#endif	//PRINTSTATUSON
										 
#define UDP_DEST_PORT (int)6600

uint8_t UDP_Data[UDP_DATA_LEN];										 
										 
//unsigned int *MyPtrTx, *MyPtrRx, *ptrTD, *ptrRD;
//unsigned int adr, len, i;
//unsigned int ArrayRead[1000];
//***************************************************************************************************
//***************************************************************************************************
//***************************************************************************************************

void vUDP_Task( void *pvParameters )
{	

#if	PRINTSTATUSON
		MyEth = Ethernet;
#endif	//PRINTSTATUSON

        My_IP[0] =  193;
        My_IP[1] =  1;
        My_IP[2] =  1;
        My_IP[3] =  8;
//	    My_IP[0] =  192;
//        My_IP[1] =  168;
//        My_IP[2] =  1;
//        My_IP[3] =  8;

//         My_MAC[0] = 0x94;
//         My_MAC[1] = 0xDE;
//         My_MAC[2] = 0x80;
//         My_MAC[3] = 0xD9;
//         My_MAC[4] = 0x0B;
//         My_MAC[5] = 0x47;
	
// 	      My_MAC[0] = 0x0A;
//         My_MAC[1] = 0x1B;
//         My_MAC[2] = 0x2C;
//         My_MAC[3] = 0x3D;
//         My_MAC[4] = 0x4E;
//         My_MAC[5] = 0x5F;

// 		MAC_adress[0] = 0xAB;
//         MAC_adress[1] = 0x51;
//         MAC_adress[2] = 0x78;
//         MAC_adress[3] = 0xB9;
//         MAC_adress[4] = 0x64;
//         MAC_adress[5] = 0x94;


		InitTxDescriptor();	
		InitRxDescriptor();

		EthCfg();
		//PORTB->PORTx_SET=0x8000;

#if	PRINTSTATUSON
// 		Packet.CounterRxPacket = 0;
// 		Packet.CounterTxPacket = 0;
#endif	//PRINTSTATUSON

//������ Ethernet ���������������
        while(1)
        {
			if(Read_Rx_Descriptor(RxCurrentDesc.RxCurrentDescriptor) == 0) //�������� ������� ��������� ������
			{
				  //  PORTD->RXTX |= 1 << 11;
				
				Packet.Length = Read_Packet_Length(RxCurrentDesc.RxCurrentDescriptor);
				Packet.Address = Read_Packet_Start_Address(RxCurrentDesc.RxCurrentDescriptor);
				Read_Packet(&Packet);				   //������� ���������� �����

				Ready_Rx_Descriptor(RxCurrentDesc.RxCurrentDescriptor);
				
				
#if	PRINTSTATUSON
				Packet.CounterRxPacket++;
#endif	//PRINTSTATUSON						

				Ethernet->RXBF_HEAD = (Ethernet->RXBF_TAIL-1)&0x07FF;

				TypeOfProtocol = (Packet.Data[12]<<8)|Packet.Data[13];

				switch(TypeOfProtocol) //0x0006 //����� � ������ ������� �� 2 �����! (��������� ������ ������� 2 ������!)
				{
					case 0x0806:    //receive ARP-packet
						Receive_IP[0] = Packet.Data[38];
						Receive_IP[1] = Packet.Data[39];
						Receive_IP[2] = Packet.Data[40];
						Receive_IP[3] = Packet.Data[41];

						if((Receive_IP[0] == My_IP[0])&&(Receive_IP[1] == My_IP[1])&&(Receive_IP[2] == My_IP[2])&&(Receive_IP[3] == My_IP[3]))         //������ ���������� IP � ������ � ������. ���� ��������
						{
							Packet.Remote_IP[0] = Packet.Data[28];
							Packet.Remote_IP[1] = Packet.Data[29];
							Packet.Remote_IP[2] = Packet.Data[30];
							Packet.Remote_IP[3] = Packet.Data[31];

							Packet.Remote_MAC[0] = Packet.Data[22];
							Packet.Remote_MAC[1] = Packet.Data[23];
							Packet.Remote_MAC[2] = Packet.Data[24];
							Packet.Remote_MAC[3] = Packet.Data[25];
							Packet.Remote_MAC[4] = Packet.Data[26];
							Packet.Remote_MAC[5] = Packet.Data[27];
							
							
							Answear_ARP(&Packet);
							
						}
					  //  PORTD->RXTX &= 0xF7FF;
						break;

					case 0x0800:    //receive IP-packet     //��������� ICMP-��������
							Receive_IP[0] = Packet.Data[30];
							Receive_IP[1] = Packet.Data[31];
							Receive_IP[2] = Packet.Data[32];
							Receive_IP[3] = Packet.Data[33];

						if((Receive_IP[0] == My_IP[0])&&(Receive_IP[1] == My_IP[1])&&(Receive_IP[2] == My_IP[2])&&(Receive_IP[3] == My_IP[3]))  //�������� IP-����� � ����� IP �������
						{
							Temp = CheckSum_IP((uint8_t *)Packet.Data);
							if(Temp == ((Packet.Data[24] << 8)&0xFF00|Packet.Data[25]))  //���� ����������� ����� IP-��������� ������ � ����������� ���������, �� �������� ������, ����� ���������� �����
							{
								Packet.Remote_IP[0] = Packet.Data[26];
								Packet.Remote_IP[1] = Packet.Data[27];
								Packet.Remote_IP[2] = Packet.Data[28];
								Packet.Remote_IP[3] = Packet.Data[29];

								Packet.Remote_MAC[0] = Packet.Data[6];
								Packet.Remote_MAC[1] = Packet.Data[7];
								Packet.Remote_MAC[2] = Packet.Data[8];
								Packet.Remote_MAC[3] = Packet.Data[9];
								Packet.Remote_MAC[4] = Packet.Data[10];
								Packet.Remote_MAC[5] = Packet.Data[11];

								//��������� ��������� �������� (ICMP)
								if(Packet.Data[23] == 0x01)//ICMP-��������
								{																			
									if(Packet.Data[34] == 0x08) //������� echo request
									{
										Temp = CheckSum_ICMP(&Packet);
										if(Temp == ((Packet.Data[36] << 8)&0xFF00|Packet.Data[37]))	//�������� ���������� ����������� ����� ICMP-������ � �����������
										{
											Answear_ICMP(&Packet, ICMP_Packet);
										}
									}
								}
								else
								if(Packet.Data[23] == 0x11)//UDP-��������
								{	
									UDP_Port = (Packet.Data[36]<<8)|(Packet.Data[37]);
									if(UDP_Port == UDP_DEST_PORT) //���� ���������
									{
										Temp = CheckSum_UDP((uint8_t *)Packet.Data);
										if(!Temp)	//check CRC
										{
											//read input data
											PKDU_From_Eth.Type = (Packet.Data[42]<<8)|(Packet.Data[43]);
											PKDU_From_Eth.DG_Cmd = (Packet.Data[44]<<8)|(Packet.Data[45]);
											PKDU_From_Eth.GOM_Cmd = (Packet.Data[46]<<8)|(Packet.Data[47]);
											PKDU_From_Eth.DG_Un = (Packet.Data[48]<<8)|(Packet.Data[49]);
											PKDU_From_Eth.DG_Fn = (Packet.Data[50]<<8)|(Packet.Data[51]);
											PKDU_From_Eth.GOM_Fn = (Packet.Data[52]<<8)|(Packet.Data[53]);
											PKDU_From_Eth.GOM_Un = (Packet.Data[54]<<8)|(Packet.Data[55]);
											PKDU_From_Eth.Res1 = (Packet.Data[56]<<8)|(Packet.Data[57]);
											PKDU_From_Eth.Res2 = (Packet.Data[58]<<8)|(Packet.Data[59]);
											PKDU_From_Eth.Res3 = (Packet.Data[60]<<8)|(Packet.Data[61]);

											Send_UDP(&Packet, UDP_Packet);
										}
									}
								}
							}
						}
						//    PORTD->RXTX &= 0xF7FF;
					break;
				}
			} //if(Read_Rx_Descriptor(RxCurrentDescriptor) == 0) //we receive Eth. packet
	 //       PORTD->RXTX &= 0xF7FF;

			//������� � ���������� �����������
			RxCurrentDesc.RxCurrentDescriptor++;
			RxCurrentDesc.Number++;
			if(RxCurrentDesc.Number == NUMRXDESCRIPTOR)
			{
				RxCurrentDesc.RxCurrentDescriptor = ARxDescriptor;
				RxCurrentDesc.Number = 0;	
			}
			vTaskDelay( 1 );
        }
}

//--------------------------------------------------------------------------------------
//������� ��� ���������� ����������� ����� IP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� ����������� ����� IP-������
//--------------------------------------------------------------------------------------
unsigned short CheckSum_IP(uint8_t* Dt)
{
	unsigned long Temp, Check = 0;
	for(Temp = 0; Temp < 20; Temp += 2)
	{
			if(Temp == 10) continue;
			else Check += ((Dt[Temp + 14] << 8)&0xFF00)|Dt[Temp + 15];
	}
	Check = (Check >> 16) + (Check & 0xFFFF);
	
	return (unsigned short)(~Check);
}
//--------------------------------------------------------------------------------------
//������� ��� ���������� ����������� ����� ICMP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� ����������� ����� ICMP-������
//--------------------------------------------------------------------------------------
unsigned short CheckSum_ICMP(_ethernet_packet* Dt)
{
	unsigned long Temp, Check = 0;
	for(Temp = 0; Temp < Dt->Length - 38; Temp += 2)      //����������� ��� ������ ICMP ������� �� 1450 ����
	{                                                     //Dt->Length - 38,�.�. 34 ����� - ��� Eth2 � IP ���������, 4 ����� - CR�32 Eth2 ������
			if(Temp == 2) continue;
			else Check += ((Dt->Data[Temp + 34] << 8)&0xFF00)|Dt->Data[Temp + 35];
	}
	Check = (Check >> 16) + (Check & 0xFFFF);
	
	return (unsigned short)(~Check);
}
//--------------------------------------------------------------------------------------
//������� ��� ���������� ����������� ����� UDP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� ����������� ����� UDP-������
//--------------------------------------------------------------------------------------
unsigned short CheckSum_UDP(uint8_t* Dt)
{
	uint16_t Temp,  Len = 0;
	uint32_t Check = 0;
	
	Len = (uint8_t)(Dt[38]<<8)|((uint8_t)Dt[39]);
	
	for(Temp = 26; Temp < Len + 34; Temp += 2)      //����������� ��� ������ UDP ������� �� 1450 ����
	{                                               //
		Check += ((Dt[Temp] << 8)&0xFF00)|Dt[Temp + 1];
	}
	
	Check += (Dt[23]) + Len;
	
	while(Check>>16)
		Check = (Check >> 16) + (Check & 0xFFFF);
	
	return (uint16_t)(~Check);
}
//--------------------------------------------------------------------------------------
//������� ��� ���������� ������
//��������:	��������� �� �������� Ethernet-�����
//���������� 0
//--------------------------------------------------------------------------------------
int Read_Packet(_ethernet_packet* Dt)
{
	uint16_t Temp,Val;
	uint32_t* MyPointer;
	MyPointer = (uint32_t*)((uint32_t)RxBuffer + (uint32_t)(Dt->Address<<2));
	for(Temp=0; Temp<Dt->Length; Temp += 2)
	{
		Val = *MyPointer++;
		Dt->Data[Temp] = (uint8_t)Val;
		Dt->Data[Temp+1] = (uint8_t)(Val>>8);
		if( ((int)MyPointer & 0x00002000) == 0x00002000) 
			MyPointer = RxBuffer;
	}
	
	if((Dt->Length & 0x0001) == 1)  //���� ����� �������� �������� ���-�� ����, �� ���������� �������� �������������
									//��������� ���� � ������, ��� ��� �� ������ �� ����������� ����� ICMP-������.
	{
		Dt->Data[Dt->Length-4] = 0;
	}
	
	return 0;
}

//--------------------------------------------------------------------------------------
//������� ��� ������������ ������ �� ARP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� 0
//--------------------------------------------------------------------------------------
int Answear_ARP(_ethernet_packet* Dt)
{
	unsigned char Send[42];
	unsigned short Temp;
	unsigned int* MyPointer;

	while(Read_Tx_Descriptor(TxCurrentDesc.TxCurrentDescriptor) != 0)
	{
		TxCurrentDesc.TxCurrentDescriptor++;
		TxCurrentDesc.Number++;
		if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
		{
			TxCurrentDesc.Number = 0;
			TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
		}	
	}
	
	//������ 6 ���� - ��������� MAC - �����
	Send[0] = Dt->Remote_MAC[0];
	Send[1] = Dt->Remote_MAC[1];
	Send[2] = Dt->Remote_MAC[2];
	Send[3] = Dt->Remote_MAC[3];
	Send[4] = Dt->Remote_MAC[4];
	Send[5] = Dt->Remote_MAC[5];

	//����� 6 ���� - ��� MAC-�����
	Send[6] = MAC_adress[0];
	Send[7] = MAC_adress[1];
	Send[8] = MAC_adress[2];
	Send[9] = MAC_adress[3];
	Send[10] = MAC_adress[4];
	Send[11] = MAC_adress[5];

	//����� ��������� �� ��� �� ��� � ���������� ������ �� 19 ����� (������������)
	for(Temp = 12; Temp < 20; Temp++)
	{
			Send[Temp] = Dt->Data[Temp];
	}

	//20 � 21 ����� - Opcode: ��� ������ ������ ���� ����� 2
	Send[21] = 0x02;
	Send[20] = 0x00;

	//� 22 �� 27 (������������) ���� ��� MAC

	Send[22] = MAC_adress[0];
	Send[23] = MAC_adress[1];
	Send[24] = MAC_adress[2];
	Send[25] = MAC_adress[3];
	Send[26] = MAC_adress[4];
	Send[27] = MAC_adress[5];

	//� 28 �� 31 (������������) ���� ��� IP

	Send[28] = My_IP[0];
	Send[29] = My_IP[1];
	Send[30] = My_IP[2];
	Send[31] = My_IP[3];

	//� 32 �� 37 (������������) ���� ��������� MAC

	Send[32] = Dt->Remote_MAC[0];
	Send[33] = Dt->Remote_MAC[1];
	Send[34] = Dt->Remote_MAC[2];
	Send[35] = Dt->Remote_MAC[3];
	Send[36] = Dt->Remote_MAC[4];
	Send[37] = Dt->Remote_MAC[5];

	//� 38 �� 41 (������������) ���� ��������� IP

	Send[38] = Dt->Remote_IP[0];
	Send[39] = Dt->Remote_IP[1];
	Send[40] = Dt->Remote_IP[2];
	Send[41] = Dt->Remote_IP[3];
	
	MyPointer = (unsigned int*)TxCurrentDesc.FirstEmptyWord;

	for(Temp=0;Temp<42;Temp+=2)
	{
		*MyPointer++ = Send[Temp]|(Send[Temp+1]<<8);
		if((unsigned int)MyPointer > 0x60005FFC) MyPointer = (unsigned int*)0x60004000;
	}
	
	Write_Tx_Descriptor(0x002A,&TxCurrentDesc);     //42 bytes

	TxCurrentDesc.TxCurrentDescriptor++;
	TxCurrentDesc.Number++;
	if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
	{
		TxCurrentDesc.Number = 0;
		TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
	}

#if PRINTSTATUSON
		Packet.CounterTxPacket++;
#endif	//PRINTSTATUSON

		return 0;
}
//--------------------------------------------------------------------------------------
//������� ��� ������������ ������ �� ICMP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� 0
//--------------------------------------------------------------------------------------
int Answear_ICMP(_ethernet_packet* Dt , unsigned char* ICMP_Packet)
{
        unsigned short Temp;
		unsigned int* MyPointer;

        while(Read_Tx_Descriptor(TxCurrentDesc.TxCurrentDescriptor) != 0)
		{
			TxCurrentDesc.TxCurrentDescriptor++;
			TxCurrentDesc.Number++;
			if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
			{
				TxCurrentDesc.Number = 0;
				TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
			}
		}
		
        //Remote MAC-Address
        ICMP_Packet[0] = Dt->Remote_MAC[0];
        ICMP_Packet[1] = Dt->Remote_MAC[1];
        ICMP_Packet[2] = Dt->Remote_MAC[2];
        ICMP_Packet[3] = Dt->Remote_MAC[3];
        ICMP_Packet[4] = Dt->Remote_MAC[4];
        ICMP_Packet[5] = Dt->Remote_MAC[5];

        //My MAC-Address
        ICMP_Packet[6] = MAC_adress[0];
        ICMP_Packet[7] = MAC_adress[1];
        ICMP_Packet[8] = MAC_adress[2];
        ICMP_Packet[9] = MAC_adress[3];
        ICMP_Packet[10] = MAC_adress[4];
        ICMP_Packet[11] = MAC_adress[5];

        //IP-protocol
        ICMP_Packet[12] = 0x08;
        ICMP_Packet[13] = 0x00;
//�������� ����������� Eth2-�����

//��������� IP-�����
        //Field of IP-protocol
        for(Temp = 14; Temp < 24; Temp++)
        {
                ICMP_Packet[Temp] = Dt->Data[Temp];
        }
        ICMP_Packet[26] = Dt->Data[30];
        ICMP_Packet[27] = Dt->Data[31];
        ICMP_Packet[28] = Dt->Data[32];
        ICMP_Packet[29] = Dt->Data[33];
        //Remote IP
        ICMP_Packet[30] = Dt->Data[26];
        ICMP_Packet[31] = Dt->Data[27];
        ICMP_Packet[32] = Dt->Data[28];
        ICMP_Packet[33] = Dt->Data[29];

        Temp = CheckSum_IP(Dt->Data);

        //IP CheckSum
        ICMP_Packet[24] = (unsigned char)(Temp >> 8);
        ICMP_Packet[25] = (unsigned char)Temp;

//�������� ����������� IP-�����
//��������� ICMP-�����
        ICMP_Packet[34] = 0x00; //Echo reply
        ICMP_Packet[35] = 0x00;

        Dt->Data[34] = 0x00;
        Dt->Data[35] = 0x00;

        Temp = CheckSum_ICMP(Dt);
        ICMP_Packet[36] = (unsigned char)(Temp >> 8);
        ICMP_Packet[37] = (unsigned char)Temp;

        for(Temp = 38; Temp < Dt->Length - 4; Temp++)
        {
                ICMP_Packet[Temp] = Dt->Data[Temp];
        }

		MyPointer = (unsigned int*)TxCurrentDesc.FirstEmptyWord;

        if((Dt->Length & 0x0001) == 1)
        {
                ICMP_Packet[Dt->Length - 4] = Dt->Data[Dt->Length - 4];
                for(Temp=0;Temp<Dt->Length - 3;Temp+=2)
				{
					*MyPointer++ = ICMP_Packet[Temp]|(ICMP_Packet[Temp+1]<<8);
					if((unsigned int)MyPointer > 0x60005FFC) MyPointer = (unsigned int*)0x60004000;
				}
        }
        else
        {
                for(Temp=0;Temp<Dt->Length - 4;Temp+=2)
				{
					*MyPointer++ = ICMP_Packet[Temp]|(ICMP_Packet[Temp+1]<<8);
					if((unsigned int)MyPointer > 0x60005FFC) MyPointer = (unsigned int*)0x60004000;
				}
        }
//��������� ����������� ICMP-�����

        Write_Tx_Descriptor(Dt->Length-4,&TxCurrentDesc);     //74 bytes

		TxCurrentDesc.TxCurrentDescriptor++;
		TxCurrentDesc.Number++;
		if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
		{
			TxCurrentDesc.Number = 0;
			TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
		}

#if PRINTSTATUSON
		Packet.CounterTxPacket++;
#endif	//PRINTSTATUSON

        return 0;
}
//--------------------------------------------------------------------------------------
//������� ��� ������������ ������ �� UDP-������
//��������:	��������� �� �������� Ethernet-�����
//���������� 0
//--------------------------------------------------------------------------------------
int Send_UDP(_ethernet_packet* Dt, unsigned char* UDP_Packet)
{
        uint16_t Temp;
		uint16_t tmp_16 = 0;
		unsigned int* MyPointer;

        while(Read_Tx_Descriptor(TxCurrentDesc.TxCurrentDescriptor) != 0)
		{
			TxCurrentDesc.TxCurrentDescriptor++;
			TxCurrentDesc.Number++;
			if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
			{
				TxCurrentDesc.Number = 0;
				TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
			}
		}
		
        //Remote MAC-Address
        UDP_Packet[0] = Dt->Remote_MAC[0];
        UDP_Packet[1] = Dt->Remote_MAC[1];
        UDP_Packet[2] = Dt->Remote_MAC[2];
        UDP_Packet[3] = Dt->Remote_MAC[3];
        UDP_Packet[4] = Dt->Remote_MAC[4];
        UDP_Packet[5] = Dt->Remote_MAC[5];

        //My MAC-Address
        UDP_Packet[6] = MAC_adress[0];
        UDP_Packet[7] = MAC_adress[1];
        UDP_Packet[8] = MAC_adress[2];
        UDP_Packet[9] = MAC_adress[3];
        UDP_Packet[10] = MAC_adress[4];
        UDP_Packet[11] = MAC_adress[5];

        //IP-protocol
        UDP_Packet[12] = 0x08;
        UDP_Packet[13] = 0x00;
//�������� ����������� Eth2-�����
//��������� IP-�����		

		UDP_Packet[14] = Dt->Data[14]; // length
        UDP_Packet[15] = Dt->Data[15];
		Temp = UDP_DATA_LEN + 8 + 20;
		UDP_Packet[16] = (uint8_t)(Temp >> 8); //total length
        UDP_Packet[17] = (uint8_t)Temp;
		
        //Field of IP-protocol
        for(Temp = 18; Temp < 24; Temp++)
        {
			UDP_Packet[Temp] = Dt->Data[Temp];
        }
        UDP_Packet[26] = Dt->Data[30];
        UDP_Packet[27] = Dt->Data[31];
        UDP_Packet[28] = Dt->Data[32];
        UDP_Packet[29] = Dt->Data[33];
        //Remote IP
        UDP_Packet[30] = Dt->Data[26];
        UDP_Packet[31] = Dt->Data[27];
        UDP_Packet[32] = Dt->Data[28];
       // UDP_Packet[33] = Dt->Data[29];
		UDP_Packet[33] = 255;

		UDP_Packet[24] = 0;
		UDP_Packet[25] = 0;
        Temp = CheckSum_IP(UDP_Packet);

        //IP CheckSum
        UDP_Packet[24] = (uint8_t)(Temp >> 8);
        UDP_Packet[25] = (uint8_t)Temp;

//�������� ����������� IP-�����
//��������� UDP-�����
        UDP_Packet[34] = Dt->Data[36]; 
        UDP_Packet[35] = Dt->Data[37];//src port 6600

//		UDP_Packet[36] = Dt->Data[34]; 
//        UDP_Packet[37] = Dt->Data[35];//dest port
		UDP_Packet[36] = 0x19; 
		UDP_Packet[37] = 0xC9;//dest port 6601
		
		tmp_16 = UDP_DATA_LEN + 8;
		UDP_Packet[38] = (uint8_t)(tmp_16>>8); //length
        UDP_Packet[39] = (uint8_t)(tmp_16);

		UDP_Packet[40] = 0;
		UDP_Packet[41] = 0;
        
//������
		UDP_Packet[42] = (uint8_t)(PKDU_To_Eth.Type>>8);
		UDP_Packet[43] = (uint8_t)PKDU_To_Eth.Type;
		
		UDP_Packet[44] = (uint8_t)(PKDU_To_Eth.DG_Cmd>>8);
		UDP_Packet[45] = (uint8_t)PKDU_To_Eth.DG_Cmd;
		
		
		UDP_Packet[46] = (uint8_t)(PKDU_To_Eth.DG_res1>>8);
		UDP_Packet[47] = (uint8_t)PKDU_To_Eth.DG_res1;
		
		UDP_Packet[48] = (uint8_t)(PKDU_To_Eth.GOM_Cmd>>8);
		UDP_Packet[49] = (uint8_t)PKDU_To_Eth.GOM_Cmd;
	
		UDP_Packet[50] = (uint8_t)(PKDU_To_Eth.SET_GOM_cmd>>8);
		UDP_Packet[51] = (uint8_t)PKDU_To_Eth.SET_GOM_cmd;
		
		UDP_Packet[52] = (uint8_t)(PKDU_To_Eth.Indikator>>8);
		UDP_Packet[53] = (uint8_t)PKDU_To_Eth.Indikator;
		
		UDP_Packet[54] = (uint8_t)(PKDU_To_Eth.UL>>8);
		UDP_Packet[55] = (uint8_t)PKDU_To_Eth.UL;
		
		UDP_Packet[56] = (uint8_t)(PKDU_To_Eth.U_dg>>8);
		UDP_Packet[57] = (uint8_t)PKDU_To_Eth.U_dg;
		
		UDP_Packet[58] = (uint8_t)(PKDU_To_Eth.U_gom>>8);
		UDP_Packet[59] = (uint8_t)PKDU_To_Eth.U_gom;
		
		UDP_Packet[60] = (uint8_t)(PKDU_To_Eth.U_c>>8);
		UDP_Packet[61] = (uint8_t)PKDU_To_Eth.U_c;
		
		UDP_Packet[62] = (uint8_t)(PKDU_To_Eth.I_L>>8);
		UDP_Packet[63] = (uint8_t)PKDU_To_Eth.I_L;
		
		UDP_Packet[64] = (uint8_t)(PKDU_To_Eth.TOG>>8);
		UDP_Packet[65] = (uint8_t)PKDU_To_Eth.TOG;
		
		UDP_Packet[66] = (uint8_t)(PKDU_To_Eth.DM>>8);
		UDP_Packet[67] = (uint8_t)PKDU_To_Eth.DM;
		
		UDP_Packet[68] = (uint8_t)(PKDU_To_Eth.Ris_DG>>8);
		UDP_Packet[69] = (uint8_t)PKDU_To_Eth.Ris_DG;
		
		UDP_Packet[70] = (uint8_t)(PKDU_To_Eth.Ris_GOM>>8);
		UDP_Packet[71] = (uint8_t)PKDU_To_Eth.Ris_GOM;
		
		UDP_Packet[72] = (uint8_t)(PKDU_To_Eth.U_ab>>8);
		UDP_Packet[73] = (uint8_t)PKDU_To_Eth.U_ab;
		
		UDP_Packet[74] = (uint8_t)(PKDU_To_Eth.I_ab>>8);
		UDP_Packet[75] = (uint8_t)PKDU_To_Eth.I_ab;
		
		UDP_Packet[76] = (uint8_t)(PKDU_To_Eth.FLine>>8);
		UDP_Packet[77] = (uint8_t)PKDU_To_Eth.FLine;
		
		UDP_Packet[78] = (uint8_t)(PKDU_To_Eth.Reserv1>>8);
		UDP_Packet[79] = (uint8_t)PKDU_To_Eth.Reserv1;
		
		UDP_Packet[80] = (uint8_t)(PKDU_To_Eth.Reserv2>>8);
		UDP_Packet[81] = (uint8_t)PKDU_To_Eth.Reserv2;
		
		Temp = CheckSum_UDP(UDP_Packet);
        UDP_Packet[40] = (uint8_t)(Temp >> 8);
        UDP_Packet[41] = (uint8_t)Temp;

//         for(Temp = 80; Temp < UDP_DATA_LEN+42; Temp++)
//         {
// 			UDP_Packet[Temp] = PKDU_Status.DG_Cmd;
// 			++i;
//         }

		MyPointer = (uint32_t*)TxCurrentDesc.FirstEmptyWord;

		//tmp_16 = UDP_DATA_LEN + 33 + 8;
		tmp_16 = 82;
        if((tmp_16 & 0x0001) == 1)
        {
			UDP_Packet[tmp_16] = 0;
			for(Temp=0; Temp<(tmp_16); Temp += 2)
			{
				*MyPointer++ = UDP_Packet[Temp]|(UDP_Packet[Temp+1]<<8);
				if((uint32_t)MyPointer > 0x60005FFC) 
					MyPointer = (uint32_t*)0x60004000;
			}
        }
        else
        {
			for(Temp=0;Temp<(tmp_16);Temp += 2)
			{
				*MyPointer++ = UDP_Packet[Temp]|(UDP_Packet[Temp+1]<<8);
				if((uint32_t)MyPointer > 0x60005FFC) 
					MyPointer = (uint32_t*)0x60004000;
			}
        }
//��������� ����������� UDP-�����

        Write_Tx_Descriptor(tmp_16,&TxCurrentDesc);     //?? bytes

		TxCurrentDesc.TxCurrentDescriptor++;
		TxCurrentDesc.Number++;
		if(TxCurrentDesc.Number == NUMTXDESCRIPTOR)
		{
			TxCurrentDesc.Number = 0;
			TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
		}

#if PRINTSTATUSON
		Packet.CounterTxPacket++;
#endif	//PRINTSTATUSON

        return 0;
}
//--------------------------------------------------------------------------------------
//������� ��� ������������� ������������ ������������ �������
//��������:	���
//������������ �������� ���
//--------------------------------------------------------------------------------------
void InitTxDescriptor()
{
	unsigned int* TDescPointer;

	TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
	TxCurrentDesc.FirstEmptyWord = 0x60004000;

	for(Temp=0;Temp<NUMTXDESCRIPTOR;Temp++)
	{
		TDescPointer = (unsigned int*)(TxCurrentDesc.TxCurrentDescriptor->StartAddress);
		*TDescPointer = TxCurrentDesc.TxCurrentDescriptor->Status;
		TxCurrentDesc.TxCurrentDescriptor++;
	}

	TxCurrentDesc.TxCurrentDescriptor--;
	TxCurrentDesc.TxCurrentDescriptor->LastDesc = 1;	//��������� �������� ���������� �����������
	TDescPointer = (unsigned int*)(TxCurrentDesc.TxCurrentDescriptor->StartAddress);
	*TDescPointer = 0x4000;								//��������� �������������� ����������

	TxCurrentDesc.Number = 0;
	TxCurrentDesc.TxCurrentDescriptor = ATxDescriptor;
}
//--------------------------------------------------------------------------------------
//������� ��� ������������� ������������ ����������� �������
//��������:	���
//������������ �������� ���
//--------------------------------------------------------------------------------------
void InitRxDescriptor()
{
	unsigned int* RDescPointer;

	RxCurrentDesc.RxCurrentDescriptor = ARxDescriptor;

	for(Temp=0;Temp<NUMRXDESCRIPTOR;Temp++)
	{
		RDescPointer = (unsigned int*)(RxCurrentDesc.RxCurrentDescriptor->StartAddress);
		*RDescPointer = RxCurrentDesc.RxCurrentDescriptor->Status;
		RxCurrentDesc.RxCurrentDescriptor++;
	}

	RxCurrentDesc.RxCurrentDescriptor--;
	RxCurrentDesc.RxCurrentDescriptor->LastDesc = 1;	//��������� �������� ���������� �����������
	RDescPointer = (unsigned int*)(RxCurrentDesc.RxCurrentDescriptor->StartAddress);
	*RDescPointer = 0xC000;								//��������� �������������� ����������

	RxCurrentDesc.Number = 0;
	RxCurrentDesc.RxCurrentDescriptor = ARxDescriptor;
}






