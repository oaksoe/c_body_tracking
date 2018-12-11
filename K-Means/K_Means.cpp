#pragma once
#include<cstdlib>
#include<ctime>

#include "K_Means.h"

K_Means::K_Means()	
{
	srand (time(NULL));
}
	
int K_Means::CheckSeeds(int i, vector<int> seeds, int currentSeed, int sampleSize)
{	
	for(int j = 0;j < i;j++)
	{
		if(currentSeed == seeds[j])
		{
			currentSeed = rand() % sampleSize;
			return CheckSeeds(i, seeds, currentSeed, sampleSize);
		}
	}
	return currentSeed;
}
	
double K_Means::CalcEuclideanDist(Position point, Position meanPoint){	
	return sqrt(pow((meanPoint.x - point.x)*1.0, 2) + pow((meanPoint.y - point.y)*1.0, 2));
}

vector<Cluster> K_Means::process_K_Means(vector<Feature>sample, int k)
{
	vector<Cluster> clusters;
	clusters.clear();
	
	vector<vector<int>> clusters_samples_map;
	for(int i = 0;i<k;i++)
	{
		vector<int> sampleIDList;
		clusters_samples_map.push_back(sampleIDList);

		Cluster cluster;
		clusters.push_back(cluster);
	}

	vector<int> seeds(k);
				
	//1. Random pick a seed for each cluster k
	for(int i = 0;i < k;i++){
		int currentSeed = rand() % sample.size();
		
		seeds.push_back(CheckSeeds(i, seeds, currentSeed, sample.size()));
		clusters[i].centerPoint = sample[seeds[i]].pos;		
	}
		
	bool change= true;
	int prevCluster= -1;
	int prevClusterInd= -1;
				
	while(change)
	{						
		change= false;
					
		//2. Assign every point to its closest seeds
		//For each sample point	
		double* dist_to_cluster_list= new double[sample.size() * k];

		for(int i=0;i<sample.size();i++){				
			
			double nearestDist= CalcEuclideanDist(sample[i].pos, clusters[0].centerPoint);
			dist_to_cluster_list[i * k + 0]= nearestDist;
			int nearestSeed= seeds[0];
			int nearestCluster= 0;
			bool found= false;
												
			for(int j=1;j<k;j++){
									
				double currentDist= CalcEuclideanDist(sample[i].pos, clusters[j].centerPoint);
				dist_to_cluster_list[i * k + j]= currentDist;
				if(currentDist < nearestDist){
					nearestDist= currentDist;
					nearestSeed= seeds[j];
					nearestCluster= j; 
				}
			}
						
			for(int j=0;j<k;j++){
				if(!found)
				{
					for(int m=0;m<clusters_samples_map[j].size();m++)
					{
						if(i == clusters_samples_map[j][m])
						{
							prevCluster= j;
							prevClusterInd= m;
							found= true;
							break;
						}
					}					
				}
			}
				
			if(prevCluster != nearestCluster){
				change= true;
				if(prevClusterInd != -1)
					clusters_samples_map[prevCluster].erase(clusters_samples_map[prevCluster].begin() + prevClusterInd);
				clusters_samples_map[nearestCluster].push_back(i);
			}				
		}
			
		//3. Calculate the center or mean point of each cluster
		for(int j = 0;j<k;j++)
		{
			Cluster cluster;
			cluster.id = j+1;

			int mean_x = 0, mean_y = 0;
			int sampleCount = clusters_samples_map[j].size();
			for(int m = 0;m<sampleCount;m++)
			{
				Position point = sample[clusters_samples_map[j][m]].pos;
				cluster.points.push_back(point);
				
				mean_x += point.x;
				mean_y += point.y;
			}

			mean_x /= sampleCount;
			mean_y /= sampleCount;

			cluster.centerPoint.x = mean_x;
			cluster.centerPoint.y = mean_y;

			clusters.push_back(cluster);
		}	
	}

	return clusters;
}
	
