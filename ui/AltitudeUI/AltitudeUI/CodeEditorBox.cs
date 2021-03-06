﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AltitudeUI
{
    public partial class CodeEditorBox : UserControl
    {
        public CodeEditorBox()
        {
            InitializeComponent();
            this.RTxtBoxLineNo.Font = this.RTxtBoxCode.Font;
        }

        public void Write(string text)
        {
            this.RTxtBoxCode.Text = text;
        }

        public string Read() 
        {
            return this.RTxtBoxCode.Text;
        }

        private void RTxtBoxCode_TextChanged(object sender, EventArgs e)
        {
            this.RTxtBoxLineNo.Text = "";
            for (int i = 0; i < this.RTxtBoxCode.Lines.Count(); ++i)
            {
                this.RTxtBoxLineNo.Text += (i+1) + "\n";
            }
        }
    }
}
