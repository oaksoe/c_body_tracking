#include<iostream>
#include<algorithm>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<cmath>

#include "DecisionTree.h"
using namespace std;

/********************  region DecisionTree Class Member Functions   ********************/

DecisionTree::DecisionTree()
{
	srand (time(NULL));

	nodeDepth = 0;
}

void DecisionTree::BuildDecisionTree(vector<Patch> patches)
{
	TreeNode rootNode(0);
	TreeNodes.push_back(rootNode);

	BuildTreeNode(0, patches);
}

void DecisionTree::BuildTreeNode(int nodeID, vector<Patch> patches)
{
	/*  Section BT => At each tree node,

		if leaf (nodeDepth == 15 or no of patches < = 20)
			calculate leafData
			insert it into LeafNodes
			get its index in leafNodes as leafNodeID
			set TreeNodes[NodeID].NodeType to leaf
			set TreeNodes[NodeID].NodeTypeID to leafNodeID
			set TreeNodes[NodeID].leftChildNodeID to -1
			set TreeNodes[NodeID].rightChildNodeID to -1			
		else
			find best PixelTest candidate
			insert it into NonleafNodes
			get its index in nonleafNodes as nonleafNodeID
			set TreeNodes[NodeID].NodeType to nonleaf
			set TreeNodes[NodeID].NodeTypeID to nonleafNodeID
			create leftChildNode and rightChildNode
			insert them into TreeNodes
			get their indices in TreeNodes as leftChildNodeID and rightChildNodeID
			set TreeNodes[NodeID].leftChildNodeID to leftChildNodeID
			set TreeNodes[NodeID].rightChildNodeID to rightChildNodeID
			increment nodeDepth
			buildTreeNode(leftChildNodeID, leftPatches)
			buildTreeNode(rightChildNodeID, rightPatches)
		endif
	*/

	if(nodeDepth == 15 || patches.size() <= 20) 
	{
		LeafData leaf;
		int objPatchCount = 0;
		
		for(int i = 0;i<patches.size();i++)
		{
			if(patches[i].c == ClassLabel_object)
			{
				objPatchCount++;
				leaf.DL.push_back(patches[i].offset);
			}
		}

		leaf.CL = objPatchCount / patches.size();

		int leafNodeID = LeafNodes.size();
		LeafNodes.push_back(leaf);

		TreeNodes[nodeID].nodeType = TreeNodeType_leaf;
		TreeNodes[nodeID].nodeTypeID = leafNodeID;

		TreeNodes[nodeID].leftChildNodeID = -1;
		TreeNodes[nodeID].rightChildNodeID = -1;		
	}
	else
	{
		SetPixelTestAtNonLeaf(patches);
		
		int nonleafNodeID = NonleafNodes.size();
		NonleafNodes.push_back(bestTestCandidate);

		TreeNodes[nodeID].nodeType = TreeNodeType_nonleaf;
		TreeNodes[nodeID].nodeTypeID = nonleafNodeID;

		int leftChildNodeID = TreeNodes.size();
		int rightChildNodeID = leftChildNodeID + 1;

		TreeNode leftChildNode(leftChildNodeID);
		TreeNode rightChildNode(rightChildNodeID);

		TreeNodes.push_back(leftChildNode);
		TreeNodes.push_back(rightChildNode);

		TreeNodes[nodeID].leftChildNodeID = leftChildNodeID;
		TreeNodes[nodeID].rightChildNodeID = rightChildNodeID;

		nodeDepth++;

		vector<Patch> leftPatchSubset;
		vector<Patch> rightPatchSubset;

		for(int i=0;i<leftPatchIndexSubsetToSplit.size();i++)
			leftPatchSubset.push_back(patches[leftPatchIndexSubsetToSplit[i]]);

		for(int i=0;i<rightPatchIndexSubsetToSplit.size();i++)
			rightPatchSubset.push_back(patches[rightPatchIndexSubsetToSplit[i]]);

		BuildTreeNode(leftChildNodeID, leftPatchSubset);
		BuildTreeNode(rightChildNodeID, rightPatchSubset);
	}
}

