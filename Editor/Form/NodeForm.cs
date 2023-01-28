using System;
using System.IO;
using System.Windows.Forms;
using Editor.Nyt;

namespace Editor
{
	public partial class NodeForm : Form
	{
		public event EventHandler _OnAddNode;
		private NytTreeNode _node;

		public NodeForm(NytTreeNode node = null)
		{
			InitializeComponent();
			KeyDown += OnKeyDown;

			if (node != null)
			{
				_node = node;
				Text = "수정하기";
				typeComboBox.Text = $"{node._type}";
				typeComboBox.SelectedIndex = (int)node._type;
				nameTextBox.Text = node._name;
				valueTextBox.Text = node._value;
			}
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

		private void OnTypeComboBoxSelectedIndexChange(object sender, EventArgs e)
		{
			valueTextBox.Enabled = true;

			NytDataType selectedDataType = (NytDataType)typeComboBox.SelectedIndex;
			switch (selectedDataType)
			{
				case NytDataType.GROUP:
					valueTextBox.Enabled = false;
					break;
				case NytDataType.IMAGE:
					OpenFileDialog openFileDialog = new OpenFileDialog();
					openFileDialog.Filter = "png file (*.png)|*.png";
					openFileDialog.Title = "불러올 이미지를 선택해주세요.";
					string filePath = string.Empty;
					if (openFileDialog.ShowDialog() == DialogResult.OK)
					{
						filePath = openFileDialog.FileName;
					}
					valueTextBox.Text = filePath;
					Console.WriteLine(filePath);
					break;
				default:
					break;
			}
		}

		private void OnAddButtonClick(object sender, EventArgs e)
		{
			// 타입에 따른 값 체크
			NytDataType type = (NytDataType)typeComboBox.SelectedIndex;
			switch (type)
			{
				case NytDataType.GROUP:
					break;
				case NytDataType.INT:
					try
					{
						int.Parse(valueTextBox.Text);
					}
					catch (Exception exception)
					{
						MessageBox.Show(exception.Message);
						return;
					}
					break;
				case NytDataType.FLOAT:
					try
					{ 
						float.Parse(valueTextBox.Text);
					}
					catch (Exception exception)
					{
						MessageBox.Show(exception.Message);
						return;
					}
					break;
				case NytDataType.IMAGE:
					FileInfo fileInfo = new FileInfo(valueTextBox.Text);
					if (!fileInfo.Exists)
					{
						MessageBox.Show("해당 이미지 파일을 찾을 수 없습니다.");
						return;
					}
					break;
			}

			// 노드 추가, 수정
			if (_node == null)
				_OnAddNode(new NytTreeNodeInfo(typeComboBox.SelectedIndex, nameTextBox.Text, valueTextBox.Text), null);
			else
			{
				_node._type = (NytDataType)typeComboBox.SelectedIndex;
				_node._name = nameTextBox.Text;
				_node._value = valueTextBox.Text;

				switch (_node._type)
				{
					case NytDataType.GROUP:
						_node.Text = _node._name;
						break;
					case NytDataType.IMAGE:
						_node.Text = _node._name;
						_node._data = File.ReadAllBytes(_node._value);
						break;
					default:
						_node.Text = $"{_node._name} : {_node._value}";
						break;
				}
			}			
			Close();
		}

		private void OnCancleButtonClick(object sender, EventArgs e)
		{
			Close();
		}
	}
}
