#pragma once
#include "DataStructures.h"
#include "Util.h"
using namespace std;
 
class DecisionTree
{
public:

/********************  region DecisionTree Class Member Functions   ********************/

DecisionTree();

void BuildDecisionTree(vector<Patch> patches);

void BuildTreeNode(int nodeID, vector<Patch> patches);

LeafData TraverseDecisionTreeUntilLeaf(Patch patch);

/******************** endregion DecisionTree Class Member Functions ********************/

private:

/********************  region DecisionTree Class Utility Functions   ********************/

double CalculateClassUncertainty(vector<Patch> patches, vector<int>leftPatchIndexSubset, vector<int>rightPatchIndexSubset);

double CalculateOffsetUncertainty(vector<Patch> patches, vector<int>leftPatchIndexSubset, vector<int>rightPatchIndexSubset);

BinaryPixelTestResult doPixelTest(int i_pq_a, int i_rs_a);

void SetPixelTestAtNonLeaf(vector<Patch> patches);

/******************** endregion DecisionTree Class Utility Functions ********************/

/********************  region Variable Declaration   *************************/

vector<TreeNode> TreeNodes;
vector<PixelTest> NonleafNodes;
vector<LeafData> LeafNodes;
PixelTest bestTestCandidate;
vector<int> leftPatchIndexSubsetToSplit;
vector<int> rightPatchIndexSubsetToSplit;
int nodeDepth;

/******************** endregion Variable Declaration *************************/


};

/********************  region Algorithm Details   ********************/
/*

Section BT: Build Decision Tree by recursively traversing to the child nodes and build its data

Data Structures
---------------

1. Decision Tree
- vector<TreeNode> TreeNodes		: only stores ids and types (acts as a mapping structure to its child nodes as well as NonleafNodes and LeafNodes vectors) 
- vector<PixelTest> NonleafNodes	: stores actual nonleaf data which is the pixel test (a,p,q,r,s)
- vector<LeafData> LeafNodes		: stores actual leaf data which is (CL, DL)


In Tree Structure (Logically)
							 | Node 0 (c:1,2) |
						 ____________|____________
				| Node 1 (c:3,4) |		 | Node 2 (c:-1,-1) |
	        ____________|____________
 | Node 3 (c:-1,-1) |	    | Node 4 (c:5,6) |
					     ____________|____________
			 | Node 5 (c:-1,-1) |		  | Node 6 (c:-1,-1) |



In Vector Structure (Physically in Memory)

TreeNodes

		0				1					2				  3					4				  5					6
| Node 0 (c:1,2) | Node 1 (c:3,4) | Node 2 (c:-1,-1) | Node 3 (c:-1,-1) | Node 4 (c:5,6) | Node 5 (c:-1,-1) | Node 6 (c:-1,-1) | 

LeafNodes

	0		 1		  2		   3
| Node 3 | Node 5 | Node 6 | Node 2 |  

NonleafNodes

	0		 1		  2
| Node 0 | Node 1 | Node 4 | 


Algorithm
---------

At nonleaf TreeNode,
=> Create two child nodes with incomplete node data (Just nodeID is known)  
	=> How the nodeID is known? Check last index of TreeNodes. The next two indices are the nodeID of the child nodes
	=> insert them into TreeNodes 

=> Update the current node with nonleaf data
	=> Here, nonleaf data means the id of NonLeafNodes vector
	=> How to get the id of NonLeafNodes vector? Check last index of NonLeafNodes. The next index is the id.
	=> Create the nonleafNode with actual nonleaf data (best pixel test candidate) and insert it into NonLeafNodes.

=> Traverse to left child node

=> Traverse to right child node


At leaf TreeNode,
=> Update the current node with leaf data
	=> Here, leaf data means the id of LeafNodes vector
	=> How to get the id of LeafNodes vector? Check last index of LeafNodes. The next index is the id.
	=> Create the leafNode with actual leaf data (CL, DL) and insert it into LeafNodes.

=> Return

-----------------------------------------------------------------------

Section NL: Select a binary test for the non-leaf 

pick the tests, so that the uncertainties in both the class labels and the offset 
vectors decrease towards the leaves.

Given the two uncertainty measures, the binary test is chosen as follows.

Given a training set of patches, 

1. we first generate a pool of pixel tests {tk} by
sampling a, p, q, r, and s uniformly. 

	The handicap value ? for each test is chosen uniformly at random from the range
	of differences observed on the data. 

2. Then, the randomized decision is made whether the node should minimize the
class-label uncertainty or the offset uncertainty. 

	In general,
	we choose this with equal probability unless the number of
	negative patches is small (< 5%), in which case the node is
	chosen to minimize the offset uncertainty. 

3. Finally, we pick the binary test with the minimal sum of the respective uncertainty
measures for the two subsets, it splits the training
set into: 


*/
/******************** endregion Algorithm Details ********************/


