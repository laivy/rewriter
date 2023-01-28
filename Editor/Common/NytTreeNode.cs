using System;
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

#if DEBUG
		public NytTreeNode(StreamReader streamReader)
		{
			// 들여쓰기 삭제
			string line = streamReader.ReadLine();
			string[] info = line.Split(',');
			for (int i = 0; i < info.Length; ++i)
			{
				info[i] = info[i].Trim();
			}

			// 데이터 읽기
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

			// 노드 이름 설정
			switch (_type)
			{
				case NytDataType.GROUP:
					Text = _name;
					break;
				case NytDataType.IMAGE:
					Text = _name;
					break;
				default:
					Text = $"{_name} : {_value}";
					break;
			}

			// 하위 노드 읽기
			int childNodeCount = int.Parse(streamReader.ReadLine());
			for (int i = 0; i < childNodeCount; i++)
			{
				NytTreeNode node = new NytTreeNode(streamReader);
				Nodes.Add(node);
			}
		}
#else
		public NytTreeNode(BinaryReader binaryReader)
		{
			_type = (NytDataType)binaryReader.ReadByte();
			switch (_type)
			{
				case NytDataType.GROUP:
					_name = binaryReader.ReadString();
					break;
				case NytDataType.INT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadInt32().ToString();
					break;
				case NytDataType.FLOAT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadSingle().ToString();
					break;
				case NytDataType.STRING:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadString();
					break;
				case NytDataType.IMAGE:
					_name = binaryReader.ReadString();
					_data = binaryReader.ReadBytes(binaryReader.ReadInt32());
					break;
			}

			switch (_type)
			{
				case NytDataType.GROUP:
					Text = _name;
					break;
				case NytDataType.IMAGE:
					Text = _name;
					break;
				default:
					Text = $"{_name} : {_value}";
					break;
			}

			int childNodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < childNodeCount; ++i)
				Nodes.Add(new NytTreeNode(binaryReader));
		}
#endif

#if DEBUG
		public void Save(StreamWriter streamWriter, int depth = 0)
		{
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
		}
#else
		public void Save(BinaryWriter binaryWriter)
		{
			binaryWriter.Write((byte)_type);
			switch (_type)
			{
				case NytDataType.GROUP:
					binaryWriter.Write(_name);
					break;
				case NytDataType.INT:
					binaryWriter.Write(_name);
					binaryWriter.Write(int.Parse(_value));
					break;
				case NytDataType.FLOAT:
					binaryWriter.Write(_name);
					binaryWriter.Write(float.Parse(_value));
					break;
				case NytDataType.STRING:
					binaryWriter.Write(_name);
					binaryWriter.Write(_value);
					break;
				case NytDataType.IMAGE:
					binaryWriter.Write(_name);
					binaryWriter.Write(_data.Length);
					binaryWriter.Write(_data);
					break;
			}

			binaryWriter.Write(Nodes.Count);
			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytTreeNode node = (NytTreeNode)iter.Current;
				node.Save(binaryWriter);
			}
		}
#endif
	}
}
