using Editor.Nyt;
using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class MainForm : Form
	{
		private FileViewForm _treeViewForm;	// 활성화 상태인 트리뷰폼

		public MainForm()
		{
			InitializeComponent();
		}

		private void OnActivated(object sender, EventArgs e)
		{
			_treeViewForm = (FileViewForm)sender;
		}

		private void OnNewFileMenuClick(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "저장할 파일 위치를 선택해주세요."
			};

			DialogResult dialogResult = saveFileDialog.ShowDialog();
			if (dialogResult != DialogResult.Cancel && dialogResult != DialogResult.OK)
			{
				MessageBox.Show("해당 경로에 저장할 수 없습니다.");
				return;
			}

			_treeViewForm = new FileViewForm(saveFileDialog.FileName);
			_treeViewForm.Activated += OnActivated;
			_treeViewForm.MdiParent = this;
			_treeViewForm.Show();
		}

		private void OnFileOpenMenuClick(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "불러올 파일을 선택해주세요."
			};

			DialogResult dialogResult = openFileDialog.ShowDialog();
			if (dialogResult != DialogResult.Cancel && dialogResult != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}

			_treeViewForm = new FileViewForm(openFileDialog.FileName);
			_treeViewForm.Activated += OnActivated;
			_treeViewForm.MdiParent = this;
			_treeViewForm.LoadFile();
			_treeViewForm.Show();
		}

		private void OnFileSaveMenuClick(object sender, EventArgs e)
		{
			_treeViewForm?.SaveFile();
		}

		private void OnFileSaveAsMenuClick(object sender, EventArgs e)
		{
			if (_treeViewForm == null)
				return;

			SaveFileDialog saveFileDialog = new SaveFileDialog();
			saveFileDialog.Filter = "nyt files (*.nyt)|*.nyt";
			saveFileDialog.Title = "저장할 파일 위치를 선택해주세요.";
			if (saveFileDialog.ShowDialog() != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}
			_treeViewForm.SaveFile(saveFileDialog.FileName);
		}

		private void OnAddNodeMenuClick(object sender, EventArgs e)
		{
			if (_treeViewForm == null)
				return;

			NodeForm nodeAddForm = new NodeForm();
			nodeAddForm._OnAddNode += _treeViewForm.OnAddNode;
			nodeAddForm.ShowDialog();
		}

		private void OnModifyNodeMenuClick(object sender, EventArgs e)
		{
			if (_treeViewForm == null)
				return;

			NytTreeNode node = _treeViewForm.GetSelectedNode();
			if (node == null)
				return;

			NodeForm nodeAddForm = new NodeForm(node);
			nodeAddForm._OnAddNode += _treeViewForm.OnAddNode;
			nodeAddForm.ShowDialog();
		}
	}
}
