#pragma once
#include<cstdlib>
#include<ctime>

#include "RNN.h"
#include "Util.h"

RNN::RNN()	
{
	srand (time(NULL));
}

vector<CodeVector> RNN::Process_RNN(vector<Descriptor> sample)
{
	vector<CodeVector> clusters;
	
	/*
	// Start the chain L with a random point v belongsTo V.
	// All remaining points are kept in R.

	last <- 0 
	lastsim[0] <- 0
	L[last] <- v belongsTo V
	R <- V\v (1)

	while R != EmptySet do

		// Search for the next NN in R and retrieve its similarity sim.
		(s, sim) <- getNearestNeighbor(L[last], R) 
		
		if sim > lastsim[last] then

			// No RNNs -> Add s to the NN chain
			last <- last + 1
			L[last] <- s
			R <- R\{s}
			lastsim[last] <- sim 

		else

			// Found RNNs -> agglomerate the last two chain links
			if lastsim[last] > t then

				s <- agglomerate(L[last], L[last ? 1])   => Problem 1
				R <- RU{s}
				last <- last ? 2 

			else

				// Discard the current chain.
				last <- ?1

			end if

		end if

		if last < 0 then

			// Initialize a new chain with another random point v belongsTo R.
			last <- last + 1
			L[last ] <- v belongsTo R
			R <- R\{v} 

		end if
	end while
	*/

	vector<RNN_Cluster> clusterPoints;
	int last = 0;
	int v = rand() % sample.size();
	
	vector<double>lastsim;
	vector<int> L;
	vector<int> R;

	lastsim.push_back(last);
	
	for(int i = 0;i<v;i++)
		R.push_back(i);
	
	for(int i = v+1;i<sample.size();i++)
		R.push_back(i);

	L.push_back(v);

	while(R.size() != 0)
	{
		double sim = 0;
		int foundIndex = -1;
		int s = GetNearestNeighbor(sample, clusterPoints, L[last], R, sim, foundIndex);

		if(sim > lastsim[last])
		{
			L.push_back(s);
			last++;
			R.erase(R.begin() + foundIndex);
			lastsim.push_back(sim);
		}
		else
		{
			if(lastsim[last] > RNN_THRESHOLD)
			{
				clusterPoints.push_back(Agglomerate(sample, clusterPoints, L[last], L[last-1]));
				s = sample.size() + clusterPoints.size();
				R.push_back(s);
				last -= 2;
			}
			else
			{
				last = -1;
				L.clear();
			}
		}

		if(last < 0)
		{
			last = 0;
			v = rand() % R.size();
			L.push_back(R[v]);
			R.erase(R.begin() + v);
		}
	}

	for(int i = 0;i<L.size();i++)
	{
		int index = L[i];
	
		CodeVector cluster;
		cluster.id = clusters.size();

		if(index >= sample.size())
		{
			index -= sample.size();
			cluster.descriptor = clusterPoints[index].mean;
		}
		else
		{
			cluster.descriptor = sample[index];
		}

		clusters.push_back(cluster);
	}

	return clusters;
}

int RNN::GetNearestNeighbor(vector<Descriptor> sample, vector<RNN_Cluster> clusterPoints, int l, vector<int> R, double& max_sim, int& foundIndex)
{
	RNN_Cluster c1;
	if(l >= sample.size())
	{
		l -= sample.size();
		c1 = clusterPoints[l];
	}
	else
	{
		c1.mean = sample[l];
		c1.count = 1;
		c1.variance = 0;
	}

	for(int i = 0;i < R.size();i++)
	{
		RNN_Cluster c2;
		int Ri = R[i];

		if(Ri >= sample.size())
		{
			Ri -= sample.size();
			c2 = clusterPoints[Ri];
		}
		else
		{
			c2.mean = sample[Ri];
			c2.count = 1;
			c2.variance = 0;
		}

		double euclideanDist = Util::EuclideanDistance(c1.mean, c2.mean);
		double sim = -1 * (pow(c1.variance, 2) + pow(c2.variance, 2) + pow(euclideanDist, 2));

		if(sim > max_sim)
		{
			max_sim = sim;
			foundIndex = i;
		}
	}
	return R[foundIndex];
}

RNN_Cluster RNN::Agglomerate(vector<Descriptor> sample, vector<RNN_Cluster> clusterPoints, int l1, int l2)
{
	RNN_Cluster c;

	RNN_Cluster c1;
	if(l1 >= sample.size())
	{
		l1 -= sample.size();
		c1 = clusterPoints[l1];
	}
	else
	{
		c1.mean = sample[l1];
		c1.count = 1;
		c1.variance = 0;
	}

	RNN_Cluster c2;
	if(l2 >= sample.size())
	{
		l2 -= sample.size();
		c2 = clusterPoints[l2];
	}
	else
	{
		c2.mean = sample[l2];
		c2.count = 1;
		c2.variance = 0;
	}

	c.count = c1.count + c2.count;
	
	for(int i = 0;i < c1.mean.size(); i++)
		c.mean.push_back((c1.count * c1.mean[i] + c2.count * c2.mean[i]) / c.count);
	
	double euclideanDist = Util::EuclideanDistance(c1.mean, c2.mean);
	c.variance = (c1.count * c1.variance + c2.count * c2.variance + pow(euclideanDist, 2) * c1.count * c2.count / c.count) / c.count;
	
	return c;
}



/*  Problem 1

	After agglomerating, new cluster point is created from 2 sample feature points.
	Since L and R are the sets of indices that reference 'sample' vector, sample vector shall not change.
	It cannot be added from the back since the size will grow very big.
	So, create a data structure called 'clusterPoints' that stores new cluster points (position x and y)
	The index to be stored for the new cluster points in L and R will be sample.size() + clusterPoints index 

*/