LeafData DecisionTree::TraverseDecisionTreeUntilLeaf(Patch patch)
{
	/*  Section BT => Traversing the tree
		
		set current node id to 0 (0 is always the root index in TreeNodes vector)
		while the node is not leaf
			Get node data
			if node is leaf
				Get leaf data from LeafNodes
				Return CL and DL
			else
				Get pixel test from NonleafNodes
				Apply pixel test to the input patch
				Decide which node to traverse (left child node or right child node)
				traverse to that child node
			endif
		endwhile

	*/

	bool isLeaf = false;
	int nodeID = 0;

	while(true)
	{
		TreeNode node = TreeNodes[nodeID];
		if(node.nodeType == TreeNodeType_leaf)
		{
			int leafNodeID = node.nodeTypeID;
			LeafData leafData = LeafNodes[leafNodeID];
			return leafData;
		}
		else
		{
			int nonleafNodeID = node.nodeTypeID;
			PixelTest test = NonleafNodes[nonleafNodeID];
			
			int i_pq_a, i_rs_a;

			Position patch_start_pos_in_frame = Util::SearchPatchPosByPatchID(patch.id);

			Position pq_pos_in_frame(patch_start_pos_in_frame.x + test.p, patch_start_pos_in_frame.y + test.q);
			Position rs_pos_in_frame(patch_start_pos_in_frame.x + test.r, patch_start_pos_in_frame.y + test.s);

			i_pq_a = 20;	//Look up the frame's pixel value of channel r,g or b at pos (p,q)
			i_rs_a = 30;	//Look up the frame's pixel value of channel r,g or b at pos (r,s)

			if(doPixelTest(i_pq_a, i_rs_a) == BinaryPixelTestResult_left)
				nodeID = node.leftChildNodeID;
			else
				nodeID = node.rightChildNodeID;			
		}
	}
}

/******************** endregion DecisionTree Class Member Functions ********************/


/********************  region DecisionTree Class Utility Functions   ********************/

double DecisionTree::CalculateClassUncertainty(vector<Patch> patches, vector<int>leftPatchIndexSubset, vector<int>rightPatchIndexSubset)
{
	//19.3.2012 Change: Removed
	//vector<Patch> sortedPatches(patches.size());

	///*
	//Sort the patches by its ID. 
	//Alternative:
	//for each patch in patches
	//	for each patchID in leftPatchIndexSubset
	//		if patchID = patch.ID
	//			the patch is left patch and proceed processing with it

	//Overhead: 
	//If there are 1000 patches and 400 patchID => 1000 * 400 = 400000 times

	//Instead, copy the rearranged position of the patch into sortedPatches.
	//Then, the loop will only go at most 1000 times 
	//*/

	//for(int i = 0;i<patches.size();i++)
	//	sortedPatches[patches[i].id] = patches[i];
	

	/* 
			CU = Patch Size * Entropy({ci})

			Entropy({ci}) = -c * log c - (1-c) * log (1-c)

			c = (mean_c / i < 0.5 ? 0 : 1)

			mean_c = c0 + c1 + ... + ci / i

	whereas:
			CU = class uncertainty
			ci = patches[i].c
			i = patch count
	*/

	double left_CU, right_CU, testValue;
	
	//Region: Calculate Class Uncertainty for left patch subset
	int total_c = 0;
	int leftPatchCount = leftPatchIndexSubset.size();

	for(int j = 0;j<leftPatchCount;j++)
	{
		//19.3.2012 Change: Added
		total_c += patches[leftPatchIndexSubset[j]].c;
		//End

		/* 19.3.2012 Change: Removed
		total_c += sortedPatches[leftPatchIndexSubset[j]].c;
		*/
	}

	double mean_c = total_c / leftPatchCount;
	int c = (mean_c < 0.5 ? 0 : 1);

	double entropy_ci = -1 * c * log(c * 1.0) - (1-c) * log((1-c) * 1.0);
	
	left_CU = leftPatchCount * entropy_ci;
	//EndRegion: Calculate Class Uncertainty for left patch subset

	//Region: Calculate Class Uncertainty for right patch subset
	total_c = 0;
	int rightPatchCount = rightPatchIndexSubset.size();

	
	for(int j = 0;j<rightPatchCount;j++)
	{
		//19.3.2012 Change: Added
		total_c += patches[rightPatchIndexSubset[j]].c;
		//End

		/* 19.3.2012 Change: Removed
		total_c += sortedPatches[rightPatchIndexSubset[j]].c;
		*/
	}

	mean_c = total_c / rightPatchCount;
	c = (mean_c < 0.5 ? 0 : 1);

	entropy_ci = -1 * c * log(c * 1.0) - (1-c) * log((1-c) * 1.0);
	
	right_CU = rightPatchCount * entropy_ci;
	//EndRegion: Calculate Class Uncertainty for right patch subset

	testValue = left_CU + right_CU;

	return testValue;
}

