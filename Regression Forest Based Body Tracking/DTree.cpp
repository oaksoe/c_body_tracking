#pragma once

#include<iostream>
#include<algorithm>
#include<iterator>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<fstream>
#include<string>

#include <time.h>
#include "windows.h"

#include "DTree.h"
using namespace std;

/********************  region DecisionTree Class Member Functions   ********************/

DTree::DTree(int appStage)
{
	treeStage = appStage;

	srand (time(NULL));

	//LoadData();
	SampleSplitCandidates();

	if(treeStage == 1)	//Build tree
	{
		joints = new float[TRAINING_IMAGE_COUNT * TRAINING_JOINT_COUNT * 3];
		pixels = new float[TRAINING_IMAGE_COUNT * TRAINING_PIXEL_COUNT * 3];
		bounds = new int[TRAINING_IMAGE_COUNT * 4];
		depthMapPixelCount = new int[TRAINING_IMAGE_COUNT];
		depthMap = new unsigned short*[TRAINING_IMAGE_COUNT];
	}
	else if(treeStage == 2)	//Learn Votes
	{
		//joints = new float[TRAINING_IMAGE_COUNT * TRAINING_JOINT_COUNT * 3];
		//pixels = new float[TRAINING_IMAGE_COUNT * TRAINING_PIXEL_COUNT * 3];	 
	}
	else if(treeStage == 3)
	{
		testPixels = new float[TEST_PIXEL_COUNT * 3];
		testImage_depthMap = new float[DEPTHMAP_WIDTH * DEPTHMAP_HEIGHT];
	}

	LoadData();

	if(treeStage == 2)
	{
		R_lj = new vector<POS3D> [leafCount * TRAINING_JOINT_COUNT]; 
		V_lj = new Vote [leafCount * TRAINING_JOINT_COUNT * K_MODES];
	}
	else if(treeStage == 3)
	{
		V_lj = new Vote [leafCount * TRAINING_JOINT_COUNT * K_MODES]; 
		Z_j = new vector<Vote> [TRAINING_JOINT_COUNT]; 
	}
}

DTree::~DTree()
{
	if(treeStage == 1)	//Build tree
	{
		delete[] joints;
		delete[] pixels;
		delete[] bounds;
		delete[] depthMapPixelCount;

		for(int i = 0;i < TRAINING_IMAGE_COUNT;i++)
			delete[] depthMap[i];
		delete depthMap;
	}
}

