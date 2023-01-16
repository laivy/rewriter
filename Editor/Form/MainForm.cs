using System;
using System.IO;
using System.Windows.Forms;
using Editor.Nyt;

namespace Editor
{
	public partial class MainForm : Form
	{
		private NytTreeView _treeView;

		public MainForm()
		{
			InitializeComponent();
			Initialize();
		}

		private void Initialize()
		{
			NytTreeViewForm fileViewForm = new NytTreeViewForm();
			fileViewForm.MdiParent = this;
			fileViewForm.Show();
			_treeView = fileViewForm.Controls.Find("NytTreeView", false)[0] as NytTreeView;
		}

		private void Menu_File_Open_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			//fileDialog.Filter = "nyt files (*.nyt)|*.nyt";
			openFileDialog.Filter = "All files (*.*)|*.*";
			openFileDialog.Title = "불러올 파일을 선택해주세요.";

			string filePath = string.Empty;
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				filePath = openFileDialog.FileName;
			}
			Console.WriteLine(filePath);
		}

		private void Menu_File_SaveAs_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog();
			saveFileDialog.Filter = "nyt files (*.nyt)|*.nyt";
			saveFileDialog.Title = "저장할 파일 위치를 선택해주세요.";

			string filePath = string.Empty;
			if (saveFileDialog.ShowDialog() == DialogResult.OK)
			{
				filePath = saveFileDialog.FileName;
				Save(filePath);
			}
		}

		private void Menu_Edit_Add_Click(object sender, EventArgs e)
		{
			AddNodeForm nodeAddForm = new AddNodeForm();
			nodeAddForm.OnAddNode += NodeAddForm_OnAddNode;
			nodeAddForm.ShowDialog();
		}

		private void NodeAddForm_OnAddNode(object sender, EventArgs e)
		{
			// 노드 추가
			NytTreeNode node = new NytTreeNode((NytTreeNodeInfo)sender);
			if (_treeView.SelectedNode == null)
				_treeView.Nodes.Add(node);
			else
				_treeView.SelectedNode.Nodes.Add(node);
			_treeView.SelectedNode = node;
		}

		private void FileTreeView_AfterSelect(object sender, TreeViewEventArgs e)
		{
			
		}

		private void Save(string filePath)
		{
			//FileStream fileStream = new FileStream(filePath, FileMode.OpenOrCreate);
			//BinaryWriter binaryWriter = new BinaryWriter(fileStream);
			_treeView.Save();
		}

		private void Menu_File_Save_Click(object sender, EventArgs e)
		{
			_treeView.Save();
		}
	}
}