double DecisionTree::CalculateOffsetUncertainty(vector<Patch> patches, vector<int>leftPatchIndexSubset, vector<int>rightPatchIndexSubset)
{
	/*19.3.2012 Change: Removed
	vector<Patch> sortedPatches(patches.size());

	for(int i = 0;i<patches.size();i++)
		sortedPatches[patches[i].id] = patches[i];
	*/

	/*
		OU = [Summation(i:ct = 1)] (di - dA) ^ 2  

	whereas 
		OU = Offset Uncertainty
		di = patch offset
		dA = mean offset over all object patches in the set
		i  = patch count
	
	Note: bg patches (ci = 0) are ignored here
	*/

	double left_OU, right_OU, testValue;

	//Region: Calculate Offset Uncertainty for left patch subset
	int total_d = 0;
	int leftPatchCount = leftPatchIndexSubset.size();

	int objPatchCount = 0;
	for(int j = 0;j<leftPatchCount;j++)
	{
		//19.3.2012 Change: Added
		Patch tempPatch = patches[leftPatchIndexSubset[j]];
		//End

		//19.3.2012 Change: Removed
		//Patch tempPatch = sortedPatches[leftPatchIndexSubset[j]];

		if(tempPatch.c == ClassLabel_object)
		{
			total_d += tempPatch.offset.d;
			objPatchCount++;
		}
	}

	double dA = total_d / objPatchCount;

	left_OU = 0;
	for(int j = 0;j<leftPatchCount;j++)
	{
		//19.3.2012 Change: Added
		Patch tempPatch = patches[leftPatchIndexSubset[j]];
		//End

		////19.3.2012 Change: Removed
		//Patch tempPatch = sortedPatches[leftPatchIndexSubset[j]];

		if(tempPatch.c == ClassLabel_object)
		{
			left_OU += pow(tempPatch.offset.d - dA, 2);
		}
	}
	//EndRegion: Calculate Offset Uncertainty for left patch subset

	//Region: Calculate Offset Uncertainty for left patch subset
	total_d = 0;
	int rightPatchCount = rightPatchIndexSubset.size();

	objPatchCount = 0;
	for(int j = 0;j<rightPatchCount;j++)
	{
		//19.3.2012 Change: Added
		Patch tempPatch = patches[leftPatchIndexSubset[j]];
		//End

		//19.3.2012 Change: Removed
		//Patch tempPatch = sortedPatches[rightPatchIndexSubset[j]];

		if(tempPatch.c == ClassLabel_object)
		{
			total_d += tempPatch.offset.d;
			objPatchCount++;
		}
	}

	dA = total_d / objPatchCount;

	right_OU = 0;
	for(int j = 0;j<rightPatchCount;j++)
	{
		//19.3.2012 Change: Added
		Patch tempPatch = patches[leftPatchIndexSubset[j]];
		//End

		//19.3.2012 Change: Removed
		//Patch tempPatch = sortedPatches[rightPatchIndexSubset[j]];

		if(tempPatch.c == ClassLabel_object)
		{
			right_OU += pow(tempPatch.offset.d - dA, 2);
		}
	}	
	//EndRegion: Calculate Offset Uncertainty for left patch subset

	testValue = left_OU + right_OU;

	return testValue;
}

BinaryPixelTestResult DecisionTree::doPixelTest(int i_pq_a, int i_rs_a)
{
	if(i_pq_a < i_rs_a + PIXEL_TEST_THRESHOLD)
		return BinaryPixelTestResult_left;
	return BinaryPixelTestResult_right;
}

