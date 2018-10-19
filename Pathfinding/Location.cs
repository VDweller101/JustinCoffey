[System.Serializable]
public struct Location  {
	public int x { get; set; }
	public int z { get; set; }
	public Location(int x, int z) : this()
	{
		this.x = x;
		this.z = z;
	}
}
