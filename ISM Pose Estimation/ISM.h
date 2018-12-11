#pragma once
#include<cv.h>
#include "DataStructures.h"
using namespace std;

class ISM
{
public:

/********************  region ISM Class Member Functions   ********************/

ISM();

void Train(vector<TrainingImage> TrainingSet);

/******************** endregion ISM Class Member Functions ********************/

private:

/********************  region ISM Class Utility Functions   ********************/

void GenerateVotes(IplImage* TestImage);

void ScaleAdpativeHypothesisSearch();

double Gaussian_Kernel(double x);

void Get_3D_Bin_Indices(int a, int &x, int &y, int &s);

int Get_1D_Bin_Index(int x, int y, int s);

/******************** endregion ISM Class Utility Functions ********************/

/********************  region Variable Declaration   *************************/

vector<CodeVector> Codebook;

/******************** endregion Variable Declaration *************************/

vector<Vote> V;

};



/********************  region Algorithm Details   ********************/
/*

Section TRAIN:	

1. Sample SIFT interest points from a set of sample images of persons
2. Perform agglomerative clustering on corresponding SIFT features to gain codevectors
3. Record positions of person center relative to codevectors to learn Pc

Section TEST:	

1. Sample SIFT interest points from test image
2. Assign SIFT descriptor vectors to codevectors
3. Use the probability distribution Pc to cast votes for the person center


having recognized an image feature
known from the codebook as codevector v with v = 1, ..., S
where S is the number of codevectors,

ISM to find person center
Train
- extract features from each frame
- -
-

Test
- extract features from test frame
- how to assign the features to the cluster/codevector (need to read more about codevectors)

ISM to find person's body parts
- extract person features from each frame
- for each marker, cluster the features collection from all training frames by offsets to the marker

need to read about mean shift clustering

Section TRAIN:





*/

