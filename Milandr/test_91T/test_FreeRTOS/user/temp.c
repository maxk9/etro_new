void vuIP_Task( void *pvParameters )
{
	uint32_t st;
	
	EthCfg();

	My_IP[0] =  uipIP_ADDR0;
	My_IP[1] =  uipIP_ADDR1;
	My_IP[2] =  uipIP_ADDR2;
	My_IP[3] =  uipIP_ADDR3;

	My_MAC[0] = mac[0];
	My_MAC[1] = mac[1];
	My_MAC[2] = mac[2];
	My_MAC[3] = mac[3];
	My_MAC[4] = mac[4];
	My_MAC[5] = mac[5];
	
	InitRxDescriptor(&RxCurrentDesc);
	InitTxDescriptor(&TxCurrentDesc);

	MDR_PORTD->RXTX = 1 << 12;
	MDR_PORTD->RXTX = 1 << 10;

	InitEthPacket(&Packet);
	
	for( ;; )
	{
#if READSTATUSON
		Request_Status(&Packet);
#endif	//READSTATUSON
		if(Read_Rx_Descriptor(RxCurrentDesc.RxCurrentDescriptor) == 0) //we receive Eth. packet
		{
			MDR_PORTD->RXTX |= 1 << 11;

			Packet.Length = Read_Packet_Length(RxCurrentDesc.RxCurrentDescriptor);
			Packet.Address = Read_Packet_Start_Address(RxCurrentDesc.RxCurrentDescriptor);
			Read_Packet(&Packet);   //считали пакет, который только что получили.

			Ready_Rx_Descriptor(RxCurrentDesc.RxCurrentDescriptor);
#if READSTATUSON
			Packet.CounterRxPacket++;
#endif	//READSTATUSON
			//модифицируем регистр головы буфера приемника RXBF_HEAD; записываем в него значение RXBF_TAIL - 1
			Write_Word(((Read_Word(0x1FD1)-1)&0x07FF),0x1FD0);	//регистр "хвоста" (RXBF_TAIL) буфера приемника (указывает на первый свободный адрес в буфере)

			TypeOfProtocol = (Packet.Data[12]<<8)|Packet.Data[13];

			switch(TypeOfProtocol) //0x0006 //слова в памяти разбиты по 2 байта! (считывать массив кратный 2 байтам!)
			{
				case 0x0608:    //receive ARP-packet
					Receive_IP[0] = Packet.Data[39];
					Receive_IP[1] = Packet.Data[38];
					Receive_IP[2] = Packet.Data[41];
					Receive_IP[3] = Packet.Data[40];

					if( (Receive_IP[0] == My_IP[0])&&(Receive_IP[1] == My_IP[1])&&(Receive_IP[2] == My_IP[2])&&(Receive_IP[3] == My_IP[3]))         //полное совпадение IP в пакете и моего. Надо отвечать
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

						MDR_PORTD->RXTX &= 0xF7FF;
					}
					else
					{
						MDR_PORTD->RXTX &= 0xF7FF;
					}
					break;

				case 0x0008:    //receive IP-packet
					Receive_IP[0] = Packet.Data[31];
					Receive_IP[1] = Packet.Data[30];
					Receive_IP[2] = Packet.Data[33];
					Receive_IP[3] = Packet.Data[32];

					if(	(Receive_IP[0] == My_IP[0])&&
						(Receive_IP[1] == My_IP[1])&&
						(Receive_IP[2] == My_IP[2])&&
						(Receive_IP[3] == My_IP[3]))  //получили IP-пакет с нашим IP адресом
					{
						Temp = CheckSum_IP(&Packet);
						if(Temp == ((Packet.Data[24] << 8)&0xFF00|Packet.Data[25]))  //если контрольная сумма IP-пакета и вычисленная совпадают, то анализируем пакет дальше, иначе отбрасываем его
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

							//проверяем следующий протокол (ICMP)
							if(Packet.Data[22] == 0x01)     //далее следует ICMP-протокол
							{																			
								if(Packet.Data[35] == 0x08) //приняли echo request
								{
									Temp = CheckSum_ICMP(&Packet);
									if(Temp == ((Packet.Data[36] << 8)&0xFF00|Packet.Data[37]))	  //если контрольная сумма ICMP-пакета и вычисленная совпадают, то формируем ответ на него, иначе отбрасываем его
									{
										Answear_ICMP(&Packet, ICMP_Packet);
									}
									MDR_PORTD->RXTX &= 0xF7FF;
								}
								else
								{
									MDR_PORTD->RXTX &= 0xF7FF;
								}
							}
							else
							{
								MDR_PORTD->RXTX &= 0xF7FF;
							}
						}
						else
						{
							MDR_PORTD->RXTX &= 0xF7FF;
						}
					}
					else
					{
						MDR_PORTD->RXTX &= 0xF7FF;
					}
			break;
			}
		} //if(Read_Rx_Descriptor(RxCurrentDescriptor) == 0)
		else
		{
			MDR_PORTD->RXTX &= 0xF7FF;
		}
		//переходим к анализу следующего дескриптора приемника
		RxCurrentDesc.RxCurrentDescriptor++;
		RxCurrentDesc.Number++;
		if(RxCurrentDesc.Number == NUMRXDESC)
		{
			RxCurrentDesc.RxCurrentDescriptor = RxDescriptor;
			RxCurrentDesc.Number = 0;	
		}

		st = uxTaskGetStackHighWaterMark(0);
		
		//vTaskDelay( 1 );
	}
}
