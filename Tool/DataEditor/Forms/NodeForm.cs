using System;
using System.IO;
using System.Windows.Forms;

namespace DataEditor
{
	public partial class NodeForm : Form
	{
		private ViewForm _fileViewForm;
		private DataNode _node;

		public NodeForm(ViewForm fileViewForm)
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
			DataType type = (DataType)_typeComboBox.SelectedIndex;
			switch (type)
			{
				case DataType.GROUP:
					_valueTextBox.Enabled = false;
					break;
				default:
					_valueTextBox.Enabled = true;
					break;
			}
			_valueTextBox.Text = "";
		}

		private void OnValueTextBoxClick(object sender, EventArgs e)
		{
			DataType type = (DataType)_typeComboBox.SelectedIndex;
			switch (type)
			{
				case DataType.D2DImage:
				case DataType.D3DImage:
					OpenFileDialog openFileDialog = new OpenFileDialog();
					openFileDialog.Filter = "png file (*.png)|*.png";
					openFileDialog.Title = "불러올 이미지를 선택해주세요.";
					if (openFileDialog.ShowDialog() == DialogResult.OK)
						_valueTextBox.Text = openFileDialog.FileName;
					break;
			}
		}

		private void OnAddButtonClick(object sender, EventArgs e)
		{
			// 타입에 따른 값 체크
			DataType type = (DataType)_typeComboBox.SelectedIndex;
			if (!IsValid(type, _valueTextBox.Text))
				return;

			// 노드 추가
			if (_node == null)
			{
				DataNode node = new DataNode(type, _nameTextBox.Text, _valueTextBox.Text);
				_fileViewForm.AddNode(node);
				Close();
				return;
			}

			// 노드 수정
			_node.Set(type, _nameTextBox.Text, _valueTextBox.Text);
			_fileViewForm.SetIsModified(true);
			Close();
		}

		private void OnCancleButtonClick(object sender, EventArgs e)
		{
			Close();
		}

		private bool IsValid(DataType type, string value)
		{
			switch (type)
			{
				case DataType.GROUP:
					return true;
				case DataType.INT:
					if (int.TryParse(value, out _))
						return true;
					return false;
				case DataType.INT2:
					string[] values = value.Split(',');
					if (values.Length != 2)
						return false;
					if (int.TryParse(values[0], out _) && int.TryParse(values[1], out _))
						return true;
					return false;
				case DataType.FLOAT:
					if (float.TryParse(_valueTextBox.Text, out _))
						return true;
					return false;
				case DataType.STRING:
					return true;
				case DataType.D2DImage:
				case DataType.D3DImage:
					if (_valueTextBox.Text.Length == 0)
						return false;
					FileInfo fileInfo = new FileInfo(_valueTextBox.Text);
					if (!fileInfo.Exists)
						return false;
					if (fileInfo.Extension != ".png")
						return false;
					return true;
			}
			return false;
		}

		public void SetNode(DataNode node)
		{
			_node = node;
			Text = "수정하기";
			_typeComboBox.Text = $"{node.GetDataType()}";
			_typeComboBox.SelectedIndex = (int)node.GetDataType();
			_nameTextBox.Text = node.GetName();
			_valueTextBox.Text = node.GetValue();
		}
	}
}
