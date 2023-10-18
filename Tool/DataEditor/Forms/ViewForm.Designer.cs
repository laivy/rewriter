namespace DataEditor
{
	partial class ViewForm
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
            this.MenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.AddNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.ModifyNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.DeleteNodeMenu = new System.Windows.Forms.ToolStripMenuItem();
            this._treeView = new System.Windows.Forms.TreeView();
            this.MenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // MenuStrip
            // 
            this.MenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.AddNodeMenu,
            this.ModifyNodeMenu,
            this.DeleteNodeMenu});
            this.MenuStrip.Name = "contextMenuStrip1";
            this.MenuStrip.Size = new System.Drawing.Size(99, 70);
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
            // _treeView
            // 
            this._treeView.AllowDrop = true;
            this._treeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this._treeView.LabelEdit = true;
            this._treeView.Location = new System.Drawing.Point(0, 0);
            this._treeView.Name = "_treeView";
            this._treeView.ShowNodeToolTips = true;
            this._treeView.Size = new System.Drawing.Size(284, 561);
            this._treeView.TabIndex = 0;
            // 
            // ViewForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 561);
            this.ContextMenuStrip = this.MenuStrip;
            this.Controls.Add(this._treeView);
            this.KeyPreview = true;
            this.Name = "ViewForm";
            this.Text = "FileView";
            this.MenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView _treeView;
		private System.Windows.Forms.ContextMenuStrip MenuStrip;
		private System.Windows.Forms.ToolStripMenuItem AddNodeMenu;
		private System.Windows.Forms.ToolStripMenuItem ModifyNodeMenu;
		private System.Windows.Forms.ToolStripMenuItem DeleteNodeMenu;
    }
}