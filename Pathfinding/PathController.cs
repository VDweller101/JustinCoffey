using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathController : MonoBehaviour {

	GameController gameController;
	PathGrid pathGrid;
	GameObject Parobj;

	public void Initialize()
	{
		//Called from Game Control script during scene initialization
		//PathGrid() pulls data for tiles from DataController on Awake() and creates grid for pathfinding use
		pathGrid = new PathGrid ();
		gameController = GameController.Instance;
	}

	public Queue<Location> GetPath (Location start, Location end)
	{
		//Check if locations provided are in bounds. If not, exit.
		if (!gameController.IsInBounds (start) || !gameController.IsInBounds (end)) {
			Debug.LogError ("Tried to get a path with start or end out of bounds: S " + start.x + "," + start.z + " E " + end.x + "," + end.z);
			return null;
		} else {

			//Early exit if start and end are identical
			if (gameController.AreLocationsIdentical (start, end)) {
				path.Enqueue (start);
				return path;
			}

			//Get map size limits from GameController
			int xSize = gameController.xSize;
			int zSize = gameController.zSize;

			//Create variables used for pathfinding
			Queue<Location> path = new Queue<Location> ();
			PriorityQueue frontier = new PriorityQueue ();
			bool[,] visited = new bool[xSize,zSize];
			Dictionary<PathNode, PathNode> leadingMap = new Dictionary<PathNode, PathNode> ();
			float[,] scores = new float[xSize,zSize];

			int steps = 0;
			scores [start.x, start.z] = 0;
			Location currentPosition = start;
			PathNode currentNode;

			while (true) {
				if (gameController.AreLocationsIdentical (currentPosition, end)) {
					//When node being evaluated == end, path has been found
					break;
				} else {
					steps++;
					//To prevent freeze up, early exit if steps exceeds full tile grid size
					if (steps > xSize * ySize) {
						Debug.LogError ("Frontier while loop timed out.");
						return null;
					}
					currentNode = pathGrid.GetPathNode (currentPosition);
					foreach (PathEdge edge in currentNode.edges) {
						int x = edge.end.location.x;
						int z = edge.end.location.z;
						
						//If tile has not been visited and is not currently in frontier, score, add to frontier, and add to leadingMap.
						if (!visited [x,z] && !frontier.Contains (edge.end.location)) {
							float f = ManhattenDistance (end, currentPosition);
							float g = scores[currentPosition.x, currentPosition.z] + edge.moveCost;
							scores [x, z] = f + g;
							frontier.Enqueue (edge.end.location, scores [x, z]);
							leadingMap.Add (edge.end, currentNode);
						}
					}
					//Set visited for current tile to true and remove from frontier.
					visited [currentPosition.x, currentPosition.z] = true;
					currentPosition = frontier.Dequeue ();
				}
			}
			//To prevent freeze up, early exit if attempts exceeds full tile grid size
			int attempts = 0;
			//Path is gathered from end to start, so a stack is used before poping all locations in path in to a queue
			Stack<Location> rev = new Stack<Location> ();
			//Start at end
			currentPosition = end;
			//Start at end and use leadingMap to find all locations in best path.
			while (true) {
				attempts++;
				if (attempts > xSize * ySize) {
					Debug.LogError ("While loop for stack population timed out.");
					return null;
				}
				if (gameController.AreLocationsIdentical (currentPosition, start)) {
					break;
				}
				rev.Push (currentPosition);
				currentPosition = leadingMap [pathGrid.GetPathNode(currentPosition)].location;
			}
			int pathCount = rev.Count;
			//Reverse order of stack by popping in to queue.
			for (int i = 0; i < pathCount; i++) {
				path.Enqueue (rev.Pop ());
			}
			return path;
		} 
	}

	float ManhattenDistance (Location loc1, Location loc2)
	{
		return Mathf.Abs (loc1.x - loc2.x) + Mathf.Abs (loc1.z - loc2.z);
	}
}
