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
    public delegate void IOInputHandler(object sender, IOEventArgs e);

    public partial class ConsoleForm : Form
    {
        public event IOInputHandler IOInputReceived;

        private List<int> static_lines = new List<int>();

        public ConsoleForm()
        {
            InitializeComponent();
        }

        public void AppendStdOut(string text)
        {
            this.static_lines.Add(this.RTxtBoxConsole.Lines.Length);
            this.RTxtBoxConsole.Text += "\r\n" + text;
        }

        private void RTxtBoxConsole_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == '\r' || e.KeyChar == '\n')
            {
                string entered_text = this.RTxtBoxConsole.Lines[this.RTxtBoxConsole.Lines.Length - 2];

                this.IOInputReceived(this, new IOEventArgs(entered_text));
            }
        }

        
    }

    public class IOEventArgs : EventArgs
    {
        private string message;
        public string Message { get { return message; } }

        public IOEventArgs(string message)
        {
            this.message = message;
        }
    }
}