void DecisionTree::SetPixelTestAtNonLeaf(vector<Patch> patches)
{
	/* Section NL => Step 1: Sample a, p, q, r, s uniformly
	a - no of channels (r, g, b)
	p, q - position (x1,y1)
	r, s - position (x2,y2)
	
	Possible pairs of two positions (p,q) and (r,s)
	(0,0) (0,1)
	      (0,2)
		  ...
		  (0,15)	//count = 15
		  (1,0)
		  (1,1)
		  ...
		  (1,15)	//count = 15 + 16
		  ...
		  (2,15)    //count = 15 + 16 + 16
		  ...
		  (3,15)	//count = 15 + 16 + 16 + 16
		   .
		   .
		   .
		  (15,15)	//count = 255
	
	(0,1) (0,2)
	      ...
		  (0,15)	//count = 14
		   .
		   .
		   .
		  (15,15)   //count = 254

	.
	.
	.

	(15,14) (15,15)	//count = 1

	Total samples for possible positions of p,q,r,s = 1+2+...+ 255 = (255 + 256) / 2 = 32640
	total no of tests resulted from uniform sampling (a,p,q,r,s) = 32640 * 3 = 97920

	Programatically, 

	for each color channel a,
		for each position1 (p,q),
			for each position2 (r,s),
				
				for each patch,
					doPixeltest()
					if left
						store in left subset
					else
						store in right subset
				endfor

				getNegativePatchCountInPercentage()

				if negativePatchCountInPercentage < 5%
					testValue = calcOffsetUncertainty(leftSubset, rightSubset)
				else 
					getRandValue(0,1)
					if randValue is 0
						testValue = calcClassUncertainty(leftSubset, rightSubset)
					else
						testValue = calcOffsetUncertainty(leftSubset, rightSubset)
					endif
				endif

				if testValue < minValue
					minValue = testValue
					minTest = currentPixelTest
				endif
			endfor
		endfor
	endfor

	*/
	int p,q,r,s;
	double minTestValue=-1;

	vector<int> leftPatchIndexSubset;
	vector<int> rightPatchIndexSubset;

	for(int a=0;a<3;a++)
	{
		for(int pos1= 0;pos1 < PATCH_WIDTH * PATCH_HEIGHT;pos1++)
		{
			p = pos1 % PATCH_WIDTH;
			q = pos1 / PATCH_WIDTH;

			for(int pos2= pos1+1;pos2 < PATCH_WIDTH * PATCH_HEIGHT;pos2++)
			{
				r = pos2 % PATCH_WIDTH;
				s = pos2 / PATCH_WIDTH;

				leftPatchIndexSubset.clear();
				rightPatchIndexSubset.clear();

				for(int i=0;i<patches.size();i++)
				{
					Position patch_start_pos_in_frame = Util::SearchPatchPosByPatchID(patches[i].id);

					Position pq_pos_in_frame(patch_start_pos_in_frame.x + p, patch_start_pos_in_frame.y + q);
					Position rs_pos_in_frame(patch_start_pos_in_frame.x + r, patch_start_pos_in_frame.y + s);

					int i_pq_a = 20;	//Look up the frame's pixel value of channel r,g or b at pos (p,q)
					int i_rs_a = 30;	//Look up the frame's pixel value of channel r,g or b at pos (r,s)

					if(doPixelTest(i_pq_a, i_rs_a) == BinaryPixelTestResult_left)
					{
						//19.3.2012 Change: Added
						leftPatchIndexSubset.push_back(i);
						//End
						
						//19.3.2012 Change: Removed
						//leftPatchIndexSubset.push_back(patches[i].id);
						//
					}
					else
					{
						//19.3.2012 Change: Added
						rightPatchIndexSubset.push_back(i);
						//End

						//19.3.2012 Change: Removed
						//rightPatchIndexSubset.push_back(patches[i].id);
						//
					}
				}

				//Region: Get Negative (Background) Patches Count
				int bg_patch_count = 0;
				for(int i=0;i<patches.size();i++)
				{
					if(patches[i].c == ClassLabel_background)
						bg_patch_count++;
				}
				//EndRegion: Get Negative (Background) Patches Count
				double testValue;

				if(bg_patch_count < (int) (patches.size() * 0.05))
					testValue = CalculateOffsetUncertainty(patches, leftPatchIndexSubset, rightPatchIndexSubset);
				else 
				{
					int randValue = rand() % 2;
					if(randValue == 0)
						testValue = CalculateClassUncertainty(patches, leftPatchIndexSubset, rightPatchIndexSubset);
					else
						testValue = CalculateOffsetUncertainty(patches, leftPatchIndexSubset, rightPatchIndexSubset);
				}

				if(minTestValue == -1 || testValue < minTestValue)
				{
					minTestValue = testValue;
					bestTestCandidate.SetValues(a, p, q, r, s);

					leftPatchIndexSubsetToSplit.clear(); 
					rightPatchIndexSubsetToSplit.clear();
					copy(leftPatchIndexSubset.begin(), leftPatchIndexSubset.end(), leftPatchIndexSubsetToSplit.begin());
					copy(rightPatchIndexSubset.begin(), rightPatchIndexSubset.end(), rightPatchIndexSubsetToSplit.begin());
				}
			}
		}
	}
}

/******************** endregion DecisionTree Class Utility Functions ********************/