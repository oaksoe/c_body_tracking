#pragma once
#include<vector>
#include<cmath>
using namespace std;

/********************  region Data Structures Declaration   ********************/
enum ClassLabel
{
	ClassLabel_background,
	ClassLabel_object
};

enum BinaryPixelTestResult
{
	BinaryPixelTestResult_left,
	BinaryPixelTestResult_right
};

enum TreeNodeType
{
	TreeNodeType_leaf,
	TreeNodeType_nonleaf
};

struct Image
{
	int i;

	Image(){}

	Image(int _i)
	{}
};

struct TreeNode
{
	int nodeID;
	int leftChildNodeID;
	int rightChildNodeID;
	TreeNodeType nodeType;	//leaf or non-leaf
	int nodeTypeID;		//leaf ID or non-leaf ID

	TreeNode(){}

	TreeNode(int _nodeID)
	{
		nodeID = _nodeID;
		leftChildNodeID = -1;
		rightChildNodeID = -1;
		nodeType = TreeNodeType_nonleaf;
		nodeTypeID = -1;
	}
};

struct Position
{
	int x;
	int y;

	Position()
	{
		x = 0;
		y = 0;
	} 

	Position(int _x, int _y)
	{
		x = _x;
		y = _y;
	} 
};

struct Offset
{
	int dx;
	int dy;
	double d;

	Offset()
	{
		dx = 0;
		dy = 0;
		d = 0.0;
	}

	Offset(int _dx, int _dy)
	{
		dx = _dx;
		dy = _dy;
		d = sqrt(pow(dx * 1.0, 2) + pow(dy * 1.0, 2)); 
	}
};

struct Patch
{
	int id;
	Offset offset;
	ClassLabel c;

	Patch(){}

	Patch(int _id, Offset _offset, ClassLabel _c)
	{
		id = _id;
		offset = _offset;
		c = _c;
	}
};

struct PixelTest
{
	int a;
	int p;
	int q;
	int r;
	int s;

	PixelTest()
	{
		a = 0;
		p = 0;
		q = 0;
		r = 0;
		s = 0;
	}

	PixelTest(int _a, int _p, int _q, int _r, int _s)
	{
		a = _a;
		p = _p;
		q = _q;
		r = _r;
		s = _s;
	}

	void SetValues(int _a, int _p, int _q, int _r, int _s)
	{
		a = _a;
		p = _p;
		q = _q;
		r = _r;
		s = _s;
	}
};

struct LeafData
{
	double CL;
	vector<Offset> DL;

	LeafData(){}

	LeafData(double _CL, vector<Offset> _DL)
	{
		CL = _CL;
		DL = _DL;	//??? Future Problem 1 
	}
};

//struct ImageSize
//{
//	int width;
//	int height;
//
//	ImageSize()
//	{
//		width = 0;
//		height = 0;
//	}
//
//	ImageSize(int _width, int _height)
//	{
//		width = _width;
//		height = _height;
//	}
//};
//
//typedef ImageSize PatchSize;

//struct PatchAppearance
//{
//	int r;
//	int g;
//	int b;
//
//	PatchAppearance()
//	{
//		r = 0;
//		g = 0;
//		b = 0;
//	}
//
//	PatchAppearance(int _r, int _g, int _b)
//	{
//		r = _r;
//		g = _g;
//		b = _b;
//	}
//};

/******************** endregion Data Structure Declaration ********************/

/********************  region Macros & Constants Declaration   *************************/

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define PATCH_WIDTH 16
#define PATCH_HEIGHT 16
#define PIXEL_TEST_THRESHOLD 10
#define PATCH_ROWS_PER_FRAME FRAME_WIDTH/PATCH_WIDTH
#define PATCH_COLS_PER_FRAME FRAME_HEIGHT/PATCH_HEIGHT
#define PATCH_CENTER_POS_X PATCH_WIDTH/2
#define PATCH_CENTER_POS_Y PATCH_HEIGHT/2

const double PI  =3.141592653589793238462;

/******************** endregion Macros & Constants Declaration *************************/