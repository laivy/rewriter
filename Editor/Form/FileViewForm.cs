using Editor.Nyt;
using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class FileViewForm : Form
	{
		private string _filePath; // 이 폼이 보여주고 있는 파일
		private bool _isModified; // 파일이 변경되었는지

		public FileViewForm(string filePath)
		{
			InitializeComponent();
			_filePath = Text = filePath;
			_isModified = false;

			NytTreeView.NodeMouseClick += OnTreeViewRightClick;
		}

		private void OnAddNodeMenuClick(object sender, EventArgs e)
		{
			NodeEditForm nodeEditForm = new NodeEditForm(this);
			nodeEditForm.ShowDialog();
		}

		private void OnModifyNodeMenuClick(object sender, EventArgs e)
		{
			NytNode node = GetSelectedNode();
			if (node == null)
				return;

			NodeEditForm nodeEditForm = new NodeEditForm(this);
			nodeEditForm.SetNode(node);
			nodeEditForm.ShowDialog();
		}

		private void OnDeleteNodeMenuClick(object sender, EventArgs e)
		{
			GetSelectedNode()?.Remove();
		}

		private void OnTreeViewRightClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				NytTreeView.SelectedNode = e.Node;
			}
		}

		private NytNode GetSelectedNode()
		{
			if (NytTreeView.SelectedNode != null)
				return (NytNode)NytTreeView.SelectedNode;
			return null;
		}

		public void SaveAsFile(string filePath)
		{
			_filePath = filePath;
			SaveFile();
			SetIsModified(false);
		}

		public void SaveFile()
		{
			NytTreeView.Save(_filePath);
			SetIsModified(false);
			MessageBox.Show("저장 완료");
		}

		public void LoadFile()
		{
			NytTreeView.Load(_filePath);
		}

		public NytTreeView GetNytTreeView()
		{
			return NytTreeView;
		}

		public void SetIsModified(bool isModified)
		{
			_isModified = isModified;
			if (_isModified)
				Text = _filePath + " *";
			else
				Text = _filePath;
		}
	}
}