void DTree::SampleSplitCandidates()
{
	//SAMPLED_FEATURE_DEPTH_THRESHOLDS[SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT]; 
	//SAMPLED_FEATURE_X_OFFSETS[SAMPLED_FEATURE_OFFSET_COUNT]

	for(int i = 0; i < SAMPLED_FEATURE_OFFSET_COUNT; i++)
	{
		SAMPLED_FEATURE_X_OFFSETS[i] = 0;
		SAMPLED_FEATURE_Y_OFFSETS[i] = 0;
	}

	for(int i = 0; i < SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT; i++)
	{
		SAMPLED_FEATURE_DEPTH_THRESHOLDS[i] = 0.0f;
	}

	//Sample features
	int width = 30;
	int height = 40; 
	int step = 10;

	int count = 0;

	/*SAMPLED_FEATURE_X_OFFSETS[count] = 0;
	SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
	count++;

	for(int x = step; x <= width; x+=step)
	{
		SAMPLED_FEATURE_X_OFFSETS[count] = x;
		SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
		count++;

		SAMPLED_FEATURE_X_OFFSETS[count] = x * -1;
		SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
		count++;
	}

	for(int y = step; y <= height; y+=step)
	{
		SAMPLED_FEATURE_X_OFFSETS[count] = 0;
		SAMPLED_FEATURE_Y_OFFSETS[count] = y;
		count++;

		SAMPLED_FEATURE_X_OFFSETS[count] = 0;
		SAMPLED_FEATURE_Y_OFFSETS[count] = y * -1;
		count++;
	}*/

	for(int x = step; x <= width; x+=step)
	{
		for(int y = step; y <= height; y+=step)
		{
			//First Quadrant
			SAMPLED_FEATURE_X_OFFSETS[count] = x;
			SAMPLED_FEATURE_Y_OFFSETS[count] = y;
			count++;

			//Second Quadrant
			SAMPLED_FEATURE_X_OFFSETS[count] = x * -1;
			SAMPLED_FEATURE_Y_OFFSETS[count] = y;
			count++;

			//Third Quadrant
			SAMPLED_FEATURE_X_OFFSETS[count] = x * -1;
			SAMPLED_FEATURE_Y_OFFSETS[count] = y * -1;
			count++;

			//Fourth Quadrant
			SAMPLED_FEATURE_X_OFFSETS[count] = x;
			SAMPLED_FEATURE_Y_OFFSETS[count] = y * -1;
			count++;
		}
	}

	SAMPLED_FEATURE_X_OFFSETS[count] = 0;
	SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
	count++;

	for(int x = step; x <= width; x+=step)
	{
		SAMPLED_FEATURE_X_OFFSETS[count] = x;
		SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
		count++;

		SAMPLED_FEATURE_X_OFFSETS[count] = x * -1;
		SAMPLED_FEATURE_Y_OFFSETS[count] = 0;
		count++;
	}

	for(int y = step; y <= height; y+=step)
	{
		SAMPLED_FEATURE_X_OFFSETS[count] = 0;
		SAMPLED_FEATURE_Y_OFFSETS[count] = y;
		count++;

		SAMPLED_FEATURE_X_OFFSETS[count] = 0;
		SAMPLED_FEATURE_Y_OFFSETS[count] = y * -1;
		count++;
	}

	//Sample Depth Thresholds
	int depthDiff = 1;
	float d_step = 1.0 / SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT;
	
	for(int i = 0; i < SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT; i++)
	{
		SAMPLED_FEATURE_DEPTH_THRESHOLDS[i] = d_step * (i+1);
	}

	//Debugging
	/*for(int i = 0; i < SAMPLED_FEATURE_OFFSET_COUNT; i++)
	{
		cout<<"("<<SAMPLED_FEATURE_X_OFFSETS[i]<<", "<<SAMPLED_FEATURE_Y_OFFSETS[i]<<")\n";
	}
	cout<<endl;
	for(int i = 0; i < SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT; i++)
	{
		cout<<SAMPLED_FEATURE_DEPTH_THRESHOLDS[i]<<endl;
	}*/
}

