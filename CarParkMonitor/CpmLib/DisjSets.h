#pragma once
#include <vector>
#include <map>

using namespace std;

template<typename T>
class Node {		
public:		
	int rank;		
	T parent;
	Node(T p, int r):parent(p), rank(r){}
	Node(){};
};

template<typename T>
class DisjointSet {
private:
	std::map<T, Node<T>> objectsToNodes;

public:
	bool findSet(T o, T& found) 
	{
		auto it = objectsToNodes.find(o);
		if(it == objectsToNodes.end())
			return false;

		Node<T>& node = it->second;
		if(o != node.parent)
		{
			T f;
			findSet(node.parent, f);
			node.parent = f;
		}			

		found = node.parent;
		return true;
	}

	void makeSet(T o) {
		objectsToNodes[o] =  Node<T>(o,0);
	}

	void merge(T x, T y) {
		T setX; T setY;

		bool xFound = findSet(x, setX);
		bool yFound = findSet(y, setY);

		if (xFound == false || yFound == false || setX == setY)
			return;

		Node<T>& nodeX = objectsToNodes[setX];
		Node<T>& nodeY = objectsToNodes[setY];
		
		//join the two sets by pointing the root of one at the root of the other
		if (nodeX.rank > nodeY.rank) {
			nodeY.parent = x;
		} else {
			nodeX.parent = y;
			if (nodeX.rank == nodeY.rank)
				nodeY.rank++;
		}
	}
};