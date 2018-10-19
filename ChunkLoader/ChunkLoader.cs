using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

namespace ProtoChunkLoader {
	public class ChunkLoader : MonoBehaviour {

		//In tile grid, regions are made of chunks, chunks are made of tiles
		public int ChunkSize;				//How many tiles per chunk

		public int InitialMapGenSize;		//Will multiply by RegionSize to get map size

		public int BufferSize;				//How many chunks are loaded at once, squared

		public int GridSize;

		public int Ceiling;
		public int Floor;

		public Dictionary <Location, Chunk> LoadedChunks;
		public Dictionary <Location, GameObject> GameObjectParentMap;
		public Character character;

		public GameObject TilePrefab;
		public GameObject InputUI;

		Location currentPlayerChunk;

		public InputField ChunkField;
		public InputField BufferField;
		public InputField FloorField;
		public InputField CeilingField;
		public Text PlayerText;

		void Awake ()
		{
			if (Directory.Exists (Application.persistentDataPath + "/Proto/ChunkLoader/")) {
				Directory.Delete (Application.persistentDataPath + "/Proto/ChunkLoader/", true);
			}
			Directory.CreateDirectory (Application.persistentDataPath + "/Proto/ChunkLoader/");
			if (Directory.Exists (Application.persistentDataPath + "/Proto/ChunkLoader/")) {
				Debug.Log ("Created directory at " + Application.persistentDataPath + "/Proto/ChunkLoader/");
			}

			LoadedChunks = new Dictionary<Location, Chunk> ();
			GameObjectParentMap = new Dictionary<Location, GameObject> ();

			GenerateGrid (GridSize);
			LoadedChunks = new Dictionary<Location, Chunk> ();
			GameObjectParentMap = new Dictionary<Location, GameObject> ();
		}

		void GenerateGrid (int count)
		{
			for (int i = -count; i <= count; i++) {
				GameObject vert = Instantiate (TilePrefab, new Vector3 ((i * ChunkSize) - .5f, 0.0f, -.1f), Quaternion.identity) as GameObject;
				vert.transform.localScale = new Vector3 (.1f, count * ChunkSize * 100, 1.0f);
				vert.GetComponent <SpriteRenderer> ().color = Color.black;

				GameObject hor = Instantiate (TilePrefab, new Vector3 (0.0f, (i * ChunkSize) - .5f, -.1f), Quaternion.identity) as GameObject;
				hor.transform.localScale = new Vector3 (count * ChunkSize * 100, .1f, 1.0f);
				hor.GetComponent <SpriteRenderer> ().color = Color.black;
			}
		}
			
		public void CharacterMoved ()
		{
			Location newChunk = PlayerChunkLocation ();
			if (!AreLocationsIdentical (GetChunkByLocation (PlayerChunkLocation ()).Location, currentPlayerChunk)) {
				LoadChunksAroundPlayer ();
			}
			currentPlayerChunk = newChunk;
			PlayerText.text = "Chunk: " + currentPlayerChunk.Coords () + "\n" + "Location: " + character.Location.Coords ();
		}

		bool AreLocationsIdentical (Location loc1, Location loc2)
		{
			return loc1.X == loc2.X && loc1.Y == loc2.Y;
		}


		public void GenerateWorld ()
		{
			if (int.TryParse (ChunkField.text, out ChunkSize)) {
				ChunkSize = int.Parse (ChunkField.text);
			} else {
				Debug.Log ("Chunk size set to default 8");
				ChunkSize = 8;
			}
			if (int.TryParse (BufferField.text, out BufferSize)) {
				BufferSize = int.Parse (BufferField.text);
			} else {
				BufferSize = 1;
			}
			if (int.TryParse (FloorField.text, out Floor)) {
				Floor = int.Parse (FloorField.text);
			} else {
				Floor = -200;
			}
			if (int.TryParse (CeilingField.text, out Ceiling)) {
				Ceiling = int.Parse (CeilingField.text);
			} else {
				Ceiling = 40;
			}

			character.Initialize (this);
			currentPlayerChunk = PlayerChunkLocation ();
			for (int i = currentPlayerChunk.X - BufferSize; i <= currentPlayerChunk.X + BufferSize; i++) {
				for (int j = currentPlayerChunk.Y - BufferSize; j <= currentPlayerChunk.Y + BufferSize; j++) {
					if (j < Mathf.FloorToInt(Ceiling / ChunkSize) && j > (Mathf.CeilToInt(Floor / ChunkSize) - 1)) {
						LoadChunk (new Location (i, j));
					}
				}
			}
			InputUI.SetActive (false);
		}

		public void Reload ()
		{
			SceneManager.LoadScene (SceneManager.GetActiveScene ().name, LoadSceneMode.Single);
		}

		public void Quit ()
		{
			Application.Quit ();
		}