void DTree::LoadData()
{
	if(treeStage == 1)
	{
		/*
		Relationship between Boundingbox and Depthmap of Interest
		x1, y1, x2, y2 (topLeft, bottomRight)

		bb_width = x2 - x1
		bb_height = y2 - y1
		depthmap[bb_width * bb_height] = { , , , ... , }

		Suppose
		- x1, y1, x2, y2 (200, 150) (300, 200)
		- bb_width = 100, bb_height = 50
		- pixel position in 2D (200, 160)
		- absolute position (200 - 100, 160 - 150) = (10, 10)
		- index in depthmap = 10 * bb_width + 10 = 1010

		*/

		//>>>read into joints, pixels, bounds, depthMap
		string fileName[TOTAL_TRAINING_FILE_COUNT] = {"BTF Dataset Part 1.txt"};

		for(int i = 0;i < TOTAL_TRAINING_FILE_COUNT; i++)
		{			
			ifstream file_reader (fileName[i]);
			string line;
			if (file_reader.is_open())
			{
				//while ( file_reader.good() )
				{
					for(int f = 0; f < TRAINING_IMAGES_PER_FILE; f++) 
					{
						getline (file_reader,line);
						//cout << line << endl;
						getline (file_reader,line);
						//cout << line << endl;

						int image_id = i * TRAINING_IMAGES_PER_FILE + f;

						for(int j = 0;j < TRAINING_JOINT_COUNT; j++)
						{
							int id = (image_id * TRAINING_JOINT_COUNT * 3) + j * 3;
							float temp;

							file_reader>>joints[id + 0];
							file_reader>>joints[id + 1];
							file_reader>>joints[id + 2];
							file_reader>>temp;
						}

						getline (file_reader,line);
						//cout << line << endl;
						getline (file_reader,line);
						//cout << line << endl;

						int minX, minY, maxX, maxY, area;
						file_reader>>minX>>minY>>maxX>>maxY>>area;
						
						bounds[image_id * 4 + 0] = minX;
						bounds[image_id * 4 + 1] = minY;
						bounds[image_id * 4 + 2] = maxX;
						bounds[image_id * 4 + 3] = maxY;

						getline (file_reader,line);
						//cout << line << endl;
						getline (file_reader,line);
						//cout << line << endl;
						
						depthMapPixelCount[image_id] = area;
						depthMap[image_id] = new unsigned short[area];
						
						int bb_width = maxX - minX;
						int p3d_count = 0;

						for(int d = 0; d < area; d++)
						{
							float depth;
							file_reader>>depth;

							if(depth < 0)
							{
								//>>>Convert back 2D to 3D pixels
								int absX = d % bb_width;
								int absY = d / bb_width;
								int depthX = minX + absX;
								int depthY = minY + absY;
								
								POS3D p = Util::ConvertDepth2DTo3D(depthX, depthY, abs(depth));
								
								//testing/validating
								//long xx, yy; unsigned short dd;
								//Util::Convert3DToDepth2D(p, xx, yy, dd);

								if(p3d_count < TRAINING_PIXEL_COUNT)
								{
									int pos = image_id * TRAINING_PIXEL_COUNT * 3 + p3d_count * 3;
									pixels[pos + 0] = p.x;
									pixels[pos + 1] = p.y;
									pixels[pos + 2] = p.z;
									p3d_count++;
								}
							}
							
							depthMap[image_id][d] = abs(depth);
						}

						getline (file_reader,line);
						//cout << line << endl;
						getline (file_reader,line);
						//cout << line << endl;						
					}
				}
				file_reader.close();
			}
			else 
				cout << "Unable to open file";
		}		

		//Debugging
		for(int i = 0; i < 1; i++)
		{
			/*cout<<"\nJoints\n";
			for(int j = 0;j < TRAINING_JOINT_COUNT; j++)
			{
				int id = (i * TRAINING_JOINT_COUNT * 3) + j * 3;
				
				cout<<joints[id + 0]<<"\t";
				cout<<joints[id + 1]<<"\t";
				cout<<joints[id + 2]<<"\t";
			}

			cout<<"\nBounding Box\n";
			cout<<bounds[i * 4 + 0]<<"\t";
			cout<<bounds[i * 4 + 1]<<"\t";
			cout<<bounds[i * 4 + 2]<<"\t";
			cout<<bounds[i * 4 + 3]<<"\t";

			int area = depthMapPixelCount[i];

			cout<<"\nArea: "<<area<<endl;*/

			/*cout<<"\n2000 pixels\n";
			for(int p = 0; p < TRAINING_PIXEL_COUNT; p++)
			{
				int id = (i * TRAINING_PIXEL_COUNT * 3) + p * 3;
				
				if(p % 100 == 0)
					system("Pause");

				cout<<pixels[id + 0]<<"\t";
				cout<<pixels[id + 1]<<"\t";
				cout<<pixels[id + 2]<<"\n";
			}*/

			//for(int d = 0; d < area; d++)
			//{
			//	//if(d % 100 == 0)
			//		//system("Pause");
			//	cout<<depthMap[i][d]<<" ";
			//}						
		}
	}
	else if(treeStage == 2)
	{
		//>>>read into nodes and tests, testCount and leafCount

		//>>>read into joints and pixels
	}
	else if(treeStage == 3)
	{
		//>>>read into nodes and tests, testCount and leafCount

		//>>>read into V_lj

		//>>>read into testPixels
	}
}

void DTree::BuildTree()
{
	testCount = 1;
	leafCount = 1;

	//Initialize the set of indices referenced to pixels
	int splitSetSize = TRAINING_IMAGE_COUNT * TRAINING_PIXEL_COUNT; 
	int* splitSet = new int[splitSetSize];

	for(int i = 0, j = 0; i < splitSetSize; i++, j+=3)
		splitSet[i] = j;	//To dereference back to know the index of training image, divide by (TRAINING_PIXEL_COUNT * 3)

	
	Node rootNode;
	nodes.push_back(rootNode);

	BuildTreeNode(0, splitSet, splitSetSize, 0);

	//>>> Write bestTestCandidate to file
	for(int i = 0; i < nodes.size(); i++)
	{
		Node node = nodes[i];
		cout<<node.leaf_nonleaf_ID<<"\t"<<node.leftChildNodeID<<"\t"<<node.rightChildNodeID<<"\n";
	}
	
	system("pause");
}

