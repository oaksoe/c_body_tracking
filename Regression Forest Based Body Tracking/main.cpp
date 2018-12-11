#pragma once
#include "DTree.h"

int main()
{
	DTree dtree(1);
	dtree.BuildTree();

	system("pause");
	return 0;
}


/* 
	Efficient Regression of General-Activity Human Poses from Depth Images

	Phase 1: Learning tree structure
	3 trees, 20 deep, 300k training images per tree, 
	2000 training example pixels per image, 2000 candidate features, and 50 candidate thresholds per feature

	- Randomly Propose a set of splitting candidates Phi = (Theta, T) 
	  (feature parameters Theta and thresholds T). 

	- Partition the set of examples Q = {(I, x)} into left and right subsets by each Phi:
			Ql(Phi) = { (I,x) | func(I,x) < T }
			Qr(Phi) = Q \ Ql(Phi)

	- Phi(*) = argmin(phi) [ SUM(s E {l,r}) |Q(s)(Phi)| * E(Q(s)(Phi)) / |Q| ]

	- func(I,x) = d(I) (x + u / d(I)(x)) - d(I) (x + v / d(I)(x))
	  where 
			d(I)(x) = depth at pixel x in image I
			Theta = (u, v) = offsets u and v
			1/d(I)(x) = normalization of offsets (to ensure that depth invariant)

	- E(Q) = SUM(j) [ SUM((i,q) E Q(j)) [ || TRI(iq->j) - Mu(j) || ] ] 
	  Mu(j) = 1 / |Q(j)| * SUM((i,q) E Q(j)) [ TRI(iq->j) ] 
	  Q(j) = { (i,q) E Q | || TRI(iq->j) || < Pica }


	Phase 2: Learning relative votes
	
	//Collect relative votes
	initialize R(lj) = EMPTY_SET for all leaf nodes l and joints j

	for all pixels q in all training images i do
		lookup ground truth joint positions Z(ij)
		lookup 3D pixel position X(iq)
		compute relative offset Triangle(iq->j) = Z(ij) - X(iq)
		descend tree to reach leaf node l
		store Triangle(iq->j) in R(lj) with reservoir sampling

	//Cluster

	for all leaf nodes l and joints j do
		cluster offsets R(lj) using mean shift
		take top K weighted modes as V(lj)
	return relative votes V(lj) for all nodes and joints


	Phase 3: Inferring Joint Position Hypotheses

	// Collect absolute votes
	initialize Votes(j) = EMPTY_SET for all joints j
	
	for all pixels q in the test image do
		lookup 3D pixel position X(q) = (x(q), y(q), z(q))

		for all trees in forest do
			descend tree to reach leaf node l

			for all joints j do
				lookup weighted relative vote set V(lj)

				for all (Triangle(ljk), w(ljk)) E V(lj) do

					if || Triangle(ljk) || <= distance threshold Lambda(j) then
						compute absolute vote Z = Triangle(ljk) + X(q)
						adapt confidence weight W = w(ljk) . z(q) ^ 2
						Votes(j) := Votes(j) U {(Z, W)}

	//Aggregate weighted votes
	sub-sample Votes(j) to contain N votes
	aggregate Votes(j) using mean shift on Eq. 1
	return weighted modes as final hypotheses


	Implementation Issues
	---------------------

	Input File Data Format

	############################
	Frame 1

	Bounding Box
	minX	minY	maxX	maxY

	Joints
	x	y	z	w
	.
	.

	Body
	x	y	z	w
	.
	.
	############################

	Note: 
	Preprocess the bounding box in dataset builder
	- create the bounding box
		- get minX, maxX, minY, maxY of the player pixels

	- validate the bounding box
		- check the area and ratio of the width and height


	Preprocess TrainingImage

	fill pixels2D after converting 3D data to 2D, the size of pixels2D should be the size of bounding box
*/