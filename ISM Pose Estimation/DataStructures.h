#pragma once
#include<vector>
#include<cv.h>
using namespace std;
/********************  region Data Structures Declaration   ********************/

struct KeyPoint
{
	int x;
	int y;
	int s;

	KeyPoint()
	{
		x = 0;
		y = 0;
		s = 0;
	} 

	KeyPoint(int _x, int _y, int _s)
	{
		x = _x;
		y = _y;
		s = _s;
	} 
};

struct Vote
{
	KeyPoint x;
	KeyPoint occ;
	KeyPoint l;
	double w;

	Vote()
	{
		w = 0.0;
	}

	Vote(KeyPoint _x, KeyPoint _occ, KeyPoint _l, double _w)
	{
		x = _x;
		occ = _occ;
		l = _l;
		w = _w;
	}
};

struct Match
{
	int i;
	KeyPoint l;
	double weight;

	Match()
	{
		i = 0;
		weight = 0.0;
	}

	Match(int _i, KeyPoint _l, double _weight)
	{
		i = _i;
		l = _l;
		weight = _weight;
	}
};

typedef vector<double> Descriptor;

struct TrainingImage
{
	IplImage* img;
	int cx;
	int cy;

	TrainingImage()
	{
		cx = 0;
		cy = 0;
	}

	TrainingImage(IplImage* _img, int _cx, int _cy)
	{
		img = _img;
		cx = _cx;
		cy = _cy;
	}
};

struct Feature
{
	KeyPoint kp;
	Descriptor descriptor;
	
	Feature(){}

	Feature(KeyPoint _kp, Descriptor _descriptor)
	{	
		kp = _kp;
		descriptor = _descriptor;
	}
};

struct RNN_Cluster
{
	Descriptor mean;
	double variance;
	int count;

	RNN_Cluster()
	{
		variance = 0;
		count = 0;
	}

	RNN_Cluster(Descriptor _mean, double _variance, int _count)
	{
		mean = _mean;
		variance = _variance;
		count = _count;
	}
};

struct CodeVector
{
	int id;
	Descriptor descriptor;
	int confidence;
	vector<KeyPoint> occurrence;

	CodeVector()
	{
		id = 0;
		confidence = 0;
	}

	CodeVector(int _id, Descriptor _descriptor, int _confidence, vector<KeyPoint> _occurrence)
	{
		id = _id;
		descriptor = _descriptor;
		confidence = _confidence;
		occurrence = _occurrence;
	}
};


/******************** endregion Data Structure Declaration ********************/

/********************  region Macros & Constants Declaration   *************************/

#define RNN_THRESHOLD 10
#define IMAGE_WIDTH	640
#define IMAGE_HEIGHT 480
#define FEATURE_SCALE 100
#define BIN_SIZE_X 16
#define BIN_SIZE_Y 16
#define BIN_SIZE_S 10
#define BIN_COUNT_X IMAGE_WIDTH / BIN_SIZE_X
#define BIN_COUNT_Y IMAGE_HEIGHT / BIN_SIZE_Y
#define BIN_COUNT_S FEATURE_SCALE / BIN_SIZE_S
#define BIN_COUNT BIN_COUNT_X * BIN_COUNT_Y * BIN_COUNT_S
#define KERNEL_BOUND 1.0
#define KERNEL_BANDWIDTH 10 //5% of hypothesized object size
#define KERNEL_VOLUME 100 // ???
#define VOTE_BIN_THRESHOLD 10
#define HYPOTHESIS_THRESHOLD 100 //THETA
#define MEANSHIFT_CONVERGING_THRESHOLD 100 //EPSILON

/******************** endregion Macros & Constants Declaration *************************/