void DTree::LearnVotes()
{
	for(int i = 0;i < TRAINING_IMAGE_COUNT * TRAINING_PIXEL_COUNT * 3; i+=3)
	{
		POS3D p3D;
		p3D.x = pixels[i];
		p3D.y = pixels[i+1];
		p3D.z = pixels[i+2];

		int imageIndex = i / (TRAINING_PIXEL_COUNT * 3);

		float* jptr = joints + (imageIndex * TRAINING_JOINT_COUNT * 3);

		//descend tree to reach leaf node l
		int leafNodeID = DescendTreeTillLeaf(i);

		for(int j = 0; j<TRAINING_JOINT_COUNT;j++)
		{						
			POS3D j3D;
			j3D.x = *jptr;
			j3D.y = *(jptr+1);
			j3D.z = *(jptr+2);						
			jptr += 3;

			POS3D iq_j = Util::CalcOffset3D(p3D, j3D);
						
			R_lj[leafNodeID * TRAINING_JOINT_COUNT + j].push_back(iq_j);
		}
	}

	//>>>Reservoir sample R_lj

	//>>>Cluster R_lj using mean shift, and take top K weighted modes as V_lj
	for(int l = 0;l < leafCount;l++)
	{
		for(int j = 0;j < TRAINING_JOINT_COUNT;j++)
		{
			vector<Vote> voteClusters = Meanshift::cluster(R_lj[l * TRAINING_JOINT_COUNT + j]); 
			
			for(int k = 0;k < voteClusters.size();k++)
				V_lj[l * TRAINING_JOINT_COUNT + j * K_MODES + k] = voteClusters[k];
		}
	}

	//>>>Store V_lj in file
}

void DTree::InferJoints()
{
	for(int i = 0;i < TEST_PIXEL_COUNT * 3; i+=3)
	{
		POS3D p;
		p.x = testPixels[i];
		p.y = testPixels[i+1];   
		p.z = testPixels[i+2];
		
		//for all trees in forests (first, test with 1 tree)
		int leafNodeID = DescendTreeTillLeaf(i);

		for(int j = 0;j < TRAINING_JOINT_COUNT;j++)
		{
			for(int k = 0;k < K_MODES; k++)
			{
				Vote vote = V_lj[leafNodeID * j + k];

				if(Util::CalcNorm3D(vote.v) <= DISTANCE_THRESHOLD_LAMDA[j])
				{
					Vote absVote;
					absVote.v.x = vote.v.x + p.x;
					absVote.v.y = vote.v.y + p.y;
					absVote.v.z = vote.v.z + p.z;

					absVote.w = vote.w * p.z;
					
					Z_j[j].push_back(absVote);
				}
			}
		}
	}

	//sub-sample Zj to contain N votes
	//aggregate Zj using mean shift on Eq. 1
	//return weighted modes as final hypotheses

	for(int j = 0;j < TRAINING_JOINT_COUNT;j++)
	{
		vector<Vote> voteClusters = Meanshift::cluster(Z_j[j]);
			
		//>>>Display joints with depth map
	}
}

/******************** endregion DecisionTree Class Member Functions ********************/


/********************  region DecisionTree Class Utility Functions   ********************/

