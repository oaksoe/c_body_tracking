#pragma once
#include "globals.h"

class Util
{
public:
	static float CalcNorm3D(POS3D p);

	static POS3D CalcOffset3D(POS3D p1, POS3D p2);

	static void Convert3DToDepth2D(POS3D p, long& lDepthX, long& lDepthY, unsigned short& usDepthValue);

	static POS3D ConvertDepth2DTo3D(long lDepthX, long lDepthY, unsigned short usDepthValue);
};