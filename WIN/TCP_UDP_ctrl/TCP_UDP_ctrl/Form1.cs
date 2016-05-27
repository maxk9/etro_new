using System;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace WindowsFormsApplication1
{
	public partial class Form1 : Form
	{	
		private static uint cnt_read = 0;
		private static string[] read_str = new string[30], out_frame;
		//private static string send_str = "";
		UdpClient u = new UdpClient(6601);
		AsyncCallback callBack = new AsyncCallback(ReceiveCallback);
		private static byte cnt = 0, cnt1 = 0;
		private static uint dt = 0;
		private static byte[] sendBytes = { 1, 1, 
											0, 0,
											0, 0,
											0, 0, 
											0, 0, 
											0, 0, 
											0, 0, 
											0, 0, 
											0, 0, 
											0, 0, 
											}; 

		private static IPAddress ipAddress;// = IPAddress.Parse("193.1.1.8");
		private static IPEndPoint ep;// = new IPEndPoint(ipAddress, 6600);
		
		public class UdpState
		{
			public IPEndPoint e;
			public UdpClient u;
		}

		private static UdpState s = new UdpState();

		public static bool messageReceived = false;


		public static void ReceiveCallback(IAsyncResult ar)
		{
			try
			{
				UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
				IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

				Byte[] receiveBytes = u.EndReceive(ar, ref e);

				read_str = new string[receiveBytes.Length];

				//read_str[0] = (Convert.ToString((int)(receiveBytes[0] << 8 | receiveBytes[1]), 2));//.PadLeft(16, '0');
				read_str[0]  = "0x" + (Convert.ToString((int)(receiveBytes[0] << 8 | receiveBytes[1]), 16)).PadLeft(4, '0');
				read_str[1]  = "b" + (Convert.ToString((int)(receiveBytes[2] << 8 | receiveBytes[3]), 2)).PadLeft(16, '0');
				read_str[2]  = "b" + (Convert.ToString((int)(receiveBytes[4] << 8 | receiveBytes[5]), 2)).PadLeft(16, '0');
				read_str[3]  = "b" + (Convert.ToString((int)(receiveBytes[6] << 8 | receiveBytes[7]), 2)).PadLeft(16, '0');
				read_str[4]  = "b" + (Convert.ToString((int)(receiveBytes[8] << 8 | receiveBytes[9]), 2)).PadLeft(16, '0');
				read_str[5]  = "b" + (Convert.ToString((int)(receiveBytes[10] << 8 | receiveBytes[11]), 2)).PadLeft(16, '0');
				read_str[6]  = "0x" + (Convert.ToString((int)(receiveBytes[12] << 8 | receiveBytes[13]), 16)).PadLeft(4, '0');
				read_str[7]  = "0x" + (Convert.ToString((int)(receiveBytes[14] << 8 | receiveBytes[15]), 16)).PadLeft(4, '0');
				read_str[8]  = "0x" + (Convert.ToString((int)(receiveBytes[16] << 8 | receiveBytes[17]), 16)).PadLeft(4, '0');
				read_str[9]  = "0x" + (Convert.ToString((int)(receiveBytes[18] << 8 | receiveBytes[19]), 16)).PadLeft(4, '0');
				read_str[10] = "0x" + (Convert.ToString((int)(receiveBytes[20] << 8 | receiveBytes[21]), 16)).PadLeft(4, '0');
				read_str[11] = "0x" + (Convert.ToString((int)(receiveBytes[22] << 8 | receiveBytes[23]), 16)).PadLeft(4, '0');
				read_str[12] = "0x" + (Convert.ToString((int)(receiveBytes[24] << 8 | receiveBytes[25]), 16)).PadLeft(4, '0');
				read_str[13] = "0x" + (Convert.ToString((int)(receiveBytes[26] << 8 | receiveBytes[27]), 16)).PadLeft(4, '0');
				read_str[14] = "0x" + (Convert.ToString((int)(receiveBytes[28] << 8 | receiveBytes[29]), 16)).PadLeft(4, '0');
				read_str[15] = "0x" + (Convert.ToString((int)(receiveBytes[30] << 8 | receiveBytes[31]), 16)).PadLeft(4, '0');
				read_str[16] = "0x" + (Convert.ToString((int)(receiveBytes[32] << 8 | receiveBytes[33]), 16)).PadLeft(4, '0');
				read_str[17] = "0x" + (Convert.ToString((int)(receiveBytes[34] << 8 | receiveBytes[35]), 16)).PadLeft(4, '0');
				read_str[18] = "0x" + (Convert.ToString((int)(receiveBytes[36] << 8 | receiveBytes[37]), 16)).PadLeft(4, '0');
				read_str[19] = "0x" + (Convert.ToString((int)(receiveBytes[38] << 8 | receiveBytes[39]), 16)).PadLeft(4, '0');
				//read_str[17] = "0x" + (Convert.ToString((int)(receiveBytes[34] << 8 | receiveBytes[35]), 16)).PadLeft(4, '0');
				//read_str[18] = "0x" + (Convert.ToString((int)(receiveBytes[36] << 8 | receiveBytes[37]), 16)).PadLeft(4, '0');
				//read_str[19] = "0x" + (Convert.ToString((int)(receiveBytes[38] << 8 | receiveBytes[39]), 16)).PadLeft(4, '0');
				//read_str[20] = "0x" + (Convert.ToString((int)(receiveBytes[40] << 8 | receiveBytes[41]), 16)).PadLeft(4, '0');
				//read_str[21] = "0x" + (Convert.ToString((int)(receiveBytes[42] << 8 | receiveBytes[43]), 16)).PadLeft(4, '0');
				//read_str[22] = "b" + (Convert.ToString((int)(receiveBytes[44] << 8 | receiveBytes[45]), 2)).PadLeft(16, '0');
				//byte j = 0;
				//for (byte i = 0; i < receiveBytes.Length; i+=2)
				//    {
				//        read_str[j] = (Convert.ToString((int)(receiveBytes[i] << 8 | receiveBytes[i+1]), 2)).PadLeft(16, '0');
				//        j++;
				//    }
				//	string receiveString = Encoding.ASCII.GetString(receiveBytes);

				messageReceived = true;
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString(), "Ошибка!");
			}
		}

		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e)
		{
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
            int n_sb = 0;//, temp = 0;
            string send_str = "";
			try
			{
                foreach (CheckBox checkBox in groupBox1.Controls.OfType<CheckBox>())
				{
					if (checkBox.Checked)
					{
						n_sb |= (int)1 << (Int32.Parse(checkBox.Text) - 1);
					}
				}
                sendBytes[2] = (byte)((n_sb >> 8) & 0xFF);
                sendBytes[3] = (byte)((n_sb) & 0xFF);

                foreach (byte sb in sendBytes)
                    send_str += Convert.ToString(sb, 16) + ' ';

                out_frame[1] = (send_str);

				/*
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
				*/
				out_frame[2] =  "Тип:         " + read_str[0];
				out_frame[3] =  "DG_Cmd:      " + read_str[1];
				out_frame[4] =  "DG_res1:     " + read_str[2];
				out_frame[5] =  "GOM_Cmd:     " + read_str[3];
				out_frame[6] =  "SET_GOM_cmd: " + read_str[4];
				out_frame[7] =  "Indikator:   " + read_str[5];
				out_frame[8] =  "UL:          " + read_str[6];
				out_frame[9] =  "U_dg:        " + read_str[7];
				out_frame[10] = "U_gom:       " + read_str[8];
				out_frame[11] = "U_c:         " + read_str[9];
				out_frame[12] = "I_L:         " + read_str[10];
				out_frame[13] = "TOG:         " + read_str[11];
				out_frame[14] = "DM:          " + read_str[12];
				out_frame[15] = "Ris_DG:      " + read_str[13];
				out_frame[16] = "Ris_GOM:     " + read_str[14];
				out_frame[17] = "U_ab:        " + read_str[15];
				out_frame[18] = "I_ab:        " + read_str[16];
				out_frame[19] = "Fline:       " + read_str[17];
				out_frame[20] = "R1:          " + read_str[18];
				out_frame[21] = "R2:          " + read_str[19];
				out_frame[22] = "";
				out_frame[23] = "";
				//out_frame[21] = "Err_DG:       " + read_str[19];
				//out_frame[22] = "Err_GOM:      " + read_str[20];
				//out_frame[23] = "Err_SET:      " + read_str[21];
				//out_frame[24] = "Btn_Ctrl:     " + read_str[22];
				

				if (cnt > 99)
				{			
					cnt = 0;
					cnt_read = 0;
				}

                dt = (cnt - (cnt_read));

				if (messageReceived)
				{
					messageReceived = false;
					cnt1 = 0;

					++cnt_read;
					++cnt;
					//u.Send(sendBytes, sendBytes.Length);
					//u.BeginReceive(callBack, s);
				}
				else
				{
					++cnt1;
					if (cnt1 > 4)
					{
						cnt1 = 0;
						++cnt;
						//u.Send(sendBytes, sendBytes.Length);
						//u.BeginReceive(callBack, s);
					}
				}
				out_frame[23] = ("Отправлено " + cnt + ", потеряно " + dt + " раз.");
				TB1.Text = "";
				foreach (string n in out_frame)
					TB1.Text +="\n\r"+ n + "\n\r";


				u.Send(sendBytes, sendBytes.Length);

				//u.BeginReceive(new AsyncCallback(ReceiveCallback), s);
				u.BeginReceive(callBack, s);
			}
			catch (Exception ex)
			{
				TB1.Clear();
				TB1.AppendText(ex.Message);
				timer1.Stop();
			}
			//Application.DoEvents();
		}

		private void Form1_FormClosed(object sender, FormClosedEventArgs e)
		{
			timer1.Stop();
			//u.Close();
		}

        private void button1_Click(object sender, EventArgs e)
        {
           try
			{
                if (button1.Text != "закрыть")
                {
                    //string ip_str = textBox1.Text + "." + textBox2.Text + "." + textBox3.Text + "." + textBox4.Text;
                    string ip_str = maskedTextBox1.Text.ToString().Replace(" ","");
                    //ipAddress = IPAddress.Parse("193.1.1.8");
                    ipAddress = IPAddress.Parse(ip_str);
                    ep = new IPEndPoint(ipAddress, 6600);

                    s.e = ep;
                    s.u = u;

                    out_frame = new string[100];

                    out_frame[0] = ("Message sent to port: 6600, IP:" + ipAddress);

                    u.Connect(ipAddress, 6600);

                    button1.Text = "закрыть";
                    maskedTextBox1.Enabled = false;
                    groupBox1.Enabled = true;
                    TB1.Enabled = true;

                    timer1.Start();
                }
                else
                {
                    Close();
                }
           }
           catch (Exception ex)
           {
               TB1.Clear();
               TB1.AppendText(ex.Message);
           }
        }
	}
}







