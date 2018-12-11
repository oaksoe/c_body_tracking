#include "HoughForest.h"
using namespace std;

/********************  region DecisionTree Class Member Functions   ********************/

HoughForest::HoughForest()
{
	
}

void HoughForest::Train(vector<Image> TrainingSet)
{
	/*  Section HF => Training
	    Assume that each set of patches is obtained from each training image
		For each set of patches, build a decision tree, use the set as input
		Save the decison trees data into the file
	*/

	for(int i = 0;i<TrainingSet.size();i++)
	{
		DecisionTree DT;
		
		vector<Patch> patches = ExtractPatches(TrainingSet[i]);
		DT.BuildDecisionTree(patches);

		//Write to file (DT.TreeNodes, DT.LeafNodes, DT.NonLeafNodes)
	}
}

Position HoughForest::DetectObject(Image frame)
{
	vector<Patch> patches = ExtractPatches(frame);
	vector<DecisionTree> trees;
		
	//Read from file (TreeNodes, LeafNodes, NonLeafNodes)
	int DTCount = 0; //Read from file
	
	for(int i = 0;i<DTCount; i++)
	{
		DecisionTree tree;
		//tree.TreeNodes = ;
		//tree.LeafNodes = ;
		//tree.NonleafNodes = ;

		trees.push_back(tree);
	}

	/*	Section HF => Detection
		Initialize HoughImage to 0(vector of size 640*480)

		for each c (center of patch)
			for each decision tree
				traverse the tree until leaf
				get leaf data(CL, DL) from the tree
				for each d in DL
					pos_in_hough_image = c - d
					HoughImage[pos_in_hough_image] += (CL / sizeof(DL))
				endfor
			endfor
		endfor

		GaussianFilter the votes accumulated in hough image
		... (check the vote result)
		Select the pixel with highest votes
	*/

	double HoughImage[FRAME_WIDTH*FRAME_HEIGHT];
	for(int i = 0;i<FRAME_WIDTH*FRAME_HEIGHT;i++)
		HoughImage[i] = 0;

	for(int i = 0;i<patches.size();i++)
	{
		Position patch_start_pos_in_frame = Util::SearchPatchPosByPatchID(patches[i].id);

		Position c(patch_start_pos_in_frame.x + PATCH_CENTER_POS_X, patch_start_pos_in_frame.y + PATCH_CENTER_POS_Y);

		for(int j = 0;j < trees.size();j++)
		{
			LeafData leaf = trees[j].TraverseDecisionTreeUntilLeaf(patches[i]);
			for(int d = 0;d < leaf.DL.size();d++)
			{
				Position pos(c.x - leaf.DL[d].dx, c.y - leaf.DL[d].dy);
				HoughImage[pos.y * FRAME_WIDTH + pos.x] += leaf.CL / leaf.DL.size();
			}
		}
	}

	vector<double> HoughVotes;
	for(int i = 0;i< FRAME_WIDTH*FRAME_HEIGHT;i++)
	{
		if(HoughImage[i] > 0)
		{
			HoughVotes.push_back(HoughImage[i]);
		}
	}

	double mean = 0, variance = 0;
	for(int i = 0;i < HoughVotes.size();i++)
		mean += HoughVotes[i];
	
	mean /= HoughVotes.size();

	for(int i = 0;i< HoughVotes.size();i++)
		variance += pow(HoughVotes[i] - mean, 2);

	variance /= (HoughVotes.size() - 1);

	//Gaussian Filter Function
	for(int i = 0;i< HoughVotes.size();i++)
		HoughVotes[i] = (1 / (2 * PI * variance)) * exp(-1 * (HoughVotes[i] - mean) / (2 * variance));

	//To be continued
}

/******************** endregion DecisionTree Class Member Functions ********************/

/********************  region DecisionTree Class Utility Functions   ********************/

vector<Patch> HoughForest::ExtractPatches(Image image)
{
	vector<Patch> patches;
	return patches;
}

/********************  region DecisionTree Class Utility Functions   ********************/