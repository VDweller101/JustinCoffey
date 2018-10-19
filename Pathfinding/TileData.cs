[System.Serializable]
public class TileData {
	public Location location { get; protected set; }
	public InstalledObject installedObject { get; set; }
	public TileData (Location location)
	{
		this.location = location;
	}
}