void DTree::BuildTreeNode(int nodeID, int* splitSet, int splitSetSize, int nodeDepth)
{
	/*  Section BT => At each tree node,

		if leaf (nodeDepth == MAX_NODE_DEPTH or no of pixels < = MIN_PIXELS_AT_NODE)
			set TreeNodes[NodeID].NodeType to leaf
			set TreeNodes[NodeID].NodeRefID to -1
			set TreeNodes[NodeID].leftChildNodeID to -1
			set TreeNodes[NodeID].rightChildNodeID to -1			
		else
			find best PixelTest candidate
			insert it into NonleafNodes
			get its index in nonleafNodes as nonleafNodeID
			set TreeNodes[NodeID].NodeType to nonleaf
			set TreeNodes[NodeID].NodeRefID to nonleafNodeID
			create leftChildNode and rightChildNode
			insert them into TreeNodes
			get their indices in TreeNodes as leftChildNodeID and rightChildNodeID
			set TreeNodes[NodeID].leftChildNodeID to leftChildNodeID
			set TreeNodes[NodeID].rightChildNodeID to rightChildNodeID
			increment nodeDepth
			learnTreeNode(leftChildNodeID, leftSplitSet)
			learnTreeNode(rightChildNodeID, rightSplitSet)
		endif
	*/

	if(nodeDepth == MAX_NODE_DEPTH || splitSetSize <= MIN_PIXELS_AT_NODE) 
	{
		nodes[nodeID].leftChildNodeID = -1;
		nodes[nodeID].rightChildNodeID = -1;		
		nodes[nodeID].leaf_nonleaf_ID = -1 * leafCount;
		
		leafCount++;
		delete[] splitSet;
	}
	else
	{
		int* leftSplitSet, *rightSplitSet;
		int leftSplitSetSize = 0, rightSplitSetSize = 0;
		
		vector<int> temp_leftSplitSet, temp_rightSplitSet;
		SetPixelTestAtNonLeaf(splitSet, splitSetSize, temp_leftSplitSet, temp_rightSplitSet);

		leftSplitSetSize = temp_leftSplitSet.size();
		leftSplitSet = new int[leftSplitSetSize];
		
		for(int i = 0;i < leftSplitSetSize;i++)
			leftSplitSet[i] = temp_leftSplitSet[i];

		rightSplitSetSize = temp_rightSplitSet.size();
		rightSplitSet = new int[rightSplitSetSize];
		
		for(int i = 0;i < rightSplitSetSize;i++)
			rightSplitSet[i] = temp_rightSplitSet[i];
		
		// >>> Write bestTestCandidate to file
		
		nodes[nodeID].leaf_nonleaf_ID = testCount++;
		
		int leftChildNodeID = nodes.size();
		int rightChildNodeID = leftChildNodeID + 1;

		Node leftChildNode;
		Node rightChildNode;

		nodes.push_back(leftChildNode);
		nodes.push_back(rightChildNode);

		nodes[nodeID].leftChildNodeID = leftChildNodeID;
		nodes[nodeID].rightChildNodeID = rightChildNodeID;

		delete[] splitSet;

		BuildTreeNode(leftChildNodeID, leftSplitSet, leftSplitSetSize, nodeDepth+1);
		BuildTreeNode(rightChildNodeID, rightSplitSet, rightSplitSetSize, nodeDepth+1);
	}
}

int DTree::DescendTreeTillLeaf(int pixelID)
{
	/*  Section BT => Traversing the tree
		
		set current node id to 0 (0 is always the root index in Nodes vector)
		while the node is not leaf
			Get node data
			if node is leaf
				store iq_j in R_lj
			else
				Get pixel test from NonleafNodes
				Apply pixel test to the input pixel
				Decide which node to traverse (left child node or right child node)
				traverse to that child node
			endif
		endwhile
	*/

	bool isLeaf = false;
	int nodeID = 0;

	while(true)
	{
		Node node = nodes[nodeID];
		if(node.leaf_nonleaf_ID < 0)
		{
			int leafNodeID = abs(node.leaf_nonleaf_ID) - 1;
			return leafNodeID;
		}
		else
		{
			int nonleafNodeID = node.leaf_nonleaf_ID - 1;
			PixelTest test = tests[nonleafNodeID];
			
			bool splitToLeftNode = DoPixelTest(pixelID, test);

			if(splitToLeftNode)
				nodeID = node.leftChildNodeID;
			else
				nodeID = node.rightChildNodeID;			
		}
	}
}

