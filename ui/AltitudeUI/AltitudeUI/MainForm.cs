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
using System.Text.RegularExpressions;

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

            //this.errorsform.add_error(new VMError(223, false, new loc_info(), "test!")); // test error msg
        }

        void altitude_vm_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            string reg_err = @"^\[(?<errno>[0-9]+)\]\s*";
            string reg_loc = "@(?<filename>[0-9a-z._]+):(?<lineno>[0-9]+):(?<byteoffset>[0-9]+|-1)";
            string reg_message = @"(?<message>.*)";

            string output = e.Data;

            Match is_err = Regex.Match(output, reg_err, RegexOptions.Compiled);
            if (is_err.Success) // is an error message
            {
                // we have an error, ladies and gentlemen
                // get the error number
                VMError err;

                int errno = Int32.Parse(is_err.Groups["errno"].Value);

                loc_info loc;
                bool hasLoc;
                Match has_loc = Regex.Match(output, reg_loc, RegexOptions.Compiled | RegexOptions.IgnoreCase);
                // see if there is also location information
                if (has_loc.Success)
                {
                    hasLoc = true;
                    loc.filename = has_loc.Groups["filename"].Value;
                    loc.lineno = Int32.Parse(has_loc.Groups["lineno"].Value);
                    loc.bytecodepos = Int32.Parse(has_loc.Groups["byteoffset"].Value);
                }
                else
                {
                    hasLoc = false;
                    loc.filename = "";
                    loc.lineno = 0;
                    loc.bytecodepos = -1;
                }

                // and finally get the message
                string message;
                Match has_message = Regex.Match(output, reg_err + "(" + reg_loc + ")?" + reg_message, RegexOptions.Compiled | RegexOptions.IgnoreCase);
                if (has_message.Success)
                {
                    message = has_message.Groups["message"].Value;
                }
                else
                {
                    message = "No message, which is odd.";
                }
                err = new VMError(errno, hasLoc, loc, message);
                this.errorsform.add_error(err);
            }
            else // is user program message
            {
                this.consoleform.AppendStdOut(output);
            }
        }

        void consoleform_IOInputReceived(object sender, IOEventArgs e)
        {
            if (this.altitude_vm != null)
            {
                this.altitude_vm.StandardInput.WriteLine(e.Message);
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            // GO FORWARD
            if (this.altitude_vm != null)
            {
                this.altitude_vm.StandardInput.Write("run\n");
                this.altitude_vm.StandardInput.Flush();
            }
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            // GO BACK
            if (this.altitude_vm != null)
            {
                this.altitude_vm.StandardInput.Write("runback\n");
                this.altitude_vm.StandardInput.Flush();
            }
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            //// SET A BREAKPOINT
            if (this.altitude_vm != null)
            {
                int lineno = this.CodeEditorBox.GetLineNo();
                this.altitude_vm.StandardInput.Write("set " + lineno + "\n");
                this.altitude_vm.StandardInput.Flush();
            }
        }

        private void toolStripButton4_Click(object sender, EventArgs e)
        {
            //// UNSET A BREAKPOINT
            if (this.altitude_vm != null)
            {
                int lineno = this.CodeEditorBox.GetLineNo();
                this.altitude_vm.StandardInput.Write("unset " + lineno + "\n");
                this.altitude_vm.StandardInput.Flush();
            }
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
            this.altitude_vm.StartInfo.FileName = "../../../../vm/altitude"; // the altitude binary
            this.altitude_vm.StartInfo.Arguments = this.currLoaded; // the c file
            this.altitude_vm.StartInfo.UseShellExecute = true; // the rest of this can be ignored really
            // start altitude!
            try
            {
                this.altitude_vm.Start();
                // set up data stream handles
                this.altitude_vm.OutputDataReceived += new DataReceivedEventHandler(altitude_vm_OutputDataReceived);
                this.altitude_vm.Exited += new EventHandler(altitude_vm_Exited);
            }
            catch (Win32Exception w32e)
            {
                MessageBox.Show("Couldn't load the Altitude VM!\n" + w32e.Message, "Fatal Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }
        }

        void altitude_vm_Exited(object sender, EventArgs e)
        {
            // the VM died
            this.altitude_vm = null;
        }

        private void MainForm_Move(object sender, EventArgs e)
        {
            if (this.consoleform != null && this.errorsform != null)
            {
                this.consoleform.Location = new Point(this.Location.X, this.Location.Y + this.Height + 5);

                this.errorsform.Location = new Point(this.Location.X + this.Width + 5, this.Location.Y);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.altitude_vm != null)
            {
                this.altitude_vm.Kill();
            }
        }
    }
}
