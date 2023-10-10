using System;
using System.IO;
using System.Windows.Forms;
using Editor.Nyt;

namespace Editor
{
	public partial class NodeEditForm : Form
	{
		private FileViewForm _fileViewForm;
		private NytNode _node;

		public NodeEditForm(FileViewForm fileViewForm)
		{
			InitializeComponent();
			_fileViewForm = fileViewForm;
			KeyDown += OnKeyDown;
		}

		private void OnKeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					OnAddButtonClick(null, null);
					break;
				case Keys.Escape:
					OnCancleButtonClick(null, null);
					break;
			}
		}

		private void OnNodeTypeChange(object sender, EventArgs e)
		{
			NytType type = (NytType)typeComboBox.SelectedIndex;
			switch (type)
			{
				case NytType.GROUP:
					valueTextBox.Enabled = false;
					break;
				default:
					valueTextBox.Enabled = true;
					break;
			}
		}

		private void OnValueTextBoxClick(object sender, EventArgs e)
		{
			NytType type = (NytType)typeComboBox.SelectedIndex;
			switch (type)
			{
				case NytType.D2DImage:
				case NytType.D3DImage:
					OpenFileDialog openFileDialog = new OpenFileDialog();
					openFileDialog.Filter = "png file (*.png)|*.png";
					openFileDialog.Title = "불러올 이미지를 선택해주세요.";
					if (openFileDialog.ShowDialog() == DialogResult.OK)
						valueTextBox.Text = openFileDialog.FileName;
					break;
			}
		}

		private void OnAddButtonClick(object sender, EventArgs e)
		{
			// 타입에 따른 값 체크
			NytType type = (NytType)typeComboBox.SelectedIndex;
			if (!IsValid(type, valueTextBox.Text))
				return;

			// 노드 추가
			if (_node == null)
			{
				NytNode node = new NytNode(type, nameTextBox.Text, valueTextBox.Text);
				NytTreeView nytTreeView = _fileViewForm.GetNytTreeView();
				nytTreeView.Add(node);
				_fileViewForm.SetIsModified(true);
				Close();
				return;
			}

			// 노드 수정
			_node._type = type;
			_node._name = nameTextBox.Text;
			_node._value = valueTextBox.Text;

			switch (_node._type)
			{
				case NytType.GROUP:
					_node.Text = _node._name;
					break;
				case NytType.D2DImage:
				case NytType.D3DImage:
					_node.Text = _node._name;
					_node._data = File.ReadAllBytes(_node._value);
					break;
				default:
					_node.Text = $"{_node._name} : {_node._value}";
					break;
			}
			_fileViewForm.SetIsModified(true);
			Close();
		}

		private void OnCancleButtonClick(object sender, EventArgs e)
		{
			Close();
		}

		private bool IsValid(NytType type, string value)
		{
			switch (type)
			{
				case NytType.GROUP:
					return true;
				case NytType.INT:
					try
					{
						int.Parse(value);
					}
					catch (Exception exception)
					{
						MessageBox.Show(exception.Message);
						return false;
					}
					return true;
				case NytType.INT2:
					try
					{
						string[] values = value.Split(',');
						if (values.Length != 2)
							throw new IndexOutOfRangeException();
						int.Parse(values[0].Trim());
						int.Parse(values[1].Trim());
					}
					catch (Exception exception)
					{
						MessageBox.Show(exception.Message);
						return false;
					}
					return true;
				case NytType.FLOAT:
					try
					{
						float.Parse(valueTextBox.Text);
					}
					catch (Exception exception)
					{
						MessageBox.Show(exception.Message);
						return false;
					}
					return true;
				case NytType.STRING:
					return true;
				case NytType.D2DImage:
				case NytType.D3DImage:
					FileInfo fileInfo = new FileInfo(valueTextBox.Text);
					if (!fileInfo.Exists)
					{
						MessageBox.Show("해당 이미지 파일을 찾을 수 없습니다.");
						return false;
					}
					return true;
			}
			return false;
		}

		public void SetNode(NytNode node)
		{
			_node = node;
			Text = "수정하기";
			typeComboBox.Text = $"{node._type}";
			typeComboBox.SelectedIndex = (int)node._type;
			nameTextBox.Text = node._name;
			valueTextBox.Text = node._value;
		}
	}
}
