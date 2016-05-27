using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;//filestream
using System.IO.Ports;//порты
using Microsoft.Win32;//реестр
using System.Threading;//потоки

namespace WindowsFormsApplication1
{
	public partial class Form1 : Form
	{

		byte[] arr = new byte[20];
		byte[] read_buf = new byte[20];

		public Form1()
		{
			InitializeComponent();
		}

		//заполняет ListBox LB_Ports доступными портами
		private void RegCom()
		{
			int n_sb = 0;
			string[] ports = SerialPort.GetPortNames();

			ComSel.Items.Clear();
			foreach (string port in ports)
			{
				ComSel.Items.Add(port);
			}

			if (ComSel.Items.Count > 0)
			{
				ComSel.SelectedIndex = 0;
			}
			else
			{
				MessageBox.Show("Ни одного COM порта не найдено!", "Error!");
				//richTextBox1.AppendText("Ни одного COM порта не найдено!" + '\n');
				BT_COM_open.Enabled = false;
			}
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
			int n_sb = 0, temp = 0;
			try
			{
				foreach (CheckBox checkBox in this.tabPage3.Controls.OfType<CheckBox>())
				{
					if (checkBox.Checked)
					{
						n_sb |= (int)1 << (Int32.Parse(checkBox.Text) - 1);
					}
				}

				arr[0] = 0xAA;
				arr[1] = Convert.ToByte(n_sb & 0xFF);
				arr[2] = Convert.ToByte((n_sb >> 8) & 0xFF);
				arr[3] = Convert.ToByte((n_sb >> 16) & 0xFF);

				Application.DoEvents();

				if (sp.BytesToRead == 15)
				{
					sp.Read(read_buf, 0, 15);
					sp.DiscardInBuffer();
					sp.DiscardOutBuffer();

					//ADC_data1.Text = string.Format("{0:x2}", read_buf[0]).ToUpper();
					//ADC_data2.Text = string.Format("{0:x2}", read_buf[1]).ToUpper();
					//ADC_data3.Text = string.Format("{0:x2}", read_buf[2]).ToUpper();
					//ADC_data4.Text = string.Format("{0:x2}", read_buf[3]).ToUpper();
					//ADC_data5.Text = string.Format("{0:x2}", read_buf[4]).ToUpper();
					//ADC_data6.Text = string.Format("{0:x2}", read_buf[5]).ToUpper();
					//ADC_data7.Text = string.Format("{0:x2}", read_buf[6]).ToUpper();
					//ADC_data8.Text = string.Format("{0:x2}", read_buf[7]).ToUpper();
					ADC_data1.Text = Convert.ToString(read_buf[0]);
					ADC_data2.Text = Convert.ToString(read_buf[1]);
					ADC_data3.Text = Convert.ToString(read_buf[2]);
					ADC_data4.Text = Convert.ToString(read_buf[3]);
					ADC_data5.Text = Convert.ToString(read_buf[4]);
					ADC_data6.Text = Convert.ToString(read_buf[5]);
					ADC_data7.Text = Convert.ToString(read_buf[6]);
					ADC_data8.Text = Convert.ToString(read_buf[7]);

					temp = read_buf[8];
					if (temp > 95)
						temp = (256 * temp - 24520) / 100;
					else //minus
						temp = 0;

					T_sens.Text = Convert.ToString(temp) + "C";

					inPortA.Text = "b" + (Convert.ToString((int)(read_buf[10] << 8 | read_buf[9]), 2)).PadLeft(16, '0');

					label4.Text = "F1 = "+ (Convert.ToString((int)(read_buf[12] << 8 | read_buf[11]), 10)).PadLeft(4, '0');
					label6.Text = "F2 = "+ (Convert.ToString((int)(read_buf[14] << 8 | read_buf[13]), 10)).PadLeft(4, '0');
				}
				sp.Write(arr, 0, 4);
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message.ToString(), "Error!");
				timer1.Stop();
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			if (BT_COM_open.Text == "открыть")
			{
				sp.PortName = ComSel.SelectedItem.ToString();
				//sp.BaudRate = BaudRate;
				sp.BaudRate = 9600;
				sp.Parity = 0;
				sp.DataBits = 8;
				sp.StopBits = StopBits.Two;
				sp.WriteTimeout = 500;

				try//блочим элементы управления при открытии порта
				{
					sp.Open();
					if (sp.IsOpen)
					{
						BT_COM_open.Text = "закрыть";
						ComSel.Enabled = false;
						Data_frame.Enabled = true;
						sp.DiscardInBuffer();
						sp.DiscardOutBuffer();
						timer1.Start();

						Array.Clear(arr, 0, arr.Length);
						Array.Clear(read_buf, 0, read_buf.Length);

						sp.DiscardInBuffer();
						sp.DiscardOutBuffer();
					}
				}
				catch (Exception ex)
				{
					MessageBox.Show(ex.Message.ToString(), "Error!");
					timer1.Stop();
				}
			}
			else
			{
				sp.DiscardInBuffer();
				sp.DiscardOutBuffer();
				sp.Close();
				BT_COM_open.Text = "открыть";
				ComSel.Enabled = true;
				Data_frame.Enabled = false;
				timer1.Stop();
			}
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			RegCom();
		}

		//конвертер байтового массива в hex строку
		private string ArrToHexStr(byte[] bData)
		{
			string st = "";
			for (int i = 0; i < bData.Length; i++)
			{
				st += string.Format("{0:x2}", bData[i]).ToUpper() + " ";
			}
			return st;
		}

		//конвертер байтового массива в ascii строку
		private string ArrToAsciiStr(byte[] bData)
		{
			string st = "";
			ASCIIEncoding ascii = new ASCIIEncoding();

			for (int i = 0; i < bData.Length - 1; i++)
			{
				if (bData[i] > 0x20)
				{
					st += ascii.GetString(bData, i, 1);
				}
				else
				{
					st += ".";
				}
			}
			return st;
		}
	}
}
