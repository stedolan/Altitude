﻿namespace AltitudeUI
{
    partial class ConsoleForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.RTxtBoxConsole = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // RTxtBoxConsole
            // 
            this.RTxtBoxConsole.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RTxtBoxConsole.Location = new System.Drawing.Point(0, 0);
            this.RTxtBoxConsole.Name = "RTxtBoxConsole";
            this.RTxtBoxConsole.Size = new System.Drawing.Size(588, 294);
            this.RTxtBoxConsole.TabIndex = 0;
            this.RTxtBoxConsole.Text = "";
            this.RTxtBoxConsole.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RTxtBoxConsole_KeyPress);
            // 
            // ConsoleForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(588, 294);
            this.Controls.Add(this.RTxtBoxConsole);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConsoleForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "IO Console";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox RTxtBoxConsole;
    }
}