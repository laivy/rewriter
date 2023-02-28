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
		
		public NytTreeNode() { }

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
				case NytDataType.UI:
				case NytDataType.IMAGE:
					Text = _name;
					_data = File.ReadAllBytes(_value);
					break;
				default:
					Text = $"{_name} : {_value}";
					break;
			}
		}

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
				case NytDataType.INT2:
					binaryWriter.Write(_name);
					string[] values = _value.Split(',');
					foreach (string value in values)
						binaryWriter.Write(int.Parse(value.Trim()));
					break;
				case NytDataType.FLOAT:
					binaryWriter.Write(_name);
					binaryWriter.Write(float.Parse(_value));
					break;
				case NytDataType.STRING:
					binaryWriter.Write(_name);
					binaryWriter.Write(_value);
					break;
				case NytDataType.UI:
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

		public void Load(BinaryReader binaryReader)
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
				case NytDataType.INT2:
					_name = binaryReader.ReadString();
					_value = $"{binaryReader.ReadInt32()},{binaryReader.ReadInt32()}";
					break;
				case NytDataType.FLOAT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadSingle().ToString();
					break;
				case NytDataType.STRING:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadString();
					break;
				case NytDataType.UI:
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
			{
				NytTreeNode childNode = new NytTreeNode();
				childNode.Load(binaryReader);
				Nodes.Add(childNode);
			}
		}
	}
}
