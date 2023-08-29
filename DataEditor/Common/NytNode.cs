using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public class NytNode : TreeNode
	{
		public NytType _type { get; set; }
		public string _name { get; set; }
		public string _value { get; set; }
		public byte[] _data { get; set; }
		
		public NytNode() { }

		public NytNode(NytType type, string name, string value)
		{
			_type = type;
			_name = name;
			_value = value;

			switch (_type)
			{
				case NytType.GROUP:
					Text = _name;
					break;
				case NytType.D2DImage:
				case NytType.D3DImage:
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
				case NytType.GROUP:
					binaryWriter.Write(_name);
					break;
				case NytType.INT:
					binaryWriter.Write(_name);
					binaryWriter.Write(int.Parse(_value));
					break;
				case NytType.INT2:
					binaryWriter.Write(_name);
					string[] values = _value.Split(',');
					foreach (string value in values)
						binaryWriter.Write(int.Parse(value.Trim()));
					break;
				case NytType.FLOAT:
					binaryWriter.Write(_name);
					binaryWriter.Write(float.Parse(_value));
					break;
				case NytType.STRING:
					binaryWriter.Write(_name);
					binaryWriter.Write(_value);
					break;
				case NytType.D2DImage:
				case NytType.D3DImage:
					binaryWriter.Write(_name);
					binaryWriter.Write(_data.Length);
					binaryWriter.Write(_data);
					break;
			}

			binaryWriter.Write(Nodes.Count);
			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				NytNode node = (NytNode)iter.Current;
				node.Save(binaryWriter);
			}
		}

		public void Load(BinaryReader binaryReader)
		{
			_type = (NytType)binaryReader.ReadByte();
			switch (_type)
			{
				case NytType.GROUP:
					_name = binaryReader.ReadString();
					break;
				case NytType.INT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadInt32().ToString();
					break;
				case NytType.INT2:
					_name = binaryReader.ReadString();
					_value = $"{binaryReader.ReadInt32()},{binaryReader.ReadInt32()}";
					break;
				case NytType.FLOAT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadSingle().ToString();
					break;
				case NytType.STRING:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadString();
					break;
				case NytType.D2DImage:
				case NytType.D3DImage:
					_name = binaryReader.ReadString();
					_data = binaryReader.ReadBytes(binaryReader.ReadInt32());
					break;
			}

			switch (_type)
			{
				case NytType.GROUP:
					Text = _name;
					break;
				case NytType.D3DImage:
					Text = _name;
					break;
				default:
					Text = $"{_name} : {_value}";
					break;
			}

			int childNodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < childNodeCount; ++i)
			{
				NytNode childNode = new NytNode();
				childNode.Load(binaryReader);
				Nodes.Add(childNode);
			}
		}

		override public object Clone() 
		{
			NytNode node = (NytNode)base.Clone();
			node._type = _type;
			node._name = _name;
			node._value = _value;
			node._data = _data;
			return node;
		}
	}
}
