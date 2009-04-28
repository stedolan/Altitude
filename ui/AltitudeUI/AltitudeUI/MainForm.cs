using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace AltitudeUI
{
    public partial class MainForm : Form
    {
        ConsoleForm consoleform;
        ErrorsForm errorsform;

        private string currLoaded = ""; // the currently loaded c file
        private Process altitude_vm;

        public MainForm()
        {
            InitializeComponent();
            this.Location = new Point(40, 40);

            /* consoleform stuff */
            this.consoleform = new ConsoleForm();
            this.consoleform.Show();
            this.consoleform.IOInputReceived += new IOInputHandler(consoleform_IOInputReceived);
            this.consoleform.Location = new Point(this.Location.X, this.Location.Y + this.Height + 5);
            this.consoleform.Width = this.Width;

            /* errorsform stuff */
            this.errorsform = new ErrorsForm();
            this.errorsform.Show();
            this.errorsform.Location = new Point(this.Location.X + this.Width + 5, this.Location.Y);
            this.errorsform.Height = this.Height + 5 + this.consoleform.Height;
        }

        void altitude_vm_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            throw new NotImplementedException();
        }

        void consoleform_IOInputReceived(object sender, IOEventArgs e)
        {
            MessageBox.Show(e.Message);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            // GO FORWARD
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            // GO BACK
        }

        private void cSourceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Load a C source file
            this.openFileDialog.ShowDialog();
            this.currLoaded = this.openFileDialog.FileName;
            StreamReader s = new StreamReader(this.currLoaded);
            this.CodeEditorBox.Write(s.ReadToEnd());

            // call altitude with this file
            this.altitude_vm = new Process();
            this.altitude_vm.StartInfo.FileName = "altitude"; // the altitude binary
            this.altitude_vm.StartInfo.Arguments = this.currLoaded; // the c file
            this.altitude_vm.StartInfo.UseShellExecute = true; // the rest of this can be ignored really
            // start altitude!
            try
            {
                this.altitude_vm.Start();
            }
            catch (Win32Exception w32e)
            {
                MessageBox.Show("Couldn't load the Altitude VM!", "Fatal Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }

            // set up data stream handles
            this.altitude_vm.OutputDataReceived += new DataReceivedEventHandler(altitude_vm_OutputDataReceived);
        }

        private void MainForm_Move(object sender, EventArgs e)
        {
            if (this.consoleform != null && this.errorsform != null)
            {
                this.consoleform.Location = new Point(this.Location.X, this.Location.Y + this.Height + 5);

                this.errorsform.Location = new Point(this.Location.X + this.Width + 5, this.Location.Y);
            }
        }
    }
}
