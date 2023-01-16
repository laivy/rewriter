using System;
using System.IO;
using System.Windows.Forms;

namespace Editor.Nyt
{
	public enum NytDataType
	{
		GROUP, INT, FLOAT, STRING, IMAGE
	}

	public class NytTreeNodeInfo
	{
		public NytDataType _type { get; }
		public string _name { get; }
		public string _value { get; }

		public NytTreeNodeInfo(int type, string name, string value)
		{
			_type = (NytDataType)type;
			_name = name;
			_value = value;
		}

		public bool IsValid()
		{
			switch (_type)
			{
				case NytDataType.GROUP:
					if (_name.Length == 0)
						return false;
					break;
				case NytDataType.INT:
					try
					{
						int.Parse(_value);
					}
					catch (Exception)
					{
						return false;
					}
					break;
				case NytDataType.FLOAT:
					try
					{
						float.Parse(_value);
					}
					catch (Exception)
					{
						return false;
					}
					break;
				case NytDataType.STRING:
					break;
				case NytDataType.IMAGE:
					FileInfo fileInfo = new FileInfo(_value);
					if (!fileInfo.Exists)
						return false;
					break;
				default:
					return false;
			}
			return true;
		}
	};
}
