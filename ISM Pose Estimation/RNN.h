#include "DataStructures.h"

class RNN
{
public:

/********************  region RNN Class Member Functions   ********************/

RNN();

vector<CodeVector> Process_RNN(vector<Descriptor> sample);


/******************** endregion RNN Class Member Functions ********************/

private:

/********************  region RNN Class Utility Functions   ********************/

int GetNearestNeighbor(vector<Descriptor> sample, vector<RNN_Cluster> clusterPoints, int l, vector<int> R, double& sim, int& foundIndex);

RNN_Cluster Agglomerate(vector<Descriptor> sample, vector<RNN_Cluster> clusterPoints, int l1, int l2);

/******************** endregion RNN Class Utility Functions ********************/

/********************  region Variable Declaration   *************************/

/******************** endregion Variable Declaration *************************/


};
/*
RNN (Reciprocal Nearest Neighbours)

Ref: Robust Object Detection with Interleaved Categorization and Segmentation.pdf

Agglomerate and MeasureSimilarity functions are based on Equations (5) and (6) on Page 6 


*/