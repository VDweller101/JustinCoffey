using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PriorityQueue  {

	LinkedList<Location> queue;
	Dictionary <Location, float> values;
	int nodeCount;

	public PriorityQueue ()
	{
		queue = new LinkedList<Location> ();
		values = new Dictionary<Location, float> ();
		nodeCount = 0;
	}

	public void Enqueue (Location location, float value)
	{
		
		if (Count () == 0) {
			AddToFront (location, value);
		} else {
			if (value <= values [queue.First.Value]) {
				AddToFront (location, value);
			} else {
				LinkedListNode <Location> node = queue.First;
				float val = 0;
				while (val <= value) {
					if (node.Next == null) {
						//reached end
						break;
					} else {
						node = node.Next;
						val = values [node.Value];
					}
				}
				queue.AddAfter (node, new LinkedListNode<Location> (location));
				values.Add (location, value);
				nodeCount++;
			}
		}
	}

	public Location Dequeue ()
	{
		Location location = queue.First.Value;
		Remove (location);
		return location;
	}

	public Location Peek ()
	{
		return queue.First.Value;
	}

	public int Count ()
	{
		return nodeCount;
	}

	public bool Contains (Location location)
	{
		return queue.Contains (location);
	}

	public void UpdatePriority (Location location, float value)
	{
		Remove (location);
		Enqueue (location, value);
	}

	void AddToFront (Location location, float value)
	{
		nodeCount++;
		queue.AddFirst (location);
		values.Add (location, value);
	}

	public void Remove (Location location)
	{
		nodeCount--;
		values.Remove (location);
		queue.Remove (location);
	}
}
