namespace AltitudeUI
{
    partial class CodeEditorBox
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.RTxtBoxCode = new System.Windows.Forms.RichTextBox();
            this.RTxtBoxLineNo = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // RTxtBoxCode
            // 
            this.RTxtBoxCode.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RTxtBoxCode.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.RTxtBoxCode.Location = new System.Drawing.Point(42, 0);
            this.RTxtBoxCode.Name = "RTxtBoxCode";
            this.RTxtBoxCode.Size = new System.Drawing.Size(542, 457);
            this.RTxtBoxCode.TabIndex = 1;
            this.RTxtBoxCode.Text = "";
            this.RTxtBoxCode.TextChanged += new System.EventHandler(this.RTxtBoxCode_TextChanged);
            // 
            // RTxtBoxLineNo
            // 
            this.RTxtBoxLineNo.BackColor = System.Drawing.Color.LightSteelBlue;
            this.RTxtBoxLineNo.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.RTxtBoxLineNo.Dock = System.Windows.Forms.DockStyle.Left;
            this.RTxtBoxLineNo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.RTxtBoxLineNo.Location = new System.Drawing.Point(0, 0);
            this.RTxtBoxLineNo.Name = "RTxtBoxLineNo";
            this.RTxtBoxLineNo.ReadOnly = true;
            this.RTxtBoxLineNo.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.RTxtBoxLineNo.Size = new System.Drawing.Size(42, 457);
            this.RTxtBoxLineNo.TabIndex = 2;
            this.RTxtBoxLineNo.TabStop = false;
            this.RTxtBoxLineNo.Text = "";
            this.RTxtBoxLineNo.WordWrap = false;
            // 
            // CodeEditorBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.RTxtBoxCode);
            this.Controls.Add(this.RTxtBoxLineNo);
            this.Name = "CodeEditorBox";
            this.Size = new System.Drawing.Size(584, 457);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox RTxtBoxCode;
        private System.Windows.Forms.RichTextBox RTxtBoxLineNo;
    }
}
