namespace DataEditor
{
	partial class NodeForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this._typeComboBox = new System.Windows.Forms.ComboBox();
            this._valueTextBox = new System.Windows.Forms.TextBox();
            this._addButton = new System.Windows.Forms.Button();
            this._cancleButton = new System.Windows.Forms.Button();
            this._typeLabel = new System.Windows.Forms.Label();
            this._valueLabel = new System.Windows.Forms.Label();
            this._nameTextBox = new System.Windows.Forms.TextBox();
            this._nameLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // typeComboBox
            // 
            this._typeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._typeComboBox.FormattingEnabled = true;
            this._typeComboBox.Items.AddRange(new object[] {
            "GROUP",
            "INT",
            "INT2",
            "FLOAT",
            "STRING",
            "D2DImage",
            "D3DImage"});
            this._typeComboBox.Location = new System.Drawing.Point(47, 6);
            this._typeComboBox.Name = "typeComboBox";
            this._typeComboBox.Size = new System.Drawing.Size(156, 20);
            this._typeComboBox.TabIndex = 0;
            this._typeComboBox.SelectedIndexChanged += new System.EventHandler(this.OnNodeTypeChange);
            // 
            // valueTextBox
            // 
            this._valueTextBox.Location = new System.Drawing.Point(47, 59);
            this._valueTextBox.Name = "valueTextBox";
            this._valueTextBox.Size = new System.Drawing.Size(156, 21);
            this._valueTextBox.TabIndex = 2;
            this._valueTextBox.Click += new System.EventHandler(this.OnValueTextBoxClick);
            // 
            // addButton
            // 
            this._addButton.Location = new System.Drawing.Point(47, 86);
            this._addButton.Name = "addButton";
            this._addButton.Size = new System.Drawing.Size(75, 23);
            this._addButton.TabIndex = 3;
            this._addButton.Text = "확인";
            this._addButton.UseVisualStyleBackColor = true;
            this._addButton.Click += new System.EventHandler(this.OnAddButtonClick);
            // 
            // cancleButton
            // 
            this._cancleButton.Location = new System.Drawing.Point(128, 86);
            this._cancleButton.Name = "cancleButton";
            this._cancleButton.Size = new System.Drawing.Size(75, 23);
            this._cancleButton.TabIndex = 4;
            this._cancleButton.Text = "취소";
            this._cancleButton.UseVisualStyleBackColor = true;
            this._cancleButton.Click += new System.EventHandler(this.OnCancleButtonClick);
            // 
            // TypeLabel
            // 
            this._typeLabel.AutoSize = true;
            this._typeLabel.Location = new System.Drawing.Point(12, 9);
            this._typeLabel.Name = "TypeLabel";
            this._typeLabel.Size = new System.Drawing.Size(29, 12);
            this._typeLabel.TabIndex = 4;
            this._typeLabel.Text = "타입";
            // 
            // ValueLabel
            // 
            this._valueLabel.AutoSize = true;
            this._valueLabel.Location = new System.Drawing.Point(24, 62);
            this._valueLabel.Name = "ValueLabel";
            this._valueLabel.Size = new System.Drawing.Size(17, 12);
            this._valueLabel.TabIndex = 5;
            this._valueLabel.Text = "값";
            // 
            // nameTextBox
            // 
            this._nameTextBox.Location = new System.Drawing.Point(47, 32);
            this._nameTextBox.MaxLength = 30;
            this._nameTextBox.Name = "nameTextBox";
            this._nameTextBox.Size = new System.Drawing.Size(156, 21);
            this._nameTextBox.TabIndex = 1;
            // 
            // NameLabel
            // 
            this._nameLabel.AutoSize = true;
            this._nameLabel.Location = new System.Drawing.Point(12, 35);
            this._nameLabel.Name = "NameLabel";
            this._nameLabel.Size = new System.Drawing.Size(29, 12);
            this._nameLabel.TabIndex = 7;
            this._nameLabel.Text = "이름";
            // 
            // NodeEditForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(217, 119);
            this.Controls.Add(this._nameLabel);
            this.Controls.Add(this._nameTextBox);
            this.Controls.Add(this._valueLabel);
            this.Controls.Add(this._typeLabel);
            this.Controls.Add(this._cancleButton);
            this.Controls.Add(this._addButton);
            this.Controls.Add(this._valueTextBox);
            this.Controls.Add(this._typeComboBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "NodeEditForm";
            this.ShowIcon = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "추가하기";
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ComboBox _typeComboBox;
		private System.Windows.Forms.TextBox _valueTextBox;
		private System.Windows.Forms.Button _addButton;
		private System.Windows.Forms.Button _cancleButton;
		private System.Windows.Forms.Label _typeLabel;
		private System.Windows.Forms.Label _valueLabel;
		private System.Windows.Forms.TextBox _nameTextBox;
		private System.Windows.Forms.Label _nameLabel;
	}
}