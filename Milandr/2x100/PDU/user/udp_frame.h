

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
	uint16_t	DG_Cmd;      	//Udg1
	uint16_t	DG_res1;      	//Udg2
	uint16_t	GOM_Cmd;      	//Igen1
	uint16_t	SET_GOM_cmd;    //Igen2
	uint16_t	Indikator;      //Fgen1
	uint16_t	UL;  	    	//Fgen2
	uint16_t	U_dg;			//Pgen1
	uint16_t	U_gom;			//Pgen2
	uint16_t	U_c;			//Ndiz1
	uint16_t	I_L;			//Ndiz2
	uint16_t	TOG;			//PM1
	uint16_t	DM;				//PM2
	uint16_t	Ris_DG;			//TOG1
	uint16_t	Ris_GOM;      	//TOG2
	uint16_t	U_ab;			//UT1
	uint16_t	I_ab;			//UT2
} packet_To_eth;

#define UDP_DATA_LEN  (int)(34) //размер в байтах status_packet_To_eth


extern stat PKDU_Status;

#define RS485_TX_OFF	PORT_ResetBits(MDR_PORTD, PORT_Pin_2)
#define RS485_TX_ON		PORT_SetBits(MDR_PORTD, PORT_Pin_2)



#endif 










