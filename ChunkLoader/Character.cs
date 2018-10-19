using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace ProtoChunkLoader {
	public class Character : MonoBehaviour {

		ChunkLoader loader;

		public Location Location;
		public GameObject PlaceObject;
		public bool android;

		Location lastLocation;

		bool initialized;
		bool moveMode;

		void Start()
		{
			PlaceObject.transform.position = new Vector3 (Location.X, Location.Y, -.1f);
		}

		public void Initialize (ChunkLoader loader)
		{
			this.loader = loader;
			lastLocation = Location;
			UpdateObjectLocation ();
			PlaceObject.GetComponent <SpriteRenderer> ().color = Color.blue;
			initialized = true;
		}

		void Update()
		{
			if (initialized) {
				if (android) {
					if (Input.GetMouseButton (0)) {
						Vector3 mouseLoc = Camera.main.ScreenToWorldPoint (Input.mousePosition) - this.gameObject.transform.position;
						mouseLoc = mouseLoc.normalized;
						int x = Mathf.RoundToInt (mouseLoc.x);
						int y = Mathf.RoundToInt (mouseLoc.y);
						Location.X += x;
						if (y > 0) {
							if (Location.Y < loader.Ceiling) {
								Location.Y += y;
							}
						} else if (y < 0) {
							if (Location.Y > loader.Floor) {
								Location.Y += y;
							}
						}
						UpdateObjectLocation ();
						lastLocation = Location;
					}
				} else {
					if (Input.GetKeyDown (KeyCode.R)) {
						moveMode = !moveMode;
					}
					Location = lastLocation;
					if (moveMode) {
						MoveConstant ();
					} else {
						MoveSingle ();
					}
					UpdateObjectLocation ();
					lastLocation = Location;
				}
			}
		}

		void MoveConstant ()
		{
			if (Input.GetKey (KeyCode.W) && Location.Y < loader.Ceiling) {
				Location.Y++;
			} 
			if (Input.GetKey (KeyCode.S) && Location.Y > loader.Floor) {
				Location.Y--;
			} 
			if (Input.GetKey (KeyCode.D)) {
				Location.X++;
			} 
			if (Input.GetKey (KeyCode.A)) {
				Location.X--;
			}
		}

		void MoveSingle ()
		{
			if (Input.GetKeyDown (KeyCode.W)) {
				Location.Y++;
			} 
			if (Input.GetKeyDown (KeyCode.S)) {
				Location.Y--;
			} 
			if (Input.GetKeyDown (KeyCode.D)) {
				Location.X++;
			} 
			if (Input.GetKeyDown (KeyCode.A)) {
				Location.X--;
			}
		}

		void UpdateObjectLocation ()
		{
			if (lastLocation.X != Location.X || lastLocation.Y != Location.Y) {
				PlaceObject.transform.position = new Vector3 (Location.X, Location.Y, -.1f);
				loader.CharacterMoved ();
			}
		}
	}
}