void DTree::SetPixelTestAtNonLeaf(int* splitSet, int splitSetSize, vector<int>& leftSplitSet, vector<int>& rightSplitSet)
{
	/*
	Programatically, 

	for each depth threshold a,
		for each position1 u,
			for each position2 v,
				
				for each pixel in split set,
					doPixeltest()
					if left
						store in left subset
					else
						store in right subset
				endfor

				if testValue < minValue
					minValue = testValue
					minTest = currentPixelTest
				endif
			endfor
		endfor
	endfor

	*/
	
	PixelTest pt;
	double minTestValue=-1;

	vector<int> leftSubset;
	vector<int> rightSubset;
	
	int debug_counter = 0;

	SYSTEMTIME t1;
	GetSystemTime(&t1); 
	WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;

	for(int i= 0;i < SAMPLED_FEATURE_OFFSET_COUNT;i++)
	{
		//cout<<"i: "<<i<<endl;
		pt.u.x = SAMPLED_FEATURE_X_OFFSETS[i];
		pt.u.y = SAMPLED_FEATURE_Y_OFFSETS[i];
		
		for(int j= i+1;j < SAMPLED_FEATURE_OFFSET_COUNT;j++)
		{
			cout<<"i: "<<i<<"      j:"<<j<<endl;

			pt.v.x = SAMPLED_FEATURE_X_OFFSETS[j];
			pt.v.y = SAMPLED_FEATURE_Y_OFFSETS[j];

			for(int k = 0;k < SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT;k++)
			{
				cout<<"			  k: "<<k<<endl;
				pt.T = SAMPLED_FEATURE_DEPTH_THRESHOLDS[k];

				leftSubset.clear();
				rightSubset.clear();

				//time_t t1 = time(NULL);

				/*SYSTEMTIME t1;
				GetSystemTime(&t1); 
				WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;*/

				for(int l=0;l<splitSetSize;l++)
				{	
					debug_counter++;
					//Debugging   
					//printf("\n(%d, %d)  (%d, %d)  %.2f %d", pt.u.x, pt.u.y, pt.v.x, pt.v.y, pt.T, l); //splitSet[l]);

					//Do Pixel Test
					/*cout<<l<<" : "<<splitSet[l]<<endl;

					if(l == 20000)
						int i = 0;*/

					bool splitToLeftNode = DoPixelTest(splitSet[l], pt);
										
					if(splitToLeftNode)
					{
						leftSubset.push_back(splitSet[l]);
					}
					else
					{
						rightSubset.push_back(splitSet[l]);
					}
				}

				/*SYSTEMTIME t2;
				GetSystemTime(&t2);
				WORD tms2 = (t2.wSecond * 1000) + t2.wMilliseconds;
				float diff = tms2 - tms1;
				cout<<endl<<tms2<<" - "<<tms1<<" = "<<diff;

				system("pause");*/

				//Score the test
				int leftSubsetSize = leftSubset.size();
				int rightSubsetSize = rightSubset.size();

				/*SYSTEMTIME t1;
				GetSystemTime(&t1); 
				WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;*/

				//Process left subset
				POS3D avg[TRAINING_JOINT_COUNT] = {POS3D()};    //???
				int count[TRAINING_JOINT_COUNT] = {0};

				for(int l = 0;l<leftSubsetSize;l++)
				{
					int imageIndex = leftSubset[l] / (TRAINING_PIXEL_COUNT * 3);

					//Get 3D point
					float* pptr = pixels + leftSubset[l];
					POS3D p3D;
					p3D.x = *pptr;
					p3D.y = *(pptr+1);
					p3D.z = *(pptr+2);

					float* jptr = joints + (imageIndex * TRAINING_JOINT_COUNT * 3);

					for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
					{						
						POS3D j3D;
						j3D.x = *jptr;
						j3D.y = *(jptr+1);
						j3D.z = *(jptr+2);						
						jptr += 3;						

						POS3D iq_j = Util::CalcOffset3D(p3D, j3D);
						//if(Util::CalcNorm3D(iq_j) < EREG_OFFSET_THRESHOLD)
						{
							avg[m].x += iq_j.x;
							avg[m].y += iq_j.y;
							avg[m].z += iq_j.z;

							count[m]++;
						}
					}
				}

				/*SYSTEMTIME t2;
				GetSystemTime(&t2);
				WORD tms2 = (t2.wSecond * 1000) + t2.wMilliseconds;
				float diff = tms2 - tms1;
				cout<<endl<<tms2<<" - "<<tms1<<" = "<<diff;

				system("pause");*/

				/*SYSTEMTIME t1;
				GetSystemTime(&t1); 
				WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;*/

				for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
				{
					avg[m].x /= count[m];
					avg[m].y /= count[m];
					avg[m].z /= count[m];
				}

				float lscore = 0.0;
				for(int l = 0;l<leftSubsetSize;l++)
				{
					int imageIndex = leftSubset[l] / (TRAINING_PIXEL_COUNT * 3);

					//Get 3D point
					float* pptr = pixels + leftSubset[l];
					POS3D p3D;
					p3D.x = *pptr;
					p3D.y = *(pptr+1);
					p3D.z = *(pptr+2);

					float* jptr = joints + (imageIndex * TRAINING_JOINT_COUNT * 3);

					for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
					{						
						POS3D j3D;
						j3D.x = *jptr;
						j3D.y = *(jptr+1);
						j3D.z = *(jptr+2);						
						jptr += 3;

						POS3D iq_j = Util::CalcOffset3D(p3D, j3D);
						lscore += Util::CalcNorm3D(Util::CalcOffset3D(iq_j, avg[m]));
					}
				}

				/*SYSTEMTIME t2;
				GetSystemTime(&t2);
				WORD tms2 = (t2.wSecond * 1000) + t2.wMilliseconds;
				float diff = tms2 - tms1;
				cout<<endl<<tms2<<" - "<<tms1<<" = "<<diff;

				system("pause");*/

				/*SYSTEMTIME t1;
				GetSystemTime(&t1); 
				WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;*/

				//Process right subset
				POS3D ravg[TRAINING_JOINT_COUNT] = {POS3D()};    //???
				int rcount[TRAINING_JOINT_COUNT] = {0};

				for(int l = 0;l<rightSubsetSize;l++)
				{
					int imageIndex = rightSubset[l] / (TRAINING_PIXEL_COUNT * 3);

					//Get 3D point
					float* pptr = pixels + rightSubset[l];
					POS3D p3D;
					p3D.x = *pptr;
					p3D.y = *(pptr+1);
					p3D.z = *(pptr+2);

					float* jptr = joints + (imageIndex * TRAINING_JOINT_COUNT * 3);

					for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
					{						
						POS3D j3D;
						j3D.x = *jptr;
						j3D.y = *(jptr+1);
						j3D.z = *(jptr+2);						
						jptr += 3;

						POS3D iq_j = Util::CalcOffset3D(p3D, j3D);
						//if(Util::CalcNorm3D(iq_j) < EREG_OFFSET_THRESHOLD)
						{
							ravg[m].x += iq_j.x;
							ravg[m].y += iq_j.y;
							ravg[m].z += iq_j.z;

							rcount[m]++;
						}
					}
				}

				for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
				{
					ravg[m].x /= rcount[m];
					ravg[m].y /= rcount[m];
					ravg[m].z /= rcount[m];
				}

				float rscore = 0.0;
				for(int l = 0;l<rightSubsetSize;l++)
				{
					int imageIndex = rightSubset[l] / (TRAINING_PIXEL_COUNT * 3);

					//Get 3D point
					float* pptr = pixels + rightSubset[l];
					POS3D p3D;
					p3D.x = *pptr;
					p3D.y = *(pptr+1);
					p3D.z = *(pptr+2);

					float* jptr = joints + (imageIndex * TRAINING_JOINT_COUNT * 3);

					for(int m = 0; m<TRAINING_JOINT_COUNT;m++)
					{						
						POS3D j3D;
						j3D.x = *jptr;
						j3D.y = *(jptr+1);
						j3D.z = *(jptr+2);						
						jptr += 3;

						POS3D iq_j = Util::CalcOffset3D(p3D, j3D);
						rscore += Util::CalcNorm3D(Util::CalcOffset3D(iq_j, ravg[m]));
					}
				}

				/*SYSTEMTIME t1;
				GetSystemTime(&t1); 
				WORD tms1 = (t1.wSecond * 1000) + t1.wMilliseconds;*/

				float testValue = (leftSubsetSize / splitSetSize) * lscore + (rightSubsetSize / splitSetSize) * rscore;

				if(minTestValue == -1 || testValue < minTestValue)
				{
					minTestValue = testValue;
					bestTestCandidate= pt;

					leftSplitSet.clear(); 
					rightSplitSet.clear();
					copy(leftSubset.begin(), leftSubset.end(), back_inserter(leftSplitSet));
					copy(rightSubset.begin(), rightSubset.end(), back_inserter(rightSplitSet));
				}

				//time_t t2 = time(NULL);
							
				/*SYSTEMTIME t2;
				GetSystemTime(&t2);
				WORD tms2 = (t2.wSecond * 1000) + t2.wMilliseconds;
				float diff = tms2 - tms1;
				cout<<endl<<tms2<<" - "<<tms1<<" = "<<diff;

				system("pause");*/
			}
		}
	}

	SYSTEMTIME t2;
	GetSystemTime(&t2);
	WORD tms2 = (t2.wSecond * 1000) + t2.wMilliseconds;
	float diff = tms2 - tms1;
	cout<<endl<<tms2<<" - "<<tms1<<" = "<<diff;

	system("pause");

	//cout<<debug_counter;
	
}

