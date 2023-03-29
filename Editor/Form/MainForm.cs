using Editor.Nyt;
using System;
using System.Windows.Forms;

namespace Editor
{
	public partial class MainForm : Form
	{
		private FileViewForm _fileViewForm;	// 활성화 상태인 트리뷰폼

		public MainForm()
		{
			InitializeComponent();

			DragEnter += OnDragEnter;
			DragDrop += OnDragDrop;
		}

		private void OnFileViewActivated(object sender, EventArgs e)
		{
			_fileViewForm = (FileViewForm)sender;
		}

		private void OnNewFileMenuClick(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "저장할 파일 위치를 선택해주세요."
			};

			DialogResult dialogResult = saveFileDialog.ShowDialog();
			if (dialogResult == DialogResult.Cancel)
				return;
			if (dialogResult != DialogResult.OK)
			{
				MessageBox.Show("해당 경로에 저장할 수 없습니다.");
				return;
			}
			_fileViewForm = new FileViewForm(saveFileDialog.FileName);
			_fileViewForm.Activated += OnFileViewActivated;
			_fileViewForm.MdiParent = this;
			_fileViewForm.SetIsModified(true);
			_fileViewForm.Show();
		}

		private void OnFileOpenMenuClick(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "불러올 파일을 선택해주세요."
			};

			DialogResult dialogResult = openFileDialog.ShowDialog();
			if (dialogResult == DialogResult.Cancel)
				return;
			if (dialogResult != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}

			_fileViewForm = new FileViewForm(openFileDialog.FileName);
			_fileViewForm.Activated += OnFileViewActivated;
			_fileViewForm.MdiParent = this;
			_fileViewForm.LoadFile();
			_fileViewForm.Show();
		}

		private void OnFileSaveMenuClick(object sender, EventArgs e)
		{
			_fileViewForm?.SaveFile();
		}

		private void OnFileSaveAsMenuClick(object sender, EventArgs e)
		{
			if (_fileViewForm == null)
				return;

			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				Filter = "nyt files (*.nyt)|*.nyt",
				Title = "저장할 파일 위치를 선택해주세요."
			};
			if (saveFileDialog.ShowDialog() != DialogResult.OK)
			{
				MessageBox.Show("해당 파일을 열 수 없습니다.");
				return;
			}
			_fileViewForm.SaveAsFile(saveFileDialog.FileName);
		}
		
		private void OnDragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop))
				e.Effect = DragDropEffects.Copy;
		}

		private void OnDragDrop(object sender, DragEventArgs e)
		{
			string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
			foreach (string file in files)
			{
				if (!file.EndsWith(".nyt"))
					continue;

				_fileViewForm = new FileViewForm(file);
				_fileViewForm.Activated += OnFileViewActivated;
				_fileViewForm.MdiParent = this;
				_fileViewForm.LoadFile();
				_fileViewForm.Show();
			}
		}
	}
}
