#include "DataStructures.h"

class K_Means
{
public:

/********************  region K_Means Class Member Functions   ********************/

K_Means();

vector<Cluster> process_K_Means(vector<Feature>sample, int k);

/******************** endregion K_Means Class Member Functions ********************/

private:

/********************  region K_Means Class Utility Functions   ********************/

int CheckSeeds(int i, vector<int> seeds, int tempSeed, int sampleSize);
	
double CalcEuclideanDist(Position point, Position meanPoint);

/******************** endregion K_Means Class Utility Functions ********************/

/********************  region Variable Declaration   *************************/

/******************** endregion Variable Declaration *************************/


};



	
