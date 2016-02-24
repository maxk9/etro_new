

#ifndef __UDP_FRAME_H
#define __UDP_FRAME_H


typedef struct 
{
	uint32_t Ctrl_BTN;
	uint8_t ERROR;	//ошибка ПКДУ
}stat;


//Структура сигналов для обмена Ethernet-пакетами
typedef struct
{
	uint16_t	DG_Cmd;      	//биты Команды ДГ
	uint16_t	GOM_Cmd;      	//биты Команды ГОМ
	uint16_t	DG_Un;      	//DG U
	uint16_t	DG_Fn;         	//DG F
	uint16_t	GOM_Fn;         //ГОМ F
	uint16_t	GOM_Un;      	//ГОМ U
	uint16_t	Res1;     	    //Res1
	uint16_t	Res2;      	  	//Res2
	uint16_t	Res3;      		//Res3
} packet_From_eth;

typedef struct
{
	uint16_t	Type;    	  	//ист=1 ТС=1
	uint16_t	DG_Cmd;      	//биты Команды ДГ
	uint16_t	DG_res1;      	//
	uint16_t	GOM_Cmd;      	//
	uint16_t	SET_GOM_cmd;    //
	uint16_t	Indikator;      //
	uint16_t	UL;  	    	//
	uint16_t	U_dg;			//
	uint16_t	U_gom;			//
	uint16_t	U_c;			//
	uint16_t	I_L;			//
	uint16_t	TOG;			//
	uint16_t	DM;				//
	uint16_t	Ris_DG;			//
	uint16_t	Ris_GOM;      	//
	uint16_t	U_ab;			//
	uint16_t	I_ab;			//
} packet_To_eth;

#define UDP_DATA_LEN  (int)(34) //размер в байтах status_packet_To_eth


extern stat PKDU_Status;

#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTD, PORT_Pin_2)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTD, PORT_Pin_2)



#endif 










