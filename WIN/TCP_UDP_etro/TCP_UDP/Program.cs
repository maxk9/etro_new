using System;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace ConsoleApplication1
{
	
	class Program
	{
		private static uint cnt_read = 0;
		private static string[] str;
		public class UdpState
		{
			public IPEndPoint e;
			public UdpClient u;
		}

		public static bool messageReceived = false;

		public static void ReceiveCallback(IAsyncResult ar)
		{
			UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
			IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

			Byte[] receiveBytes = u.EndReceive(ar, ref e);

			str = new string[receiveBytes.Length];

			//str[0] = (Convert.ToString((int)(receiveBytes[0] << 8 | receiveBytes[1]), 2));//.PadLeft(16, '0');
			str[0] = (Convert.ToString((int)(receiveBytes[0] << 8 | receiveBytes[1]), 10));//.PadLeft(16, '0');
			str[1] = (Convert.ToString((int)(receiveBytes[2] << 8 | receiveBytes[3]), 10));
			str[2] = (Convert.ToString((int)(receiveBytes[4] << 8 | receiveBytes[5]), 10));
			str[3] = (Convert.ToString((int)(receiveBytes[6] << 8 | receiveBytes[7]), 10));
			str[4] = (Convert.ToString((int)(receiveBytes[8] << 8 | receiveBytes[9]), 10));//.PadLeft(16, '0');
			str[5] = (Convert.ToString((int)(receiveBytes[10] << 8 | receiveBytes[11]), 10));
			str[6] = (Convert.ToString((int)(receiveBytes[12] << 8 | receiveBytes[13]), 10));
			str[7] = (Convert.ToString((int)(receiveBytes[14] << 8 | receiveBytes[15]), 10));
			str[8] = (Convert.ToString((int)(receiveBytes[16] << 8 | receiveBytes[17]), 10));//.PadLeft(16, '0');
			str[9] = (Convert.ToString((int)(receiveBytes[18] << 8 | receiveBytes[19]), 10));//.PadLeft(16, '0');
			str[10] = (Convert.ToString((int)(receiveBytes[20] << 8 | receiveBytes[21]), 10));//.PadLeft(16, '0');
			str[11] = (Convert.ToString((int)(receiveBytes[22] << 8 | receiveBytes[23]), 10));//.PadLeft(16, '0');
			str[12] = (Convert.ToString((int)(receiveBytes[24] << 8 | receiveBytes[25]), 10));//.PadLeft(16, '0'); ;
			str[13] = (Convert.ToString((int)(receiveBytes[26] << 8 | receiveBytes[27]), 10));
			str[14] = (Convert.ToString((int)(receiveBytes[28] << 8 | receiveBytes[29]), 10));
			str[15] = (Convert.ToString((int)(receiveBytes[30] << 8 | receiveBytes[31]), 10));
			str[16] = (Convert.ToString((int)(receiveBytes[32] << 8 | receiveBytes[33]), 10));
			str[17] = (Convert.ToString((int)(receiveBytes[34] << 8 | receiveBytes[35]), 10));
			str[18] = (Convert.ToString((int)(receiveBytes[36] << 8 | receiveBytes[37]), 10));
			str[19] = (Convert.ToString((int)(receiveBytes[38] << 8 | receiveBytes[39]), 10));//.PadLeft(16, '0');
			str[20] = (Convert.ToString((int)(receiveBytes[40] << 8 | receiveBytes[41]), 10));//.PadLeft(16, '0');
			str[21] = (Convert.ToString((int)(receiveBytes[42] << 8 | receiveBytes[43]), 10));//.PadLeft(16, '0');
			str[22] = (Convert.ToString((int)(receiveBytes[44] << 8 | receiveBytes[45]), 2)).PadLeft(16, '0');
			//byte j = 0;
			//for (byte i = 0; i < receiveBytes.Length; i+=2)
			//    {
			//        str[j] = (Convert.ToString((int)(receiveBytes[i] << 8 | receiveBytes[i+1]), 2)).PadLeft(16, '0');
			//        j++;
			//    }
			//	string receiveString = Encoding.ASCII.GetString(receiveBytes);

			messageReceived = true;
		}


		static void Main(string[] args)
		{
			UdpClient u = new UdpClient();
			try
			{
				IPAddress ipAddress = IPAddress.Parse("192.168.100.87");
				IPEndPoint e = new IPEndPoint(ipAddress, 6600);
				

				UdpState s = new UdpState();

				s.e = e;
				s.u = u;

			 	u.Connect(ipAddress,6600);

				// Sends a message to the host to which you have connected.
				//Byte[] sendBytes = Encoding.ASCII.GetBytes(" Is anybody there?");
				Byte[] sendBytes = {0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,9,0,10,0,11,0,12};

				//string str;
				//byte[] vals = { 0x01, 0xAA, 0xB1, 0xDC, 0x10, 0xDD };
				//for (byte i = 0; i < vals.Length/2; i++)
				//{
				//    str[i] = BitConverter.ToString(vals,i,2).Replace("-", "");
				//}

				//string str = BitConverter.ToString(vals,0,2);
				
				//string str1 = vals.ToString();
				//str = Convert.ToString((int)(vals[0] << 8 | vals[1]), 2).PadLeft(16,'0');
				//str = str.PadLeft(16,'0');
				

				//Console.WriteLine("DG_Cmd:      "+ str);
				u.Send(sendBytes, sendBytes.Length);
				Console.SetWindowSize(60,40);
				Console.WriteLine("\nMessage sent to port: 6600 IP:" + ipAddress + "\n 01,02,03,04,05,06,07,08,09,010,011,012");
				u.BeginReceive(new AsyncCallback(ReceiveCallback), s);

				byte cnt = 0, cnt1 = 0;
				uint dt = 0;

				while (true)
				{
					Thread.Sleep(100);

					if (cnt > 100)
					{
						dt += (cnt - (cnt_read));
						cnt = 0;
						cnt_read = 0;
						Console.Clear();
					}

					if (messageReceived)
					{
						messageReceived = false;
						cnt1 = 0;
						/*
								uint16_t	DG_Cmd;      	//биты Команды ДГ
								uint16_t	DG_Un;      	//ДГ U
								uint16_t	DG_Fn;      	//ДГ F
								uint16_t	DG_res1;      	//ДГ рез
								uint16_t	GOM_Cmd;      	//биты Команды ГОМ
								uint16_t	GOM_Un;      	//ГОМ U
								uint16_t	GOM_Fn;         //ГОМ F
								uint16_t	GOM_res1;       //ГОМ рез
								uint16_t	SET_GOM_cmd;    //СЕТЬ ГОМ команды
								uint16_t	SET_GOM_res1;   //СЕТЬ ГОМ рез1
								uint16_t	SET_GOM_res2;   //СЕТЬ ГОМ рез2
								uint16_t	SET_GOM_res3;   //СЕТЬ ГОМ рез3
								uint16_t	Indikator;      //Индикация
								uint16_t	Ind_TOG;      	//Индикация ТОЖ
								uint16_t	Ind_PM;      	//Индикация ПМ
								uint16_t	Ind_Iload;      //Индикация ТОК нагр
								uint16_t	Ind_Uload;      //Индикация U нагр
								uint16_t	Ind_Iab;     	//Индикация ТОК ab
								uint16_t	Ind_Uab;     	//Индикация U ab
								uint16_t	Err_DG;      	//аварии ДГ
								uint16_t	Err_GOM;      	//аварии ГОМ
								uint16_t	Err_SET;      	//аварии сеть
								uint16_t	Ctrl_BTN;      	//button contrl
						*/
						Console.Write('\n');
						Console.SetCursorPosition(0, 4);
						Console.WriteLine("DG_Cmd:       {0}", str[0]);
						Console.WriteLine("DG_Un:        {0}", str[1]);
						Console.WriteLine("DG_Fn:        {0}", str[2]);
						Console.WriteLine("DG_res1:      {0}", str[3]);
						Console.WriteLine("GOM_Cmd:      {0}", str[4]);
						Console.WriteLine("GOM_Un:       {0}", str[5]);
						Console.WriteLine("GOM_Fn:       {0}", str[6]);
						Console.WriteLine("GOM_res1:     {0}", str[7]);
						Console.WriteLine("SET_GOM_cmd:  {0}", str[8]);
						Console.WriteLine("SET_GOM_res1: {0}", str[9]);
						Console.WriteLine("SET_GOM_res2: {0}", str[10]);
						Console.WriteLine("SET_GOM_res3: {0}", str[11]);
						Console.WriteLine("Indikator:    {0}", str[12]);
						Console.WriteLine("Ind_TOG:      {0}", str[13]);
						Console.WriteLine("Ind_PM:       {0}", str[14]);
						Console.WriteLine("Ind_Iload:    {0}", str[15]);
						Console.WriteLine("Ind_Uload:    {0}", str[16]);
						Console.WriteLine("Ind_Iab:      {0}", str[17]);
						Console.WriteLine("Ind_Uab:      {0}", str[18]);
						Console.WriteLine("Err_DG:       {0}", str[19]);
						Console.WriteLine("Err_GOM:      {0}", str[20]);
						Console.WriteLine("Err_SET:      {0}", str[21]);
						Console.WriteLine("Btn_Ctrl:     {0}", str[22]);
						++cnt_read;
						++cnt;
						u.Send(sendBytes, sendBytes.Length);
						u.BeginReceive(new AsyncCallback(ReceiveCallback), s);
					}
					else
					{
						++cnt1;
						if (cnt1 > 4)
						{
							cnt1 = 0;
							++cnt;
							u.Send(sendBytes, sendBytes.Length);
							u.BeginReceive(new AsyncCallback(ReceiveCallback), s);
						}
					}
					
					Console.SetCursorPosition(0, 27);
					Console.WriteLine("\n\nОтправлено {0,3}, потеряно {1,3} раз", cnt, dt);
					
				}	
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
			Console.ReadKey(true);
			u.Close();
		}
		 
		//static void Main(string[] args)
		//{
		//    try
		//    {
		//        Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram,
		//                            ProtocolType.Udp);

		//        //s.SetSocketOption(SocketOptionLevel.Udp, SocketOptionName.NoChecksum, true);
		//        IPAddress ipAddress = IPAddress.Parse("192.168.100.87");

		//        byte[] sendbuf = Encoding.ASCII.GetBytes(" ");
		//        IPEndPoint ep = new IPEndPoint(ipAddress, 6600);

		//        s.SendTo(sendbuf, ep);

		//        Console.WriteLine("Message sent to IP:" +ipAddress);
				
		//    }
		//    catch (Exception e)
		//    {
		//        Console.WriteLine(e.ToString());
		//    }
		//    Console.ReadKey(true);
		//}
	}
}
