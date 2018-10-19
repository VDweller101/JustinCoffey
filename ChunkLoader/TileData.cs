namespace ProtoChunkLoader {
	[System.Serializable]
	public class TileData  {
		public Location Location;
		public byte Type;

		public TileData (Location location, byte type)
		{
			this.Location = location;
			this.Type = type;
		}
	}
}
