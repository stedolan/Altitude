using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AltitudeUI
{
    public partial class MainForm : Form
    {
        ConsoleForm consoleform;

        public MainForm()
        {
            InitializeComponent();
            this.Location = new Point(40, 40);

            /* consoleform stuff */
            this.consoleform = new ConsoleForm();
            this.consoleform.Show();
            this.consoleform.Location = new Point(this.Location.X, this.Location.Y + this.Height + 5);
            this.consoleform.Width = this.Width;
            this.consoleform.IOInputReceived += new IOInputHandler(consoleform_IOInputReceived);
        }

        void consoleform_IOInputReceived(object sender, IOEventArgs e)
        {
            MessageBox.Show(e.Message);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
