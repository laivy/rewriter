namespace Editor.Nyt
{
	public enum NytDataType
	{
		GROUP, INT, INT2, FLOAT, STRING, UI, IMAGE
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
	};
}
