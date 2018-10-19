using System.Collections.Generic;
public class PathNode  {
	public Location location;
	public List<PathEdge> edges;
	public PathNode (Location location)
	{
		this.location = location;
	}
}
