using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class MainForm : Form
	{
		private NytTreeViewForm _treeViewForm;	// 활성화 상태인 트리뷰폼

		public MainForm()
		{
			InitializeComponent();
		}

		private void Menu_File_New_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog();
			saveFileDialog.Filter = "nyt files (*.nyt)|*.nyt";
			saveFileDialog.Title = "저장할 파일 위치를 선택해주세요.";
			if (saveFileDialog.ShowDialog() != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}
			_treeViewForm = new NytTreeViewForm(saveFileDialog.FileName);
			_treeViewForm.Activated += TreeViewForm_Activated;
			_treeViewForm.MdiParent = this;
			_treeViewForm.Show();
		}

		private void Menu_File_Open_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "불러올 파일을 선택해주세요."
			};
			if (openFileDialog.ShowDialog() != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}

			_treeViewForm = new NytTreeViewForm(openFileDialog.FileName);
			_treeViewForm.Activated += TreeViewForm_Activated;
			_treeViewForm.MdiParent = this;
			_treeViewForm.LoadFile();
			_treeViewForm.Show();
		}

		private void Menu_File_Save_Click(object sender, EventArgs e)
		{
			_treeViewForm?.SaveFile();
		}

		private void Menu_File_SaveAs_Click(object sender, EventArgs e)
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

		private void Menu_Edit_Add_Click(object sender, EventArgs e)
		{
			if (_treeViewForm == null)
				return;

			AddNodeForm nodeAddForm = new AddNodeForm();
			nodeAddForm.OnAddNode += _treeViewForm.OnAddNode;
			nodeAddForm.ShowDialog();
		}

		private void TreeViewForm_Activated(object sender, EventArgs e)
		{
			_treeViewForm = (NytTreeViewForm)sender;
		}
	}
}
