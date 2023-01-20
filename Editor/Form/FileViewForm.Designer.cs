namespace Editor
{
	partial class FileViewForm
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
            this.NytTreeView = new Editor.Nyt.NytTreeView();
            this.SuspendLayout();
            // 
            // NytTreeView
            // 
            this.NytTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.NytTreeView.LabelEdit = true;
            this.NytTreeView.Location = new System.Drawing.Point(0, 0);
            this.NytTreeView.Name = "NytTreeView";
            this.NytTreeView.ShowNodeToolTips = true;
            this.NytTreeView.Size = new System.Drawing.Size(207, 561);
            this.NytTreeView.TabIndex = 0;
            // 
            // FileViewForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(207, 561);
            this.Controls.Add(this.NytTreeView);
            this.Name = "FileViewForm";
            this.Text = "NytTreeView";
            this.ResumeLayout(false);

		}

		#endregion

		private Nyt.NytTreeView NytTreeView;
	}
}