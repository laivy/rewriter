using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public class NytTreeNode : TreeNode
	{
		public NytDataType _type { get; set; }
		public string _name { get; set; }
		public string _value { get; set; }
		public byte[] _data { get; set; }

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

		public NytTreeNode(StreamReader streamReader)
		{
#if DEBUG
			string line = streamReader.ReadLine();
			string[] info = line.Split(',');
			for (int i = 0; i < info.Length; ++i)
			{
				info[i] = info[i].Trim();
			}

			if (info[0] == "GROUP")
			{
				_type = NytDataType.GROUP;
				_name = info[1];
			}
			else if (info[0] == "INT")
			{
				_type = NytDataType.INT;
				_name = info[1];
				_value = info[2];
			}
			else if (info[0] == "FLOAT")
			{
				_type = NytDataType.FLOAT;
				_name = info[1];
				_value = info[2];
			}
			else if (info[0] == "STRING")
			{
				_type = NytDataType.STRING;
				_name = info[1];
				_value = info[2];
			}
			else if (info[0] == "IMAGE")
			{
				_type = NytDataType.IMAGE;
				_name = info[1];
				_value = info[2];
			}
#else

#endif
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

#if DEBUG
			int childNodeCount = int.Parse(streamReader.ReadLine());
			for (int i = 0; i < childNodeCount; i++)
			{
				NytTreeNode node = new NytTreeNode(streamReader);
				Nodes.Add(node);
			}
#else

#endif
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
					value += $"{_type}, {_name}";
					break;
				default:
					value += $"{_type}, {_name}, {_value}";
					break;
			}
			streamWriter.WriteLine(value);

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
