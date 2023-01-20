using Editor.Nyt;
using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class FileViewForm : Form
	{
		private string _filePath;

		public FileViewForm(string filePath)
		{
			InitializeComponent();
			_filePath = Text = filePath;
		}

		public void OnAddNode(object sender, EventArgs e)
		{
			NytTreeView.Add(new NytTreeNode((NytTreeNodeInfo)sender));
		}

		public void SaveFile(string filePath)
		{
			_filePath = filePath;
			SaveFile();
		}

		public void SaveFile()
		{
			NytTreeView.Save(_filePath);
			MessageBox.Show("저장 완료");
		}

		public void LoadFile()
		{
			NytTreeView.Load(_filePath);
		}

		public NytTreeNode GetSelectedNode()
		{
			if (NytTreeView.SelectedNode != null)
				return (NytTreeNode)NytTreeView.SelectedNode;
			return null;
		}
	}
}
