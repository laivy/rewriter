using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public class NytTreeView : TreeView
	{
		public NytTreeView()
		{
			MouseDown += NytTreeView_MouseDown;
		}

		private void NytTreeView_MouseDown(object sender, MouseEventArgs e)
		{
			if (GetNodeAt(e.X, e.Y) == null)
				SelectedNode = null;
		}

		public void Save()
		{
			/*
			저장 방식
			루트 노드 개수
			  └ 노드 타입, 이름, 값
			  └ 하위 노드 개수
			    └ ...
			*/
#if DEBUG
			string filePath = "test.nyt";
			StreamWriter streamWriter = new StreamWriter(filePath);
			streamWriter.WriteLine($"{Nodes.Count}");

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
#endif
		}
	}
}
