namespace WindowsFormsApplication1
{
	partial class Form1
	{
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		/// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Код, автоматически созданный конструктором форм Windows

		/// <summary>
		/// Обязательный метод для поддержки конструктора - не изменяйте
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.ComSel = new System.Windows.Forms.ComboBox();
			this.BT_COM_open = new System.Windows.Forms.Button();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.sp = new System.IO.Ports.SerialPort(this.components);
			this.Data_frame = new System.Windows.Forms.GroupBox();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.T_sens = new System.Windows.Forms.Label();
			this.ADC_data8 = new System.Windows.Forms.Label();
			this.ADC_data7 = new System.Windows.Forms.Label();
			this.ADC_data6 = new System.Windows.Forms.Label();
			this.ADC_data5 = new System.Windows.Forms.Label();
			this.ADC_data4 = new System.Windows.Forms.Label();
			this.ADC_data3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.ADC_data2 = new System.Windows.Forms.Label();
			this.label15 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.ADC_data1 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.inPortA = new System.Windows.Forms.Label();
			this.tabPage3 = new System.Windows.Forms.TabPage();
			this.OUT15 = new System.Windows.Forms.CheckBox();
			this.OUT14 = new System.Windows.Forms.CheckBox();
			this.OUT13 = new System.Windows.Forms.CheckBox();
			this.OUT10 = new System.Windows.Forms.CheckBox();
			this.OUT5 = new System.Windows.Forms.CheckBox();
			this.OUT9 = new System.Windows.Forms.CheckBox();
			this.OUT4 = new System.Windows.Forms.CheckBox();
			this.OUT8 = new System.Windows.Forms.CheckBox();
			this.OUT3 = new System.Windows.Forms.CheckBox();
			this.OUT12 = new System.Windows.Forms.CheckBox();
			this.OUT24 = new System.Windows.Forms.CheckBox();
			this.OUT23 = new System.Windows.Forms.CheckBox();
			this.OUT22 = new System.Windows.Forms.CheckBox();
			this.OUT7 = new System.Windows.Forms.CheckBox();
			this.OUT20 = new System.Windows.Forms.CheckBox();
			this.OUT19 = new System.Windows.Forms.CheckBox();
			this.OUT18 = new System.Windows.Forms.CheckBox();
			this.OUT17 = new System.Windows.Forms.CheckBox();
			this.OUT2 = new System.Windows.Forms.CheckBox();
			this.OUT21 = new System.Windows.Forms.CheckBox();
			this.OUT11 = new System.Windows.Forms.CheckBox();
			this.OUT16 = new System.Windows.Forms.CheckBox();
			this.OUT6 = new System.Windows.Forms.CheckBox();
			this.OUT1 = new System.Windows.Forms.CheckBox();
			this.tabPage4 = new System.Windows.Forms.TabPage();
			this.tabPage5 = new System.Windows.Forms.TabPage();
			this.label19 = new System.Windows.Forms.Label();
			this.label18 = new System.Windows.Forms.Label();
			this.label20 = new System.Windows.Forms.Label();
			this.label21 = new System.Windows.Forms.Label();
			this.label22 = new System.Windows.Forms.Label();
			this.label23 = new System.Windows.Forms.Label();
			this.label24 = new System.Windows.Forms.Label();
			this.label25 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox1.SuspendLayout();
			this.Data_frame.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.tabPage3.SuspendLayout();
			this.tabPage4.SuspendLayout();
			this.tabPage5.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.ComSel);
			this.groupBox1.Controls.Add(this.BT_COM_open);
			this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
			this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(440, 58);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "COM порт";
			// 
			// ComSel
			// 
			this.ComSel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ComSel.FormattingEnabled = true;
			this.ComSel.Location = new System.Drawing.Point(7, 20);
			this.ComSel.Name = "ComSel";
			this.ComSel.Size = new System.Drawing.Size(121, 28);
			this.ComSel.TabIndex = 1;
			// 
			// BT_COM_open
			// 
			this.BT_COM_open.AutoSize = true;
			this.BT_COM_open.BackColor = System.Drawing.Color.Red;
			this.BT_COM_open.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.BT_COM_open.ForeColor = System.Drawing.Color.Black;
			this.BT_COM_open.Location = new System.Drawing.Point(148, 20);
			this.BT_COM_open.Name = "BT_COM_open";
			this.BT_COM_open.Size = new System.Drawing.Size(90, 30);
			this.BT_COM_open.TabIndex = 0;
			this.BT_COM_open.Text = "открыть";
			this.BT_COM_open.UseVisualStyleBackColor = false;
			this.BT_COM_open.Click += new System.EventHandler(this.button1_Click);
			// 
			// timer1
			// 
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// Data_frame
			// 
			this.Data_frame.Controls.Add(this.tabControl1);
			this.Data_frame.Dock = System.Windows.Forms.DockStyle.Fill;
			this.Data_frame.Enabled = false;
			this.Data_frame.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.Data_frame.Location = new System.Drawing.Point(0, 58);
			this.Data_frame.Name = "Data_frame";
			this.Data_frame.Size = new System.Drawing.Size(440, 341);
			this.Data_frame.TabIndex = 1;
			this.Data_frame.TabStop = false;
			this.Data_frame.Text = "Данные";
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabPage1);
			this.tabControl1.Controls.Add(this.tabPage2);
			this.tabControl1.Controls.Add(this.tabPage3);
			this.tabControl1.Controls.Add(this.tabPage4);
			this.tabControl1.Controls.Add(this.tabPage5);
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tabControl1.ItemSize = new System.Drawing.Size(70, 18);
			this.tabControl1.Location = new System.Drawing.Point(3, 22);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(434, 316);
			this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
			this.tabControl1.TabIndex = 0;
			// 
			// tabPage1
			// 
			this.tabPage1.BackColor = System.Drawing.Color.Transparent;
			this.tabPage1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
			this.tabPage1.Controls.Add(this.T_sens);
			this.tabPage1.Controls.Add(this.ADC_data8);
			this.tabPage1.Controls.Add(this.ADC_data7);
			this.tabPage1.Controls.Add(this.ADC_data6);
			this.tabPage1.Controls.Add(this.ADC_data5);
			this.tabPage1.Controls.Add(this.ADC_data4);
			this.tabPage1.Controls.Add(this.ADC_data3);
			this.tabPage1.Controls.Add(this.label2);
			this.tabPage1.Controls.Add(this.ADC_data2);
			this.tabPage1.Controls.Add(this.label15);
			this.tabPage1.Controls.Add(this.label13);
			this.tabPage1.Controls.Add(this.label11);
			this.tabPage1.Controls.Add(this.label9);
			this.tabPage1.Controls.Add(this.label7);
			this.tabPage1.Controls.Add(this.label5);
			this.tabPage1.Controls.Add(this.label3);
			this.tabPage1.Controls.Add(this.ADC_data1);
			this.tabPage1.Controls.Add(this.label1);
			this.tabPage1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tabPage1.Location = new System.Drawing.Point(4, 22);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage1.Size = new System.Drawing.Size(426, 290);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "АЦП";
			// 
			// T_sens
			// 
			this.T_sens.AutoSize = true;
			this.T_sens.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.T_sens.Location = new System.Drawing.Point(85, 273);
			this.T_sens.Name = "T_sens";
			this.T_sens.Size = new System.Drawing.Size(45, 20);
			this.T_sens.TabIndex = 1;
			this.T_sens.Text = "0000";
			// 
			// ADC_data8
			// 
			this.ADC_data8.AutoSize = true;
			this.ADC_data8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data8.Location = new System.Drawing.Point(85, 201);
			this.ADC_data8.Name = "ADC_data8";
			this.ADC_data8.Size = new System.Drawing.Size(45, 20);
			this.ADC_data8.TabIndex = 1;
			this.ADC_data8.Text = "0000";
			// 
			// ADC_data7
			// 
			this.ADC_data7.AutoSize = true;
			this.ADC_data7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data7.Location = new System.Drawing.Point(85, 173);
			this.ADC_data7.Name = "ADC_data7";
			this.ADC_data7.Size = new System.Drawing.Size(45, 20);
			this.ADC_data7.TabIndex = 1;
			this.ADC_data7.Text = "0000";
			// 
			// ADC_data6
			// 
			this.ADC_data6.AutoSize = true;
			this.ADC_data6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data6.Location = new System.Drawing.Point(85, 145);
			this.ADC_data6.Name = "ADC_data6";
			this.ADC_data6.Size = new System.Drawing.Size(45, 20);
			this.ADC_data6.TabIndex = 1;
			this.ADC_data6.Text = "0000";
			// 
			// ADC_data5
			// 
			this.ADC_data5.AutoSize = true;
			this.ADC_data5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data5.Location = new System.Drawing.Point(85, 117);
			this.ADC_data5.Name = "ADC_data5";
			this.ADC_data5.Size = new System.Drawing.Size(45, 20);
			this.ADC_data5.TabIndex = 1;
			this.ADC_data5.Text = "0000";
			// 
			// ADC_data4
			// 
			this.ADC_data4.AutoSize = true;
			this.ADC_data4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data4.Location = new System.Drawing.Point(85, 89);
			this.ADC_data4.Name = "ADC_data4";
			this.ADC_data4.Size = new System.Drawing.Size(45, 20);
			this.ADC_data4.TabIndex = 1;
			this.ADC_data4.Text = "0000";
			// 
			// ADC_data3
			// 
			this.ADC_data3.AutoSize = true;
			this.ADC_data3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data3.Location = new System.Drawing.Point(85, 61);
			this.ADC_data3.Name = "ADC_data3";
			this.ADC_data3.Size = new System.Drawing.Size(45, 20);
			this.ADC_data3.TabIndex = 1;
			this.ADC_data3.Text = "0000";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label2.Location = new System.Drawing.Point(6, 273);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(56, 20);
			this.label2.TabIndex = 0;
			this.label2.Text = "T.sens";
			// 
			// ADC_data2
			// 
			this.ADC_data2.AutoSize = true;
			this.ADC_data2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data2.Location = new System.Drawing.Point(85, 33);
			this.ADC_data2.Name = "ADC_data2";
			this.ADC_data2.Size = new System.Drawing.Size(45, 20);
			this.ADC_data2.TabIndex = 1;
			this.ADC_data2.Text = "0000";
			// 
			// label15
			// 
			this.label15.AutoSize = true;
			this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label15.Location = new System.Drawing.Point(6, 201);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(40, 20);
			this.label15.TabIndex = 0;
			this.label15.Text = "PD7";
			// 
			// label13
			// 
			this.label13.AutoSize = true;
			this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label13.Location = new System.Drawing.Point(6, 173);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(40, 20);
			this.label13.TabIndex = 0;
			this.label13.Text = "PD6";
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label11.Location = new System.Drawing.Point(6, 145);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(40, 20);
			this.label11.TabIndex = 0;
			this.label11.Text = "PD5";
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label9.Location = new System.Drawing.Point(6, 117);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(40, 20);
			this.label9.TabIndex = 0;
			this.label9.Text = "PD4";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label7.Location = new System.Drawing.Point(6, 89);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(40, 20);
			this.label7.TabIndex = 0;
			this.label7.Text = "PD3";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label5.Location = new System.Drawing.Point(6, 61);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(40, 20);
			this.label5.TabIndex = 0;
			this.label5.Text = "PD2";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label3.Location = new System.Drawing.Point(6, 32);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(40, 20);
			this.label3.TabIndex = 0;
			this.label3.Text = "PD1";
			// 
			// ADC_data1
			// 
			this.ADC_data1.AutoSize = true;
			this.ADC_data1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.ADC_data1.Location = new System.Drawing.Point(85, 5);
			this.ADC_data1.Name = "ADC_data1";
			this.ADC_data1.Size = new System.Drawing.Size(45, 20);
			this.ADC_data1.TabIndex = 0;
			this.ADC_data1.Text = "0000";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label1.Location = new System.Drawing.Point(6, 4);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(40, 20);
			this.label1.TabIndex = 0;
			this.label1.Text = "PD0";
			// 
			// tabPage2
			// 
			this.tabPage2.BackColor = System.Drawing.Color.Transparent;
			this.tabPage2.Controls.Add(this.inPortA);
			this.tabPage2.Location = new System.Drawing.Point(4, 22);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage2.Size = new System.Drawing.Size(426, 290);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "Входы";
			// 
			// inPortA
			// 
			this.inPortA.AutoSize = true;
			this.inPortA.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.inPortA.Location = new System.Drawing.Point(6, 3);
			this.inPortA.Name = "inPortA";
			this.inPortA.Size = new System.Drawing.Size(36, 20);
			this.inPortA.TabIndex = 1;
			this.inPortA.Text = "###";
			// 
			// tabPage3
			// 
			this.tabPage3.BackColor = System.Drawing.Color.Transparent;
			this.tabPage3.Controls.Add(this.OUT15);
			this.tabPage3.Controls.Add(this.OUT14);
			this.tabPage3.Controls.Add(this.OUT13);
			this.tabPage3.Controls.Add(this.OUT10);
			this.tabPage3.Controls.Add(this.OUT5);
			this.tabPage3.Controls.Add(this.OUT9);
			this.tabPage3.Controls.Add(this.OUT4);
			this.tabPage3.Controls.Add(this.OUT8);
			this.tabPage3.Controls.Add(this.OUT3);
			this.tabPage3.Controls.Add(this.OUT12);
			this.tabPage3.Controls.Add(this.OUT24);
			this.tabPage3.Controls.Add(this.OUT23);
			this.tabPage3.Controls.Add(this.OUT22);
			this.tabPage3.Controls.Add(this.OUT7);
			this.tabPage3.Controls.Add(this.OUT20);
			this.tabPage3.Controls.Add(this.OUT19);
			this.tabPage3.Controls.Add(this.OUT18);
			this.tabPage3.Controls.Add(this.OUT17);
			this.tabPage3.Controls.Add(this.OUT2);
			this.tabPage3.Controls.Add(this.OUT21);
			this.tabPage3.Controls.Add(this.OUT11);
			this.tabPage3.Controls.Add(this.OUT16);
			this.tabPage3.Controls.Add(this.OUT6);
			this.tabPage3.Controls.Add(this.OUT1);
			this.tabPage3.Location = new System.Drawing.Point(4, 22);
			this.tabPage3.Name = "tabPage3";
			this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage3.Size = new System.Drawing.Size(426, 290);
			this.tabPage3.TabIndex = 2;
			this.tabPage3.Text = "Выходы";
			// 
			// OUT15
			// 
			this.OUT15.AutoSize = true;
			this.OUT15.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT15.Location = new System.Drawing.Point(283, 58);
			this.OUT15.Name = "OUT15";
			this.OUT15.Size = new System.Drawing.Size(46, 24);
			this.OUT15.TabIndex = 0;
			this.OUT15.Text = "15";
			this.OUT15.UseVisualStyleBackColor = true;
			// 
			// OUT14
			// 
			this.OUT14.AutoSize = true;
			this.OUT14.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT14.Location = new System.Drawing.Point(216, 58);
			this.OUT14.Name = "OUT14";
			this.OUT14.Size = new System.Drawing.Size(46, 24);
			this.OUT14.TabIndex = 0;
			this.OUT14.Text = "14";
			this.OUT14.UseVisualStyleBackColor = true;
			// 
			// OUT13
			// 
			this.OUT13.AutoSize = true;
			this.OUT13.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT13.Location = new System.Drawing.Point(149, 58);
			this.OUT13.Name = "OUT13";
			this.OUT13.Size = new System.Drawing.Size(46, 24);
			this.OUT13.TabIndex = 0;
			this.OUT13.Text = "13";
			this.OUT13.UseVisualStyleBackColor = true;
			// 
			// OUT10
			// 
			this.OUT10.AutoSize = true;
			this.OUT10.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT10.Location = new System.Drawing.Point(283, 34);
			this.OUT10.Name = "OUT10";
			this.OUT10.Size = new System.Drawing.Size(46, 24);
			this.OUT10.TabIndex = 0;
			this.OUT10.Text = "10";
			this.OUT10.UseVisualStyleBackColor = true;
			// 
			// OUT5
			// 
			this.OUT5.AutoSize = true;
			this.OUT5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT5.Location = new System.Drawing.Point(283, 10);
			this.OUT5.Name = "OUT5";
			this.OUT5.Size = new System.Drawing.Size(37, 24);
			this.OUT5.TabIndex = 0;
			this.OUT5.Text = "5";
			this.OUT5.UseVisualStyleBackColor = true;
			// 
			// OUT9
			// 
			this.OUT9.AutoSize = true;
			this.OUT9.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT9.Location = new System.Drawing.Point(216, 34);
			this.OUT9.Name = "OUT9";
			this.OUT9.Size = new System.Drawing.Size(37, 24);
			this.OUT9.TabIndex = 0;
			this.OUT9.Text = "9";
			this.OUT9.UseVisualStyleBackColor = true;
			// 
			// OUT4
			// 
			this.OUT4.AutoSize = true;
			this.OUT4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT4.Location = new System.Drawing.Point(216, 10);
			this.OUT4.Name = "OUT4";
			this.OUT4.Size = new System.Drawing.Size(37, 24);
			this.OUT4.TabIndex = 0;
			this.OUT4.Text = "4";
			this.OUT4.UseVisualStyleBackColor = true;
			// 
			// OUT8
			// 
			this.OUT8.AutoSize = true;
			this.OUT8.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT8.Location = new System.Drawing.Point(149, 34);
			this.OUT8.Name = "OUT8";
			this.OUT8.Size = new System.Drawing.Size(37, 24);
			this.OUT8.TabIndex = 0;
			this.OUT8.Text = "8";
			this.OUT8.UseVisualStyleBackColor = true;
			// 
			// OUT3
			// 
			this.OUT3.AutoSize = true;
			this.OUT3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT3.Location = new System.Drawing.Point(149, 10);
			this.OUT3.Name = "OUT3";
			this.OUT3.Size = new System.Drawing.Size(37, 24);
			this.OUT3.TabIndex = 0;
			this.OUT3.Text = "3";
			this.OUT3.UseVisualStyleBackColor = true;
			// 
			// OUT12
			// 
			this.OUT12.AutoSize = true;
			this.OUT12.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT12.Location = new System.Drawing.Point(82, 58);
			this.OUT12.Name = "OUT12";
			this.OUT12.Size = new System.Drawing.Size(46, 24);
			this.OUT12.TabIndex = 0;
			this.OUT12.Text = "12";
			this.OUT12.UseVisualStyleBackColor = true;
			// 
			// OUT24
			// 
			this.OUT24.AutoSize = true;
			this.OUT24.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT24.Location = new System.Drawing.Point(216, 106);
			this.OUT24.Name = "OUT24";
			this.OUT24.Size = new System.Drawing.Size(46, 24);
			this.OUT24.TabIndex = 0;
			this.OUT24.Text = "24";
			this.OUT24.UseVisualStyleBackColor = true;
			// 
			// OUT23
			// 
			this.OUT23.AutoSize = true;
			this.OUT23.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT23.Location = new System.Drawing.Point(149, 106);
			this.OUT23.Name = "OUT23";
			this.OUT23.Size = new System.Drawing.Size(46, 24);
			this.OUT23.TabIndex = 0;
			this.OUT23.Text = "23";
			this.OUT23.UseVisualStyleBackColor = true;
			// 
			// OUT22
			// 
			this.OUT22.AutoSize = true;
			this.OUT22.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT22.Location = new System.Drawing.Point(82, 106);
			this.OUT22.Name = "OUT22";
			this.OUT22.Size = new System.Drawing.Size(46, 24);
			this.OUT22.TabIndex = 0;
			this.OUT22.Text = "22";
			this.OUT22.UseVisualStyleBackColor = true;
			// 
			// OUT7
			// 
			this.OUT7.AutoSize = true;
			this.OUT7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT7.Location = new System.Drawing.Point(82, 34);
			this.OUT7.Name = "OUT7";
			this.OUT7.Size = new System.Drawing.Size(37, 24);
			this.OUT7.TabIndex = 0;
			this.OUT7.Text = "7";
			this.OUT7.UseVisualStyleBackColor = true;
			// 
			// OUT20
			// 
			this.OUT20.AutoSize = true;
			this.OUT20.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT20.Location = new System.Drawing.Point(283, 82);
			this.OUT20.Name = "OUT20";
			this.OUT20.Size = new System.Drawing.Size(46, 24);
			this.OUT20.TabIndex = 0;
			this.OUT20.Text = "20";
			this.OUT20.UseVisualStyleBackColor = true;
			// 
			// OUT19
			// 
			this.OUT19.AutoSize = true;
			this.OUT19.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT19.Location = new System.Drawing.Point(216, 82);
			this.OUT19.Name = "OUT19";
			this.OUT19.Size = new System.Drawing.Size(46, 24);
			this.OUT19.TabIndex = 0;
			this.OUT19.Text = "19";
			this.OUT19.UseVisualStyleBackColor = true;
			// 
			// OUT18
			// 
			this.OUT18.AutoSize = true;
			this.OUT18.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT18.Location = new System.Drawing.Point(149, 82);
			this.OUT18.Name = "OUT18";
			this.OUT18.Size = new System.Drawing.Size(46, 24);
			this.OUT18.TabIndex = 0;
			this.OUT18.Text = "18";
			this.OUT18.UseVisualStyleBackColor = true;
			// 
			// OUT17
			// 
			this.OUT17.AutoSize = true;
			this.OUT17.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT17.Location = new System.Drawing.Point(82, 82);
			this.OUT17.Name = "OUT17";
			this.OUT17.Size = new System.Drawing.Size(46, 24);
			this.OUT17.TabIndex = 0;
			this.OUT17.Text = "17";
			this.OUT17.UseVisualStyleBackColor = true;
			// 
			// OUT2
			// 
			this.OUT2.AutoSize = true;
			this.OUT2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT2.Location = new System.Drawing.Point(82, 10);
			this.OUT2.Name = "OUT2";
			this.OUT2.Size = new System.Drawing.Size(37, 24);
			this.OUT2.TabIndex = 0;
			this.OUT2.Text = "2";
			this.OUT2.UseVisualStyleBackColor = true;
			// 
			// OUT21
			// 
			this.OUT21.AutoSize = true;
			this.OUT21.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT21.Location = new System.Drawing.Point(15, 106);
			this.OUT21.Name = "OUT21";
			this.OUT21.Size = new System.Drawing.Size(46, 24);
			this.OUT21.TabIndex = 0;
			this.OUT21.Text = "21";
			this.OUT21.UseVisualStyleBackColor = true;
			// 
			// OUT11
			// 
			this.OUT11.AutoSize = true;
			this.OUT11.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT11.Location = new System.Drawing.Point(15, 58);
			this.OUT11.Name = "OUT11";
			this.OUT11.Size = new System.Drawing.Size(46, 24);
			this.OUT11.TabIndex = 0;
			this.OUT11.Text = "11";
			this.OUT11.UseVisualStyleBackColor = true;
			// 
			// OUT16
			// 
			this.OUT16.AutoSize = true;
			this.OUT16.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT16.Location = new System.Drawing.Point(15, 82);
			this.OUT16.Name = "OUT16";
			this.OUT16.Size = new System.Drawing.Size(46, 24);
			this.OUT16.TabIndex = 0;
			this.OUT16.Text = "16";
			this.OUT16.UseVisualStyleBackColor = true;
			// 
			// OUT6
			// 
			this.OUT6.AutoSize = true;
			this.OUT6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT6.Location = new System.Drawing.Point(15, 34);
			this.OUT6.Name = "OUT6";
			this.OUT6.Size = new System.Drawing.Size(37, 24);
			this.OUT6.TabIndex = 0;
			this.OUT6.Text = "6";
			this.OUT6.UseVisualStyleBackColor = true;
			// 
			// OUT1
			// 
			this.OUT1.AutoSize = true;
			this.OUT1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.OUT1.Location = new System.Drawing.Point(15, 10);
			this.OUT1.Name = "OUT1";
			this.OUT1.Size = new System.Drawing.Size(37, 24);
			this.OUT1.TabIndex = 0;
			this.OUT1.Text = "1";
			this.OUT1.UseVisualStyleBackColor = true;
			// 
			// tabPage4
			// 
			this.tabPage4.BackColor = System.Drawing.Color.Transparent;
			this.tabPage4.Controls.Add(this.label6);
			this.tabPage4.Controls.Add(this.label4);
			this.tabPage4.Location = new System.Drawing.Point(4, 22);
			this.tabPage4.Name = "tabPage4";
			this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage4.Size = new System.Drawing.Size(426, 290);
			this.tabPage4.TabIndex = 3;
			this.tabPage4.Text = "Частота";
			// 
			// tabPage5
			// 
			this.tabPage5.BackColor = System.Drawing.Color.Transparent;
			this.tabPage5.Controls.Add(this.label25);
			this.tabPage5.Controls.Add(this.label24);
			this.tabPage5.Controls.Add(this.label23);
			this.tabPage5.Controls.Add(this.label22);
			this.tabPage5.Controls.Add(this.label21);
			this.tabPage5.Controls.Add(this.label20);
			this.tabPage5.Controls.Add(this.label19);
			this.tabPage5.Controls.Add(this.label18);
			this.tabPage5.Location = new System.Drawing.Point(4, 22);
			this.tabPage5.Name = "tabPage5";
			this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage5.Size = new System.Drawing.Size(426, 290);
			this.tabPage5.TabIndex = 4;
			this.tabPage5.Text = "DATA";
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label19.Location = new System.Drawing.Point(6, 34);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(27, 20);
			this.label19.TabIndex = 2;
			this.label19.Text = "b1";
			// 
			// label18
			// 
			this.label18.AutoSize = true;
			this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label18.Location = new System.Drawing.Point(6, 14);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(27, 20);
			this.label18.TabIndex = 1;
			this.label18.Text = "b0";
			// 
			// label20
			// 
			this.label20.AutoSize = true;
			this.label20.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label20.Location = new System.Drawing.Point(6, 54);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(27, 20);
			this.label20.TabIndex = 3;
			this.label20.Text = "b2";
			// 
			// label21
			// 
			this.label21.AutoSize = true;
			this.label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label21.Location = new System.Drawing.Point(6, 74);
			this.label21.Name = "label21";
			this.label21.Size = new System.Drawing.Size(27, 20);
			this.label21.TabIndex = 4;
			this.label21.Text = "b3";
			// 
			// label22
			// 
			this.label22.AutoSize = true;
			this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label22.Location = new System.Drawing.Point(6, 94);
			this.label22.Name = "label22";
			this.label22.Size = new System.Drawing.Size(27, 20);
			this.label22.TabIndex = 5;
			this.label22.Text = "b4";
			// 
			// label23
			// 
			this.label23.AutoSize = true;
			this.label23.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label23.Location = new System.Drawing.Point(6, 114);
			this.label23.Name = "label23";
			this.label23.Size = new System.Drawing.Size(27, 20);
			this.label23.TabIndex = 6;
			this.label23.Text = "b5";
			// 
			// label24
			// 
			this.label24.AutoSize = true;
			this.label24.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label24.Location = new System.Drawing.Point(6, 134);
			this.label24.Name = "label24";
			this.label24.Size = new System.Drawing.Size(27, 20);
			this.label24.TabIndex = 7;
			this.label24.Text = "b6";
			// 
			// label25
			// 
			this.label25.AutoSize = true;
			this.label25.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label25.Location = new System.Drawing.Point(6, 154);
			this.label25.Name = "label25";
			this.label25.Size = new System.Drawing.Size(27, 20);
			this.label25.TabIndex = 8;
			this.label25.Text = "b7";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label4.Location = new System.Drawing.Point(6, 20);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(36, 20);
			this.label4.TabIndex = 2;
			this.label4.Text = "###";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label6.Location = new System.Drawing.Point(6, 49);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(36, 20);
			this.label6.TabIndex = 3;
			this.label6.Text = "###";
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(440, 399);
			this.Controls.Add(this.Data_frame);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.Name = "Form1";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "RS485 Монитор";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.Data_frame.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabPage1.PerformLayout();
			this.tabPage2.ResumeLayout(false);
			this.tabPage2.PerformLayout();
			this.tabPage3.ResumeLayout(false);
			this.tabPage3.PerformLayout();
			this.tabPage4.ResumeLayout(false);
			this.tabPage4.PerformLayout();
			this.tabPage5.ResumeLayout(false);
			this.tabPage5.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.ComboBox ComSel;
		private System.Windows.Forms.Button BT_COM_open;
		private System.Windows.Forms.Timer timer1;
		private System.IO.Ports.SerialPort sp;
		private System.Windows.Forms.GroupBox Data_frame;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.TabPage tabPage3;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label ADC_data1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label T_sens;
		private System.Windows.Forms.Label ADC_data8;
		private System.Windows.Forms.Label ADC_data7;
		private System.Windows.Forms.Label ADC_data6;
		private System.Windows.Forms.Label ADC_data5;
		private System.Windows.Forms.Label ADC_data4;
		private System.Windows.Forms.Label ADC_data3;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label ADC_data2;
		private System.Windows.Forms.CheckBox OUT1;
		private System.Windows.Forms.CheckBox OUT13;
		private System.Windows.Forms.CheckBox OUT8;
		private System.Windows.Forms.CheckBox OUT3;
		private System.Windows.Forms.CheckBox OUT12;
		private System.Windows.Forms.CheckBox OUT7;
		private System.Windows.Forms.CheckBox OUT2;
		private System.Windows.Forms.CheckBox OUT11;
		private System.Windows.Forms.CheckBox OUT6;
		private System.Windows.Forms.Label inPortA;
		private System.Windows.Forms.CheckBox OUT15;
		private System.Windows.Forms.CheckBox OUT14;
		private System.Windows.Forms.CheckBox OUT10;
		private System.Windows.Forms.CheckBox OUT5;
		private System.Windows.Forms.CheckBox OUT9;
		private System.Windows.Forms.CheckBox OUT4;
		private System.Windows.Forms.CheckBox OUT24;
		private System.Windows.Forms.CheckBox OUT23;
		private System.Windows.Forms.CheckBox OUT22;
		private System.Windows.Forms.CheckBox OUT20;
		private System.Windows.Forms.CheckBox OUT19;
		private System.Windows.Forms.CheckBox OUT18;
		private System.Windows.Forms.CheckBox OUT17;
		private System.Windows.Forms.CheckBox OUT21;
		private System.Windows.Forms.CheckBox OUT16;
		private System.Windows.Forms.TabPage tabPage4;
		private System.Windows.Forms.TabPage tabPage5;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.Label label25;
		private System.Windows.Forms.Label label24;
		private System.Windows.Forms.Label label23;
		private System.Windows.Forms.Label label22;
		private System.Windows.Forms.Label label21;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label4;
	}
}

