#pragma once
#include<vector>
#include<cv.h>
#include "DataStructures.h"

class Util
{
public:
	static vector<Feature> ExtractFeatures(IplImage* input_image);

	static vector<Descriptor> Util::ExtractFeatureDescriptors(IplImage* input_image); 

	static double EuclideanDistance(vector<double> d1, vector<double> d2);

	static double EuclideanDistance(KeyPoint p1, KeyPoint p2);
};