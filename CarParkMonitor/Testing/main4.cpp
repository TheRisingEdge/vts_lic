#include <stdio.h>
#include <iostream>
#include <map>
#include "DisjSets.h"
#include <ostream>

using namespace std;


int main()
{
	DisjointSet<int*> d;

	auto i = new int(4);
	auto j = new int(5);
	auto k = new int(6);

	d.makeSet(i);
	d.makeSet(j);
	d.makeSet(k);

	d.merge(j,i);

	int* f;
	d.findSet(i, f);
	cout << *f << endl;

	d.findSet(j, f);
	cout << *f<< endl;
	
	d.findSet(k, f);
	cout << *f<< endl;

	return 0;
}