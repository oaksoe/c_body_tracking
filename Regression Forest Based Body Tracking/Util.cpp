#pragma once
#include "Util.h"

float Util::CalcNorm3D(POS3D p)
{
	return sqrt(pow(p.x * 1.0, 2) + pow(p.y * 1.0, 2) + pow(p.z * 1.0, 2));
}

POS3D Util::CalcOffset3D(POS3D p1, POS3D p2)
{
	POS3D p;
	p.x = p1.x - p2.x;
	p.y = p1.y - p2.y;
	p.z = p1.z - p2.z;

	return p;
}

void Util::Convert3DToDepth2D(POS3D p, long& lDepthX, long& lDepthY, unsigned short& usDepthValue)
{
    float width = 320.f;
	float height = 240.f;

    if(p.z > FLT_EPSILON)
    {  
        lDepthX = static_cast<int>(width / 2 + p.x * (width/320.f) * 285.63f / p.z + 0.5f);
        lDepthY = static_cast<int>(height / 2 - p.y * (height/240.f) * 285.63f / p.z + 0.5f);   

		//
        //  Depth is in meters in skeleton space.
        //  The depth image pixel format has depth in millimeters shifted left by 3.
        //
        
        usDepthValue = static_cast<unsigned short>(p.z *1000) << 3;
    }
	else
    {
        lDepthX = 0;
        lDepthY = 0;
		usDepthValue = 0;
    }
}

POS3D Util::ConvertDepth2DTo3D(long lDepthX, long lDepthY, unsigned short usDepthValue)
{
    //
    //  Depth is in meters in skeleton space.
    //  The depth image pixel format has depth in millimeters shifted left by 3.
    //
    
    float fSkeletonZ = static_cast<float>(usDepthValue >> 3) / 1000.0f;

    //
    // Center of depth sensor is at (0,0,0) in skeleton space, and
    // and (width/2,height/2) in depth image coordinates.  Note that positive Y
    // is up in skeleton space and down in image coordinates.
    //
    float width = 320.f;
	float height = 240.f;

    float fSkeletonX = (lDepthX - width/2.0f) * (320.0f/width) * (1/285.63f) * fSkeletonZ;
    float fSkeletonY = -(lDepthY - height/2.0f) * (240.0f/height) * (1/285.63f) * fSkeletonZ;

    //
    // Return the result as a vector.
    //
        
    POS3D p;
    p.x = fSkeletonX;
    p.y = fSkeletonY;
    p.z = fSkeletonZ;
    
    return p;
}