#pragma once
#include<vector>
using namespace std;

/********************  region Macros & Constants Declaration   *************************/
	const double PI = 3.141592653589793238462;

	//General
	const int TRAINING_IMAGE_COUNT = 100; //5000;
	const int TRAINING_JOINT_COUNT = 20;
	const int TRAINING_PIXEL_COUNT = 2000;
	const int MAX_NODE_DEPTH = 20;
	const int MIN_PIXELS_AT_NODE = 20;

	const int TEST_PIXEL_COUNT = 2000;
	const int DEPTHMAP_WIDTH = 320;
	const int DEPTHMAP_HEIGHT = 240;

	const int K_MODES = 1;

	//Split Test
	const int SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT = 50;	
	//float SAMPLED_FEATURE_DEPTH_THRESHOLDS[SAMPLED_FEATURE_DEPTH_THRESHOLD_COUNT]; 

	const int SAMPLED_FEATURE_OFFSET_COUNT = 63;
	//int SAMPLED_FEATURE_X_OFFSETS[SAMPLED_FEATURE_OFFSET_COUNT];
	//int SAMPLED_FEATURE_Y_OFFSETS[SAMPLED_FEATURE_OFFSET_COUNT];

	//Best Test: Ereg
	const float EREG_OFFSET_THRESHOLD = 1.10;

	//Learn Relative Votes
	const float CLUSTERING_BANDWIDTH = 0.05; //??? in meters

	//Joint Position Hypothesis Inference
	const float DISTANCE_THRESHOLD_LAMDA[TRAINING_JOINT_COUNT] = {0.1}; //??? between 0.1 and 0.55m
	const float AGGREGATION_BANDWIDTH[TRAINING_JOINT_COUNT] = {1.0}; //???
	const float MS_CONVERGING_THRESHOLD = 0.0; //???
	const int MS_ITERATION_LIMIT = 10; //???

	//File IO
	const int TOTAL_TRAINING_FILE_COUNT = 1;
	const int TRAINING_IMAGES_PER_FILE = 100;
	
	

/******************** endregion Macros & Constants Declaration *************************/

/********************  region Structures Declaration   ********************/

struct POS2D
{
	int x;
	int y;
	
	POS2D()
	{
		x = 0;
		y = 0;
	}
};

struct POS3D
{
	float x;
	float y;
	float z;

	POS3D()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
};

struct BoundingBox
{
	POS2D ulCorner;	//Upper left Corner
	POS2D lrCorner;	//Lower right Corner

	BoundingBox(){}
};

struct SplitPixelRef
{
	int tImage_index;
	int pixel_index;

	SplitPixelRef()
	{
		tImage_index = 0;
		pixel_index = 0;
	}
};

struct Node
{
	int leftChildNodeID;
	int rightChildNodeID;
	int leaf_nonleaf_ID;	//0 -> unset, 1 to n -> non-leaf ID (pixel test), -1 to -n -> leaf ID
	
	Node()
	{
		leftChildNodeID = -1;
		rightChildNodeID = -1;
		leaf_nonleaf_ID = 0;
	}
};

struct PixelTest
{
	POS2D u;
	POS2D v;
	float T;

	PixelTest()
	{
		T = 0.0;
	}

	/*void SetValues(POS2D _u, POS2D _v, float _T)
	{
		u = _u;
		v = _v;
		T = _T;
	}*/
};

struct Vote
{
	POS3D v;
	float w;

	Vote()
	{
		w = 0.0;
	}
};

/******************** endregion Structure Declaration ********************/

