#pragma once
#include "Meanshift.h"

vector<Vote> Meanshift::cluster(vector<POS3D> p)
{
	Vote mode;
	mode.w = -1;

	for(int i = 0;i < p.size();i++)
	{
		float ms;
		POS3D ms_p = p[i];

		int ms_iteration = 0;
		POS3D new_p;
		
		float kernel_sum = 0.0;
		int point_count;

		do
		{
			point_count = 0;
			for(int j = 0;j < p.size();j++)
			{
				float dist = Util::CalcNorm3D(Util::CalcOffset3D(ms_p, p[j]));
				if(dist < CLUSTERING_BANDWIDTH)
				{
					float kernel_p = abs((dist / CLUSTERING_BANDWIDTH) * -1);
					new_p.x += kernel_p * p[j].x;
					new_p.y += kernel_p * p[j].y;
					new_p.z += kernel_p * p[j].z;

					kernel_sum += kernel_p;
					point_count++;
				}
			}

			new_p.x /= kernel_sum;
			new_p.y /= kernel_sum;
			new_p.z /= kernel_sum;
			
			ms = Util::CalcNorm3D(Util::CalcOffset3D(ms_p, new_p));
			ms_p = new_p;

			ms_iteration++;

		}while(ms >= MS_CONVERGING_THRESHOLD && ms_iteration < MS_ITERATION_LIMIT);

		Vote curVote;
		curVote.v = ms_p;
		curVote.w = point_count;

		if(curVote.w > mode.w)
		{
			mode = curVote;
		}
	}

	vector<Vote> clusters;

	//Assume K=1;
	clusters.push_back(mode);
	
	return clusters;
}

vector<Vote> Meanshift::cluster(vector<Vote> z)
{
	Vote mode;
	mode.w = -1;

	for(int i = 0;i < z.size();i++)
	{
		float ms;
		POS3D ms_z = z[i].v;

		int ms_iteration = 0;
		POS3D new_z;
		
		float kernel_sum = 0.0;
		int point_count;

		do
		{
			point_count = 0;
			for(int j = 0;j < z.size();j++)
			{
				float dist = Util::CalcNorm3D(Util::CalcOffset3D(ms_z, z[j].v));
				if(dist < CLUSTERING_BANDWIDTH)
				{
					float kernel_z = z[j].w * abs((dist / CLUSTERING_BANDWIDTH) * -1);
					new_z.x += kernel_z * z[j].v.x;
					new_z.y += kernel_z * z[j].v.y;
					new_z.z += kernel_z * z[j].v.z;

					kernel_sum += kernel_z;
					point_count++;
				}
			}

			new_z.x /= kernel_sum;
			new_z.y /= kernel_sum;
			new_z.z /= kernel_sum;
			
			ms = Util::CalcNorm3D(Util::CalcOffset3D(ms_z, new_z));
			ms_z = new_z;

			ms_iteration++;

		}while(ms >= MS_CONVERGING_THRESHOLD && ms_iteration < MS_ITERATION_LIMIT);

		Vote curVote;
		curVote.v = ms_z;
		curVote.w = point_count;

		if(curVote.w > mode.w)
		{
			mode = curVote;
		}
	}

	vector<Vote> clusters;

	//Assume K=1;
	clusters.push_back(mode);
	
	return clusters;
}

