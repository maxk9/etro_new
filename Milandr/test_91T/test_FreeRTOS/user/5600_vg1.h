


#ifndef __5600_VG1_H__
#define __5600_VG1_H__

#include "MDR32Fx.h"
#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_port.h"
#include "uip.h"

/* IP address configuration. */
extern const unsigned char uipIP_ADDR0,uipIP_ADDR1,uipIP_ADDR2,uipIP_ADDR3;	
extern uint8_t mac[];			


unsigned int Send_CMD_SPISEL(void);                 //функция выбора последовательного порта
int Reset_Eth(void);                                //функция сброса контроллера 5600ВГ1У
void EthCfg(uint8_t *macadr);                                  //функция конфигурирования контроллера 5600ВГ1У

int Write_Word(unsigned short, unsigned short); 	//функция записи слова по определенному адресу
unsigned short Read_Word(unsigned short);       	//функция чтения слова по определенному адресу

int WriteTxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для записи массива данных в буфер передатчика
int ReadTxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных из буфера передатчика

int WriteRxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для записи массива данных в буфер приемника
int ReadRxBufferData(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных из буфера приемника

int Read_Array_Of_Data(unsigned char*, unsigned short, unsigned short);	//функция для чтения массива данных
int Write_Array_Of_Data(unsigned char*, unsigned short, unsigned short);//функция для записи массива данных

unsigned char Get_Char(void);  //функция возвращает байт данных из SPI буфера



#endif /* __5600_VG1__ */ 



