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
using Editor.Nyt;

namespace Editor
{
	public partial class AddNodeForm : Form
	{
		private enum DataType
		{
			GROUP, INT,	FLOAT, STRING, IMAGE
		};

		public event EventHandler OnAddNode;

		public AddNodeForm()
		{
			InitializeComponent();
		}

		private void typeComboBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			valueTextBox.Enabled = true;

			DataType selectedDataType = (DataType)typeComboBox.SelectedIndex;
			switch (selectedDataType)
			{
				case DataType.GROUP:
					valueTextBox.Enabled = false;
					break;
				case DataType.IMAGE:
					OpenFileDialog openFileDialog = new OpenFileDialog();
					//fileDialog.Filter = "nyt files (*.nyt)|*.nyt";
					openFileDialog.Filter = "All files (*.*)|*.*";
					openFileDialog.Title = "불러올 파일을 선택해주세요.";
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

		private void addButton_Click(object sender, EventArgs e)
		{
			// 타입에 따른 값 체크
			DataType type = (DataType)typeComboBox.SelectedIndex;
			switch (type)
			{
				case DataType.GROUP:
					break;
				case DataType.INT:
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
				case DataType.FLOAT:
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
				case DataType.IMAGE:
					FileInfo fileInfo = new FileInfo(valueTextBox.Text);
					if (!fileInfo.Exists)
					{
						MessageBox.Show("해당 이미지 파일을 찾을 수 없습니다.");
						return;
					}
					break;
			}

			// 메인 다이얼로그에 이벤트 전달
			OnAddNode(new NytTreeNodeInfo(typeComboBox.SelectedIndex, nameTextBox.Text, valueTextBox.Text), null);
			Close();
		}

		private void cancleButton_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}
