namespace Editor.Nyt
{
	public enum NytType
	{
		GROUP, INT, INT2, FLOAT, STRING, D2DImage, D3DImage
	}

	public class NytTreeNodeInfo
	{
		public NytType _type { get; }
		public string _name { get; }
		public string _value { get; }

		public NytTreeNodeInfo(int type, string name, string value)
		{
			_type = (NytType)type;
			_name = name;
			_value = value;
		}
	};
}
