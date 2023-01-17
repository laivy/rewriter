using Editor.Nyt;
using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class NytTreeViewForm : Form
	{
		private NytTreeView _treeView;
		private string _filePath;

		public NytTreeViewForm(string filePath)
		{
			InitializeComponent();
			_treeView = Controls.Find("NytTreeView", false)[0] as NytTreeView;
			_filePath = Text = filePath;
		}

		public void OnAddNode(object sender, EventArgs e)
		{
			_treeView.Add(new NytTreeNode((NytTreeNodeInfo)sender));
		}

		public void SaveFile(string filePath)
		{
			_filePath = filePath;
			SaveFile();
		}

		public void SaveFile()
		{
			_treeView.Save(_filePath);
			MessageBox.Show("저장 완료");
		}

		public void LoadFile()
		{
			_treeView.Load(_filePath);
		}
	}
}
