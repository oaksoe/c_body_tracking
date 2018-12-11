#include<iostream>
#include<vector>
using namespace std;

/********************  region Variable Declaration   *************************/



/******************** endregion Variable Declaration *************************/

int main()
{
	
	return 0;
}


/*
Updates
16.3.2012
---------
New    => SetPixelTestAtNonLeaf() with definitions of its utility functions inside

17.2.2012
---------
Change => Procedural to Object-oriented (Separate the DecisionTree class from main)
New	   => DecisionTree DataStructure
	   => BuildDecisionTree() and BuildTreeNode()

19.3.2012
---------
Change => Instead of storing patch id in patch index subsets, store patch's index in patches vector
New    => BuildTreeNode() Continued / Complete
	   => BuildDecisionTree() Complete

Future Problems

1. later will cause problem? is it a shallow copy or deep copy?
*/