		void LoadChunksAroundPlayer ()
		{
			Dictionary <Location, Chunk> neededChunks = new Dictionary<Location, Chunk> ();
			List<Location> neededLocs = new List<Location> ();
			List<Location> remainingLocs = new List<Location> ();

			currentPlayerChunk = PlayerChunkLocation ();
			Debug.Log (currentPlayerChunk.Coords ());
			for (int i = currentPlayerChunk.X - BufferSize; i <= currentPlayerChunk.X + BufferSize; i++) {
				for (int j = currentPlayerChunk.Y - BufferSize; j <= currentPlayerChunk.Y + BufferSize; j++) {
					Location chunkLoc = new Location (i, j);
					if (chunkLoc.Y < Mathf.FloorToInt(Ceiling / ChunkSize) && chunkLoc.Y > (Mathf.CeilToInt(Floor / ChunkSize) - 1)) {
						neededLocs.Add (chunkLoc);
						if (!LoadedChunks.ContainsKey (chunkLoc)) {
							LoadChunk (chunkLoc);
						}
					}
				}
			}

			foreach (KeyValuePair<Location, Chunk> pair in LoadedChunks) {
				if (!neededLocs.Contains (pair.Key)) {
					remainingLocs.Add (pair.Key);
				} 
			}
			foreach (Location loc in remainingLocs) {
				UnloadChunk (loc);
			}
		}

		void LoadChunk (Location location)
		{
			Chunk chunk = GetChunkByLocation (location);
			LoadedChunks.Add (location, chunk);
			LoadChunkVisuals (chunk);
		}

		void UnloadChunk (Location location)
		{
			Chunk chunk = GetChunkByLocation (location);
			SerializeChunk (chunk);
			LoadedChunks.Remove (location);
			Destroy (GameObjectParentMap [location]);
			GameObjectParentMap.Remove (location);
		}

		Location PlayerChunkLocation ()
		{
			int x;
			int y;
			if (character.Location.X > 0) {
				x = Mathf.FloorToInt (character.Location.X / ChunkSize);
			} else {
				x = Mathf.FloorToInt (((float)character.Location.X + .5f) / ChunkSize);
			}

			if (character.Location.Y > 0) {
				y = Mathf.FloorToInt (character.Location.Y / ChunkSize);
			} else {
				y = Mathf.FloorToInt (((float)character.Location.Y + .5f) / ChunkSize);
			}
			return new Location (x, y);
		}

		Location PlayerTileLocation ()
		{
			Location playerChunk = PlayerChunkLocation ();
			int x = character.Location.X - (ChunkSize * playerChunk.X);
			int y = character.Location.Y - (ChunkSize * playerChunk.Y);
			return new Location (x, y);
		}

		void LoadChunkVisuals (Chunk chunk)
		{
			GameObject parent = new GameObject ();
			Vector3 chunkVec = GetVector3FromLocation (chunk.Location);
			parent.transform.position = new Vector3 (chunkVec.x * ChunkSize, chunkVec.y * ChunkSize, 0.0f);
			parent.name = "Chunk " + chunk.Location.Coords ();
			GameObjectParentMap.Add (chunk.Location, parent);
			for (int i = 0; i < ChunkSize; i++) {
				for (int j = 0; j < ChunkSize; j++) {
					GameObject obj = SpawnTile (GetActualLocation (chunk.Location, new Location (i, j)), GetTileColor (chunk.GetTile (new Location (i, j)).Type));
					obj.transform.SetParent (GameObjectParentMap [chunk.Location].transform, true);
				}
			}
		}
			
		Vector3 GetVector3FromLocation (Location location)
		{
			return new Vector3 (location.X, location.Y, 0.0f);
		}

		GameObject SpawnTile (Vector3 loc, Color color)
		{
			GameObject obj = Instantiate (TilePrefab,  loc, Quaternion.identity) as GameObject;
			obj.name = "Tile " + loc.ToString ();
			obj.GetComponent<SpriteRenderer> ().color = color;
			return obj;
		}

		Vector3 GetActualLocation (Location chk, Location tile)
		{
			int x = (chk.X * ChunkSize) + tile.X;  
			int y = (chk.Y * ChunkSize) + tile.Y; 
			return new Vector3 (x, y, 0.0f);
		}

		Chunk GetChunkByLocation (Location location)
		{
			if (LoadedChunks.ContainsKey (location)) {
				return LoadedChunks [location];
			} else if (File.Exists (Application.persistentDataPath + "/Proto/ChunkLoader/" + location.X + "/" + location.Y + "/data.chunk")) {
				return DeserializeChunk (location); 
			} else {
				return GenerateNewChunk (location);
			}
		}

		void SerializeChunk (Chunk chunk)
		{
			Directory.CreateDirectory (Application.persistentDataPath + "/Proto/ChunkLoader/" + chunk.Location.X + "/" + chunk.Location.Y);

			BinaryFormatter bf = new BinaryFormatter ();
			FileStream file = File.Create (Application.persistentDataPath + "/Proto/ChunkLoader/" + chunk.Location.X + "/" + chunk.Location.Y + "/data.chunk");
			bf.Serialize (file, chunk);
			file.Close ();
			file.Dispose ();
		}

		Chunk DeserializeChunk (Location location)
		{
			BinaryFormatter bf = new BinaryFormatter ();
			FileStream file = File.Open (Application.persistentDataPath + "/Proto/ChunkLoader/" + location.X + "/" + location.Y + "/data.chunk", FileMode.Open);
			Chunk region = (Chunk)bf.Deserialize (file);
			file.Close ();
			return region;
		}

		Chunk GenerateNewChunk (Location location)
		{
			Chunk chunk = new Chunk (ChunkSize, location);
			SerializeChunk (chunk);
			return chunk;
		}

		Color GetTileColor (byte tile)
		{
			switch (tile) {
			case 0:
				return Color.white;
			case 1:
				return Color.green;
			case 2:
				return Color.yellow;
			case 3:
				return Color.red;
			default:
				return Color.black;
			}
		}
	}
}
