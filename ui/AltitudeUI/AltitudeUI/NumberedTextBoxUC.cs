using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace NumberedTextBox
{
    public partial class NumberedTextBoxUC : UserControl
    {

        public NumberedTextBoxUC()
        {
            InitializeComponent();

            //numberLabel.Font = new Font(richTextBox1.Font.FontFamily, richTextBox1.Font.Size + 1.019f);
        }

        public void Write(string text)
        {
            this.richTextBox1.Text = text;
        }

        public int GetLineNo()
        {
            int start = this.richTextBox1.SelectionStart;
            int count = 0;

            for (int currpos = 0; currpos <= start && currpos != -1; count++)
            {
                currpos = this.richTextBox1.Text.IndexOf('\n', currpos) + 1;
            }

            return count;
        }

        private void updateNumberLabel()
        {
            //we get index of first visible char and number of first visible line
            Point pos = new Point(0, 0);
            int firstIndex = richTextBox1.GetCharIndexFromPosition(pos);
            int firstLine = richTextBox1.GetLineFromCharIndex(firstIndex);

            //now we get index of last visible char and number of last visible line
            pos.X = ClientRectangle.Width;
            pos.Y = ClientRectangle.Height;
            int lastIndex = richTextBox1.GetCharIndexFromPosition(pos);
            int lastLine = richTextBox1.GetLineFromCharIndex(lastIndex);

            //this is point position of last visible char, we'll use its Y value for calculating numberLabel size
            pos = richTextBox1.GetPositionFromCharIndex(lastIndex);

           
            //finally, renumber label
            numberLabel.Text = "";
            for (int i = firstLine; i <= lastLine + 1; i++)
            {
                numberLabel.Text += i + 1 + "\n";
            }

        }


        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            updateNumberLabel();
        }

        private void richTextBox1_VScroll(object sender, EventArgs e)
        {
            //move location of numberLabel for amount of pixels caused by scrollbar
            int d = richTextBox1.GetPositionFromCharIndex(0).Y % (richTextBox1.Font.Height + 1);
            numberLabel.Location = new Point(0, d);

            updateNumberLabel();
        }

        private void richTextBox1_Resize(object sender, EventArgs e)
        {
            richTextBox1_VScroll(null, null);
        }

        private void richTextBox1_FontChanged(object sender, EventArgs e)
        {
            updateNumberLabel();
            richTextBox1_VScroll(null, null);
        }

             


    }
}
