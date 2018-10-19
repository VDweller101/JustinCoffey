using UnityEngine;
namespace ProtoChunkLoader {
	[System.Serializable]
	public class Chunk  {
		public Location Location;
		public TileData[,] Tiles;

		public Chunk (int size, Location location)
		{
			Tiles = new TileData[size, size];
			Location = location;
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					Tiles [i, j] = new TileData (new Location (i, j), (byte)Random.Range (0, 4));
				}
			}
		}

		public TileData GetTile (Location location)
		{
			return Tiles [location.X, location.Y];
		}
	}
}
