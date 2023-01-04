using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Editor
{
	public partial class MainForm : Form
	{
		private TreeView fileTreeView;

		public MainForm()
		{
			InitializeComponent();
			Initialize();
		}

		private void Initialize()
		{
			fileTreeView = Controls.Find("FileTreeView", false)[0] as TreeView;
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
			}
			Console.WriteLine(filePath);
		}

		private void Menu_Edit_Add_Click(object sender, EventArgs e)
		{
			AddNodeForm nodeAddForm = new AddNodeForm();
			nodeAddForm.OnAddNode += NodeAddForm_OnAddNode;
			nodeAddForm.ShowDialog();
		}

		private void NodeAddForm_OnAddNode(object sender, EventArgs e)
		{
			Tuple<int, string, string> tuple = (Tuple<int, string, string>)sender;
			TreeNode node = new TreeNode(tuple.Item2);

			if (fileTreeView.SelectedNode == null)
				fileTreeView.Nodes.Add(node);
			else
				fileTreeView.SelectedNode.Nodes.Add(node);
		}

		private void FileTreeView_AfterSelect(object sender, TreeViewEventArgs e)
		{
			Console.WriteLine(fileTreeView.SelectedNode.ToString());
		}
	}
}
