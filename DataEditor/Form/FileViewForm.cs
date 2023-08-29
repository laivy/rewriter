using Editor.Nyt;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace Editor
{
	public partial class FileViewForm : Form
	{
		private string _filePath; // 이 폼이 보여주고 있는 파일
		private bool _isModified; // 파일이 변경되었는지

		public FileViewForm(string filePath)
		{
			InitializeComponent();
			_filePath = Text = filePath;
			_isModified = false;

			NytTreeView.NodeMouseClick += OnTreeViewRightClick;
			NytTreeView.ItemDrag += OnItemDrag;
			NytTreeView.DragEnter += OnDragEnter;
			NytTreeView.DragOver += OnDragOver;
			NytTreeView.DragDrop += OnDragDrop;
		}

		private void OnAddNodeMenuClick(object sender, EventArgs e)
		{
			NodeEditForm nodeEditForm = new NodeEditForm(this);
			nodeEditForm.ShowDialog();
		}

		private void OnModifyNodeMenuClick(object sender, EventArgs e)
		{
			NytNode node = GetSelectedNode();
			if (node == null)
				return;

			NodeEditForm nodeEditForm = new NodeEditForm(this);
			nodeEditForm.SetNode(node);
			nodeEditForm.ShowDialog();
		}

		private void OnDeleteNodeMenuClick(object sender, EventArgs e)
		{
			GetSelectedNode()?.Remove();
		}

		private void OnTreeViewRightClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				NytTreeView.SelectedNode = e.Node;
			}
		}

		private void OnItemDrag(object sender, ItemDragEventArgs e)
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

		private void OnDragEnter(object sender, DragEventArgs e)
		{
			e.Effect = e.AllowedEffect;
		}

		private void OnDragOver(object sender, DragEventArgs e)
		{
			Point targetPoint = NytTreeView.PointToClient(new Point(e.X, e.Y));
			NytTreeView.SelectedNode = NytTreeView.GetNodeAt(targetPoint);
		}

		private void OnDragDrop(object sender, DragEventArgs e)
		{
			// 마우스 좌표로 노드를 가져옴
			Point targetPoint = NytTreeView.PointToClient(new Point(e.X, e.Y));
			NytNode targetNode = (NytNode)NytTreeView.GetNodeAt(targetPoint);
			NytNode draggedNode = (NytNode)e.Data.GetData(typeof(NytNode));

			// 타겟이 없을 경우 최상위로 옮김
			if (targetNode == null)
			{
				draggedNode.Remove();
				NytTreeView.Nodes.Add(draggedNode);
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
					targetNode.Nodes.Add((NytNode)draggedNode.Clone());
				}
				targetNode.Expand();
			}
		}

		private NytNode GetSelectedNode()
		{
			if (NytTreeView.SelectedNode != null)
				return (NytNode)NytTreeView.SelectedNode;
			return null;
		}

		private bool IsContain(TreeNode node1, TreeNode node2)
		{
			if (node2.Parent == null) return false;
			if (node2.Parent.Equals(node1)) return true;
			return IsContain(node1, node2.Parent);
		}

		public void SaveAsFile(string filePath)
		{
			_filePath = filePath;
			SaveFile();
			SetIsModified(false);
		}

		public void SaveFile()
		{
			NytTreeView.Save(_filePath);
			SetIsModified(false);
			MessageBox.Show("저장 완료");
		}

		public void LoadFile()
		{
			NytTreeView.Load(_filePath);
		}

		public NytTreeView GetNytTreeView()
		{
			return NytTreeView;
		}

		public void SetIsModified(bool isModified)
		{
			_isModified = isModified;
			if (_isModified)
				Text = _filePath + " *";
			else
				Text = _filePath;
		}
	}
}
