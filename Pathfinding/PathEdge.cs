public struct PathEdge  {
	public int moveCost;
	public PathNode end;
	public PathEdge (int moveCost, PathNode end)
	{
		this.moveCost = moveCost;
		this.end = end;
	}
}
