using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public class NytTreeView : TreeView
	{
		public NytTreeView()
		{
			MouseDown += OnMouseDown;
		}

		private void OnMouseDown(object sender, MouseEventArgs e)
		{
			if (GetNodeAt(e.X, e.Y) == null)
				SelectedNode = null;
		}

		public void Add(NytTreeNode node)
		{
			if (SelectedNode == null)
				Nodes.Add(node);
			else
				SelectedNode.Nodes.Add(node);
			SelectedNode = node;
		}

		public void Save(string filePath)
		{
#if DEBUG
			StreamWriter streamWriter = new StreamWriter(filePath);
			streamWriter.WriteLine(Nodes.Count);

			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytTreeNode node = (NytTreeNode)iter.Current;
				node.Save(streamWriter);
			}
			streamWriter.Close();
#else
			FileStream fileStream = new FileStream(filePath, FileMode.OpenOrCreate);
			BinaryWriter binaryWriter = new BinaryWriter(fileStream);
			binaryWriter.Write(Nodes.Count);

			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytTreeNode node = (NytTreeNode)iter.Current;
				node.Save(binaryWriter);
			}
			binaryWriter.Close();
			fileStream.Close();
#endif
		}

		internal void Load(string filePath)
		{
#if DEBUG
			StreamReader streamReader = new StreamReader(filePath);
			int nodeCount = int.Parse(streamReader.ReadLine());
			for (int i = 0; i < nodeCount; ++i)
			{
				Add(new NytTreeNode(streamReader));
				SelectedNode = null; // 이렇게 해야 최상위 노드로 추가됨
			}
			streamReader.Close();
#else
			FileStream fileStream = new FileStream(filePath, FileMode.Open);
			BinaryReader binaryReader = new BinaryReader(fileStream);
			int nodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < nodeCount; ++i)
			{
				Add(new NytTreeNode(binaryReader));
				SelectedNode = null;
			}
			binaryReader.Close();
			fileStream.Close();
#endif
		}
	}
}
