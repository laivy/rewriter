using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace DataEditor
{
	public enum DataType
	{
		GROUP, INT, INT2, FLOAT, STRING, D2DImage, D3DImage
	}

	public class DataNode : TreeNode
	{
		private DataType _type;
		private string _name;
		private string _value;
		private byte[] _data;

		public DataNode() { }

		public DataNode(DataType type, string name, string value)
		{
			Set(type, name, value);
		}

		override public object Clone()
		{
			DataNode node = (DataNode)base.Clone();
			node._type = _type;
			node._name = _name;
			node._value = _value;
			node._data = _data;
			return node;
		}

		public void Set(DataType type, string name, string value, byte[] data = null)
		{
			_type = type;
			SetName(name);
			_value = value;

			if (data != null)
				_data = data;
			else
			{
				switch (_type)
				{
					case DataType.D2DImage:
					case DataType.D3DImage:
						_data = File.ReadAllBytes(value);
						break;
				}
			}
		}

		public void SetName(string name)
		{
			_name = name;
			switch (_type)
			{
				case DataType.GROUP:
				case DataType.D2DImage:
				case DataType.D3DImage:
					Text = $"{_name}({_type})";
					break;
				default:
					Text = $"{_name}({_type}) : {_value}";
					break;
			}
		}

		public void Save(BinaryWriter binaryWriter)
		{
			binaryWriter.Write((byte)_type);
			switch (_type)
			{
				case DataType.GROUP:
					binaryWriter.Write(_name);
					break;
				case DataType.INT:
					binaryWriter.Write(_name);
					binaryWriter.Write(int.Parse(_value));
					break;
				case DataType.INT2:
					binaryWriter.Write(_name);
					string[] values = _value.Split(',');
					foreach (string value in values)
						binaryWriter.Write(int.Parse(value.Trim()));
					break;
				case DataType.FLOAT:
					binaryWriter.Write(_name);
					binaryWriter.Write(float.Parse(_value));
					break;
				case DataType.STRING:
					binaryWriter.Write(_name);
					binaryWriter.Write(_value);
					break;
				case DataType.D2DImage:
				case DataType.D3DImage:
					binaryWriter.Write(_name);
					binaryWriter.Write(_data.Length);
					binaryWriter.Write(_data);
					break;
			}

			binaryWriter.Write(Nodes.Count);
			IEnumerator iter = Nodes.GetEnumerator();
			while (iter.MoveNext())
			{
				DataNode node = (DataNode)iter.Current;
				node.Save(binaryWriter);
			}
		}

		public void Load(BinaryReader binaryReader)
		{
			_type = (DataType)binaryReader.ReadByte();
			switch (_type)
			{
				case DataType.GROUP:
					_name = binaryReader.ReadString();
					break;
				case DataType.INT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadInt32().ToString();
					break;
				case DataType.INT2:
					_name = binaryReader.ReadString();
					_value = $"{binaryReader.ReadInt32()},{binaryReader.ReadInt32()}";
					break;
				case DataType.FLOAT:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadSingle().ToString();
					break;
				case DataType.STRING:
					_name = binaryReader.ReadString();
					_value = binaryReader.ReadString();
					break;
				case DataType.D2DImage:
				case DataType.D3DImage:
					_name = binaryReader.ReadString();
					_data = binaryReader.ReadBytes(binaryReader.ReadInt32());
					break;
			}

			Set(_type, _name, _value, _data);

			int childNodeCount = binaryReader.ReadInt32();
			for (int i = 0; i < childNodeCount; ++i)
			{
				DataNode childNode = new DataNode();
				childNode.Load(binaryReader);
				Nodes.Add(childNode);
			}
		}

		public DataType GetDataType() { return _type; }
		public string GetName() { return _name; }
		public string GetValue() { return _value; }
	}
}
