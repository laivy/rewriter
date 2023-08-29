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
            this.components = new System.ComponentModel.Container();
            this.Menu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.AddNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.ModifyNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.DeleteNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.NytTreeView = new Editor.Nyt.NytTreeView();
            this.Menu.SuspendLayout();
            this.SuspendLayout();
            // 
            // Menu
            // 
            this.Menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.AddNodeMenu,
            this.ModifyNodeMenu,
            this.DeleteNodeMenu});
            this.Menu.Name = "contextMenuStrip1";
            this.Menu.Size = new System.Drawing.Size(99, 70);
            // 
            // AddNodeMenu
            // 
            this.AddNodeMenu.Name = "AddNodeMenu";
            this.AddNodeMenu.Size = new System.Drawing.Size(98, 22);
            this.AddNodeMenu.Text = "추가";
            this.AddNodeMenu.Click += new System.EventHandler(this.OnAddNodeMenuClick);
            // 
            // ModifyNodeMenu
            // 
            this.ModifyNodeMenu.Name = "ModifyNodeMenu";
            this.ModifyNodeMenu.Size = new System.Drawing.Size(98, 22);
            this.ModifyNodeMenu.Text = "수정";
            this.ModifyNodeMenu.Click += new System.EventHandler(this.OnModifyNodeMenuClick);
            // 
            // DeleteNodeMenu
            // 
            this.DeleteNodeMenu.Name = "DeleteNodeMenu";
            this.DeleteNodeMenu.Size = new System.Drawing.Size(98, 22);
            this.DeleteNodeMenu.Text = "삭제";
            this.DeleteNodeMenu.Click += new System.EventHandler(this.OnDeleteNodeMenuClick);
            // 
            // NytTreeView
            // 
            this.NytTreeView.AllowDrop = true;
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
            this.ContextMenuStrip = this.Menu;
            this.Controls.Add(this.NytTreeView);
            this.Name = "FileViewForm";
            this.Text = "NytTreeView";
            this.Menu.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		#endregion

		private Nyt.NytTreeView NytTreeView;
		private System.Windows.Forms.ContextMenuStrip Menu;
		private System.Windows.Forms.ToolStripMenuItem AddNodeMenu;
		private System.Windows.Forms.ToolStripMenuItem ModifyNodeMenu;
		private System.Windows.Forms.ToolStripMenuItem DeleteNodeMenu;
	}
}