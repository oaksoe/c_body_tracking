#pragma once
#include "DataStructures.h"
#include "DecisionTree.h"
using namespace std;
 
class HoughForest
{
public:

/********************  region DecisionTree Class Member Functions   ********************/

HoughForest();

void Train(vector<Image> TrainingSet);

Position DetectObject(Image frame);

/******************** endregion DecisionTree Class Member Functions ********************/

private:

/********************  region DecisionTree Class Utility Functions   ********************/

vector<Patch> ExtractPatches(Image image);

/******************** endregion DecisionTree Class Utility Functions ********************/

/********************  region Variable Declaration   *************************/



/******************** endregion Variable Declaration *************************/


};