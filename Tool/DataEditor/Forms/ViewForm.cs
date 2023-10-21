using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace DataEditor
{
	public partial class ViewForm : Form
	{
		private string _filePath; // 이 폼이 보여주고 있는 파일
		private bool _isModified; // 파일이 변경되었는지

		public ViewForm(string filePath)
		{
			InitializeComponent();
			LoadFile(filePath);

			Text = Path.GetFileName(filePath);
			_filePath = filePath;
			_isModified = false;

			KeyDown += OnKeyDown;
			_treeView.AfterLabelEdit += OnTreeViewAfterLabelEdit;
			_treeView.MouseDown += OnTreeViewMouseDown;
			_treeView.NodeMouseClick += OnTreeViewNodeMouseClick;
			_treeView.NodeMouseDoubleClick += OnTreeviewNodeMouseDoubleClick;
			_treeView.ItemDrag += OnTreeViewItemDrag;
			_treeView.DragEnter += OnTreeViewDragEnter;
			_treeView.DragOver += OnTreeViewDragOver;
			_treeView.DragDrop += OnTreeViewDragDrop;
		}

		private void OnTreeViewAfterLabelEdit(object sender, NodeLabelEditEventArgs e)
		{
			BeginInvoke((MethodInvoker) delegate
			{
				if (e.Label == null)
					return;

				if (e.Label.Length <= 0)
				{
					e.CancelEdit = true;
					e.Node.BeginEdit();
				}

				e.Node.EndEdit(false);

				DataNode node = e.Node as DataNode;
				node.SetName(e.Label);
				SetIsModified(true);
			});
		}

		private void OnKeyDown(object sender, KeyEventArgs e)
		{
			// 노드명 변경
			if (e.KeyCode == Keys.F2)
			{
				DataNode node = _treeView.SelectedNode as DataNode;
				if (node != null && !node.IsEditing)
				{
					node.Text = node.GetName();
					node.BeginEdit();
					return;
				}
			}
		}

		private void OnTreeViewMouseDown(object sender, MouseEventArgs e)
		{
			if (_treeView.GetNodeAt(e.X, e.Y) == null)
				_treeView.SelectedNode = null;
		}

		private void OnAddNodeMenuClick(object sender, EventArgs e)
		{
			NodeForm nodeEditForm = new NodeForm(this);
			nodeEditForm.ShowDialog();
		}

		private void OnModifyNodeMenuClick(object sender, EventArgs e)
		{
			DataNode node = _treeView.SelectedNode as DataNode;
			if (node == null)
				return;

			NodeForm nodeEditForm = new NodeForm(this);
			nodeEditForm.SetNode(node);
			nodeEditForm.ShowDialog();
		}

		private void OnDeleteNodeMenuClick(object sender, EventArgs e)
		{
			DataNode node = _treeView.SelectedNode as DataNode;
			node?.Remove();
		}

		private void OnTreeViewNodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				_treeView.SelectedNode = e.Node;
			}
		}

		private void OnTreeviewNodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			DataNode node = _treeView.SelectedNode as DataNode;
			if (node == null)
				return;

			NodeForm nodeEditForm = new NodeForm(this);
			nodeEditForm.SetNode(node);
			nodeEditForm.ShowDialog();
		}

		private void OnTreeViewItemDrag(object sender, ItemDragEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				DoDragDrop(e.Item, DragDropEffects.Move);
			}
			else if (e.Button == MouseButtons.Right)
			{
				DoDragDrop(e.Item, DragDropEffects.Copy);
			}
		}

		private void OnTreeViewDragEnter(object sender, DragEventArgs e)
		{
			e.Effect = e.AllowedEffect;
		}

		private void OnTreeViewDragOver(object sender, DragEventArgs e)
		{
			Point targetPoint = _treeView.PointToClient(new Point(e.X, e.Y));
			DataNode targetNode = _treeView.GetNodeAt(targetPoint) as DataNode;
			_treeView.SelectedNode = targetNode;
		}

		private void OnTreeViewDragDrop(object sender, DragEventArgs e)
		{
			// 마우스 좌표로 노드를 가져옴
			Point targetPoint = _treeView.PointToClient(new Point(e.X, e.Y));
            DataNode targetNode = _treeView.GetNodeAt(targetPoint) as DataNode;
			DataNode draggedNode = e.Data.GetData(typeof(DataNode)) as DataNode;

			// 타겟이 없을 경우 최상위로 옮김
			if (targetNode == null)
			{
				draggedNode.Remove();
				_treeView.Nodes.Add(draggedNode);
				return;
			}

			// 대상과 타겟이 있을 경우
			if (!draggedNode.Equals(targetNode) && !IsContain(draggedNode, targetNode))
			{
				// 노드 이동
				if (e.Effect == DragDropEffects.Move)
				{
					draggedNode.Remove();
					targetNode.Nodes.Add(draggedNode);
				}

				// 노드 복사
				else if (e.Effect == DragDropEffects.Copy)
				{
					targetNode.Nodes.Add((DataNode)draggedNode.Clone());
				}
				targetNode.Expand();
			}
		}

		private bool IsContain(TreeNode node1, TreeNode node2)
		{
			if (node2.Parent == null)
				return false;
			if (node2.Parent.Equals(node1))
				return true;
			return IsContain(node1, node2.Parent);
		}

		private void LoadFile(string filePath)
		{
			if (!File.Exists(filePath))
				return;

			FileStream fileStream = new FileStream(filePath, FileMode.Open);
			BinaryReader binaryReader = new BinaryReader(fileStream);
			int nodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < nodeCount; ++i)
			{
				DataNode node = new DataNode();
				node.Load(binaryReader);
				AddNode(node);
				_treeView.SelectedNode = null;
			}
			binaryReader.Close();
			fileStream.Close();
		}

		public void AddNode(DataNode node)
		{
			if (node == null)
				return;

			_treeView.Nodes.Add(node);
			_isModified = true;
		}

		public void SaveAsFile()
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				Filter = "dat files (*.dat)|*.dat",
				Title = "저장할 파일 위치를 선택해주세요."
			};
			if (saveFileDialog.ShowDialog() != DialogResult.OK)
				return;

			_filePath = saveFileDialog.FileName;
			SaveFile();
		}

		public void SaveFile()
		{
			FileStream fileStream = new FileStream(_filePath, FileMode.OpenOrCreate);
			BinaryWriter binaryWriter = new BinaryWriter(fileStream);
			binaryWriter.Write(_treeView.Nodes.Count);

			IEnumerator iter = _treeView.Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				DataNode node = (DataNode)iter.Current;
				node.Save(binaryWriter);
			}
			binaryWriter.Close();
			fileStream.Close();

			SetIsModified(false);
		}

		public void SetIsModified(bool isModified)
		{
			_isModified = isModified;
			Text = Path.GetFileName(_filePath);
			if (_isModified)
				Text += " *";
		}
	}
}
