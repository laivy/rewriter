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

		public void Add(NytNode node)
		{
			if (SelectedNode == null)
				Nodes.Add(node);
			else
				SelectedNode.Nodes.Add(node);
			SelectedNode = node;
		}

		public void Save(string filePath)
		{
			FileStream fileStream = new FileStream(filePath, FileMode.OpenOrCreate);
			BinaryWriter binaryWriter = new BinaryWriter(fileStream);
			binaryWriter.Write(Nodes.Count);

			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytNode node = (NytNode)iter.Current;
				node.Save(binaryWriter);
			}
			binaryWriter.Close();
			fileStream.Close();
		}

		public void Load(string filePath)
		{
			FileStream fileStream = new FileStream(filePath, FileMode.Open);
			BinaryReader binaryReader = new BinaryReader(fileStream);
			int nodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < nodeCount; ++i)
			{
				NytNode node = new NytNode();
				node.Load(binaryReader);
				Add(node);
				SelectedNode = null;
			}
			binaryReader.Close();
			fileStream.Close();
		}
	}
}
