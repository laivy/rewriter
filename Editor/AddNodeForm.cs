using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

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
			OnAddNode(new Tuple<int, string, string>(typeComboBox.SelectedIndex, nameTextBox.Text, valueTextBox.Text), null);
			Close();
		}

		private void cancleButton_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}
