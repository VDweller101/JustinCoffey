using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathGrid  {

	public PathNode[,] pathNodes { get; protected set; }

	public PathGrid ()
	{
		//Get grid size from GameController.
		int x = GameController.Instance.xSize;
		int z = GameController.Instance.zSize;

		pathNodes = new PathNode[x,z];
		//Create all nodes
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < z; j++) {
				PathNode node = new PathNode (new Location (i, j));
				pathNodes [i, j] = node;
			}
		}
		//Create all edges
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < z; j++) {
				UpdateNodeEdges (new Location (i, j));
			}
		}
	}

	public PathNode GetPathNode (Location location)
	{
		//Check if location is in bounds first, return node at location if it is.
		if (GameController.Instance.IsInBounds (location)) {
			return pathNodes [location.x, location.z];
		} else {
			Debug.LogError ("Tried to get PathNode out of range: " + location.x + "," + location.z);
			return null;
		}
	}

	public void UpdateNodeEdges(Location location)
	{
		//When a change is made to the tile map, update all edges around tile changed
		List<PathEdge> edges = new List<PathEdge> ();
		foreach (PathNode neighbor in GetNodeNeighbors(location)) {
			edges.Add (CreatePathEdge (location, neighbor.location));
		}
		GetPathNode (location).edges = edges;
	}

	List<PathNode> GetNodeNeighbors (Location location)
	{
		//Return a list of all neighbors connected to location
		List<PathNode> neighbors = new List<PathNode> ();
		foreach (Location neighborLoc in GameController.Instance.DirectionalLocations(location)) {
			neighbors.Add (GetPathNode (neighborLoc));
		}
		return neighbors;
	}

	PathEdge CreatePathEdge (Location start, Location end)
	{
		//Move cost for any edge is the average of the movement cost from it's 2 connected tiles.
		int moveCost = (GetMovementCostForTile (start) + GetMovementCostForTile (end)) / 2;
		PathEdge edge = new PathEdge (moveCost, GetPathNode (end));
		return edge;
	}

	int GetMovementCostForTile (Location location)
	{
		TileData data = GameController.Instance.dataController.GetTileData (location);
		if (data.installedObject == null) {
			return 20;
		} else {
			switch (data.installedObject.installedObjectType) {
			case InstalledObjectType.None:
				//Blank tile
				return 20;
			case InstalledObjectType.Structure:
				//Paths are considered a Structure. Check if structure is a path and return 2, otherwise return 100 for any other structure type.
				if (data.installedObject.subType == 1) {
					return 2;
				} else {
					return 100;
				}
			case InstalledObjectType.Nature:
				return 100;
			case InstalledObjectType.Resource:
				return 40;
			default:
				//If trying to get unsupported tile type throw error and return 100 so testing can continue
				Debug.LogError ("Tried to get move cost for unsupported tile at " + location.x + "," + location.z);
				return 100;
			}
		}
	}
}
