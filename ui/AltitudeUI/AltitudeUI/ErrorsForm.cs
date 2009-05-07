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
    public partial class ErrorsForm : Form
    {
        private delegate void AddItem(ListViewItem item);

        public ErrorsForm()
        {
            InitializeComponent();
        }

        public void add_error(VMError err)
        {
            ListViewItem item = new ListViewItem(err.Errno.ToString());
            if (err.HasLoc)
            {
                item.SubItems.Add("@" + err.Location.filename + ":" + err.Location.lineno);
            }
            else
            {
                item.SubItems.Add("N/A");
            }
            item.SubItems.Add(err.Message);

            if (err.Errno >= 100 && err.Errno <= 199)
            {
                item.ForeColor = Color.Red;
            }
            else if (err.Errno >= 200 && err.Errno <= 299)
            {
                item.ForeColor = Color.DarkRed;
                item.BackColor = Color.Yellow;
            }

            this.listview_add(item);
        }

        private void listview_add(ListViewItem item)
        {
            if (this.listViewErrors.InvokeRequired)
            {
                this.listViewErrors.Invoke(new AddItem(listview_add), item);
            }
            else
            {
                this.listViewErrors.Items.Add(item);
            }
        }
    }

    public struct loc_info
    {
        public string filename;
        public int lineno;
        public int bytecodepos;
    }

    public class VMError
    {
        int errno;
        public int Errno { get { return errno; } }

        bool hasLoc;
        public bool HasLoc { get { return hasLoc; } }

        loc_info location;
        public loc_info Location { get { return location; } }

        string message;
        public string Message { get { return message; } }

        public VMError(int errno, bool hasLoc, loc_info location, string message)
        {
            this.errno = errno;
            this.hasLoc = hasLoc;
            this.location = location;
            this.message = message;
        }

        public override string ToString()
        {
            string ret = "[" + this.errno + "] ";
            if (this.hasLoc)
            {
                ret += "@" + this.location.filename + ":" + this.location.lineno + ":" + this.location.bytecodepos + ": ";
            }
            ret += this.message;

            return ret;
        }
    }
}