bool DTree::DoPixelTest(int pixelID, PixelTest pt)
{
	POS3D p3D;
	float pDepth;

	if(treeStage == 3)
	{
		//Get 3D point
		float* ptr = testPixels + pixelID;
		p3D.x = *ptr;
		p3D.y = *(ptr+1);
		p3D.z = *(ptr+2);

		//Convert to 2D
		int x, y;
		//>>>convert 3D points to 2D

		pDepth = testImage_depthMap[y * DEPTHMAP_WIDTH + x];

		//Calculate feature
		int ux = x + pt.u.x / pDepth;
		int uy = y + pt.u.y / pDepth;

		int vx = x + pt.v.x / pDepth;
		int vy = y + pt.v.y / pDepth;

		float uDepth, vDepth;
		if(ux < 0 || ux > DEPTHMAP_WIDTH-1 || uy < 0 || uy > DEPTHMAP_HEIGHT-1)
			uDepth = 0.0;
		else 
			uDepth = testImage_depthMap[uy * DEPTHMAP_WIDTH + ux];

		if(vx < 0 || vx > DEPTHMAP_WIDTH-1 || vy < 0 || vy > DEPTHMAP_HEIGHT-1)
			vDepth = 0.0;
		else 
			vDepth = testImage_depthMap[vy * DEPTHMAP_WIDTH + vx];

		float feature = uDepth - vDepth;

		return feature < pt.T;
	}
	else
	{
		int imageIndex = pixelID / (TRAINING_PIXEL_COUNT * 3);

		//Get 3D point
		/*float* ptr = pixels + pixelID;
		p3D.x = *ptr;
		p3D.y = *(ptr+1);
		p3D.z = *(ptr+2);*/

		p3D.x = pixels[pixelID];
		p3D.y = pixels[pixelID + 1];
		p3D.z = pixels[pixelID + 2];
		
		//Convert to 2D
		long x, y;
		unsigned short dd;
		Util::Convert3DToDepth2D(p3D, x, y, dd);
		
		//Get Bounding Box (x1, y1, x2, y2)
		int x1, y1, x2, y2;
		x1 = bounds[imageIndex * 4];
		y1 = bounds[imageIndex * 4 + 1];
		x2 = bounds[imageIndex * 4 + 2];
		y2 = bounds[imageIndex * 4 + 3];

		int bWidth = x2 - x1;
		int bHeight = y2 - y1;

		pDepth = (depthMap[imageIndex][(y - y1) * bWidth + (x - x1)] >> 3) / 1000.0;
		
		//Calculate feature
		int ux = x + pt.u.x / pDepth;
		int uy = y + pt.u.y / pDepth;

		int vx = x + pt.v.x / pDepth;
		int vy = y + pt.v.y / pDepth;

		float uDepth, vDepth;
		if(ux < x1 || ux > x2 || uy < y1 || uy > y2)
			uDepth = 0.0;
		else 
			uDepth = (depthMap[imageIndex][(uy - y1) * bWidth + (ux - x1)] >> 3) / 1000.0;

		if(vx < x1 || vx > x2 || vy < y1 || vy > y2)
			vDepth = 0.0;
		else 
			vDepth = (depthMap[imageIndex][(vy - y1) * bWidth + (vx - x1)] >> 3) / 1000.0;

		float feature = uDepth - vDepth;

		return feature < pt.T;
	}	
}

/******************** endregion DecisionTree Class Utility Functions ********************/



