#pragma once
#include<vector>
#include "globals.h"
#include "Util.h"
using namespace std;

class Meanshift
{
public:
	static vector<Vote> cluster(vector<POS3D> p);

	static vector<Vote> cluster(vector<Vote> z);
};