using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public class NytTreeNode : TreeNode
	{
		private NytDataType _type;
		private string _name;
		private string _value;
		private byte[] _data;

		public NytTreeNode(NytTreeNodeInfo info)
		{
			_type = info._type;
			_name = info._name;
			_value = info._value;

			switch (_type)
			{
				case NytDataType.GROUP:
					Text = _name;
					break;
				case NytDataType.IMAGE:
					Text = _name;
					_data = File.ReadAllBytes(_value);
					break;
				default:
					Text = $"{_name} : {_value}";
					break;
			}
		}

		public void Save(StreamWriter streamWriter, int depth = 0)
		{
#if DEBUG
			// 노드 데이터
			string intent = "";
			for (int i = 0; i < depth; i++)
				intent += "    ";

			string value = intent;
			switch (_type )
			{
				case NytDataType.GROUP:
					value += _name;
					break;
				default:
					value += $"{_type}, {_name}, {_value}";
					break;
			}
			streamWriter.WriteLine(value);

			if (Nodes.Count == 0)
				return;

			// 하위 노드 개수, 순회
			streamWriter.WriteLine($"{intent}    {Nodes.Count}");
			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytTreeNode node = (NytTreeNode)iter.Current;
				node.Save(streamWriter, depth + 1);
			}
#else

#endif
		}
	}
}
