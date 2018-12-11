#pragma once
#include<algorithm>
#include "ISM.h"
#include "MeanShift.h"
#include "RNN.h"
#include "Util.h"

using namespace std;

ISM::ISM()   
{
}

void ISM::Train(vector<TrainingImage> TrainingSet)
{
    /*
        1.  Create an appearance codebook C.
        F <- EmptySet     // Initialize the set of feature vectors F

        for all training images do
            Apply the interest point detector.
            for all interest regions k = ( x, y, s) with descriptors fk do
                F <- F U fk
            end for
        end for

        Cluster F with cut-off threshold t and keep cluster centers C.

        2.    Compute occurrences Occ.

        for all codebook entries Ci do
            Occ[i] <- EmptySet // Initialize occurrences for codebook entry Ci
        end for
   

        for all training images do
            Let (cx, cy) be the object center at a reference scale.
            Apply the interest point detector.
            for all interest regions k = ( x, y, s) with descriptors fk do
                for all codebook entries Ci do
                    if sim(Ci, fk) >= t then
                        // Record an occurrence of codebook entry Ci
                        Occ[i] <- Occ[i] U (cx - x, cy - y, s)
                    end if
                end for
            end for
        end for
    */

    vector<Descriptor> descriptorSet;
    for(int i = 0;i<TrainingSet.size();i++)
    {
        vector<Descriptor> descriptors_per_frame = Util::ExtractFeatureDescriptors(TrainingSet[i].img);
        copy(descriptors_per_frame.begin(), descriptors_per_frame.end(), descriptorSet);
    }

    RNN ClusterProcess;
    Codebook = ClusterProcess.Process_RNN(descriptorSet);

    vector<Feature> featureSet;
    for(int i = 0;i<TrainingSet.size();i++)
    {
        int cx = TrainingSet[i].cx;
        int cy = TrainingSet[i].cy;

        vector<Feature> features_per_frame = Util::ExtractFeatures(TrainingSet[i].img);
        for(int j = 0;j<features_per_frame.size();j++)
        {
            for(int k = 0;k<Codebook.size();k++)
            {
                KeyPoint p = features_per_frame[j].kp;
               
                vector<double> d1 = features_per_frame[j].descriptor;
                vector<double> d2 = Codebook[k].descriptor;

                double euclideanDist = Util::EuclideanDistance(d1, d2);
                double sim = -1 * pow(euclideanDist, 2);

                if(sim >= RNN_THRESHOLD)
                {
                    KeyPoint occ(cx - p.x, cy - p.y, p.s);
                    Codebook[k].occurrence.push_back(occ);
                }
            }
        }
    }   
}

void ISM::GenerateVotes(IplImage* TestImage)
{
    /*
        // Initialize the set of probabilistic votes V.
        V <- EmptySet
       
        //Apply the interest point detector to the test image.
        for all interest regions l k = ( lx, ly, l s) with descriptors fk do
           
            // Initialize the set of matches M
            M <- EmptySet
           
            // Record all matches to the codebook
            for all codebook entries Ci do
                if sim(fk, Ci) >= t then
                    M <- M U (i, lx, ly, ls)        // Record a match
                end if
            end for
               
            for all matching codebook entries Ci* do
                p(Ci* | fk) <- 1 / |M|            //Set the match weight or confidence
            end for
           
            // Cast the votes
            for all matches (i, l x, ly, ls) belongsTo M do
                for all occurrences occ belongsTo Occ[i] of codebook entry Ci do
                   
                    //Set the vote location
                    x <- (l x ? occx * (ls / occs), l y ? occy * (ls / occs), ls / occs)
                   
                    //Set the occurrence weight
                    p(on, x|Ci, l ) <- 1 / | Occ[i] |
                   
                    // Cast a vote (x,w, occ, l) for position x with weight w
                    w <- p(on, x|Ci, l )p(Ci|fk)
                    V <- V ?(x,w,occ,l )
                end for
            end for
        end for
    */

    vector<Feature> features = Util::ExtractFeatures(TestImage);

    for(int i = 0;i<features.size();i++)
    {
        Feature feature = features[i];

        vector<Match> M;

        for(int j = 0;j<Codebook.size();j++)
        {
            double euclideanDist = Util::EuclideanDistance(feature.descriptor, Codebook[j].descriptor);
            double sim = -1 * pow(euclideanDist, 2);

            if(sim >= RNN_THRESHOLD)
            {
                Match m(j, feature.kp, 0.0);
                M.push_back(m);
            }
        }

        for(int j = 0;j<M.size();j++)
            M[j].weight = 1 / M.size();

        for(int j = 0;j<M.size();j++)
        {
            Match m = M[j];

            CodeVector codebookEntry = Codebook[m.i];

            for(int k=0;k<codebookEntry.occurrence.size();k++)
            {
                KeyPoint occ = codebookEntry.occurrence[k];

                double s = m.l.s / occ.s;

                KeyPoint x(m.l.x - occ.x * s, m.l.y - occ.y * s, s);

                double occWeight = 1 / codebookEntry.occurrence.size();

                double w = occWeight * m.weight;

                Vote v(x, occ, m.l, w);

                V.push_back(v);
            }
        }
    }       
}

void ISM::ScaleAdpativeHypothesisSearch()
{
    /*
        // Sample the voting space V in a regular grid to obtain promising
        // starting locations.
        for all grid locations x do
            score(x) <- applyMSMEKernel(K, x)
        end for

        // Refine the local maxima using MSME with a scale-adaptive
        // kernel K. Keep all maxima above a threshold ?.
        for all grid locations x do
            if x is a local maximum in a 3 × 3 neighborhood then
                // Apply the MSME search
                repeat
                    score <- 0, xnew <- (0, 0, 0), sum <- 0
                    for all votes (xk,wk, occk, k) do
                        if xk is inside K(x) then
                            score <- score + wk * K( (x-xk) / b(x) )
                            xnew <- xnew + xk * K( (x-xk) / b(x) )
                            sum <- sum + K( (x-xk) / b(x) )
                        end if
                    end for
               
                    score <- 1 /a Vb(x) * score
                        x <- 1 / sum * xnew

                until convergence

                if score >= theta then
                    Create hypothesis h for position x.
                end if
            end if
        end for
    */

    /*  Accumulator Bins
        ----------------
        Logically 3D array => vote_bin [X][Y][S]
        Physically 1D array => vote_bin [A]

        Eg. [4][3][2] <=> [24]
                                |                          |                                |
                                |                          |                                |
         s0     s1    |        |        |        |       | s0   s1 |            |          |    s0     s1    |          |    s0     s1    |
        | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |*11*| 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
                |        |        |        |        |          |            |          |            |          |            |   
            y0        y1        y2    |    y0        y1        y2      |    y0        y1           y2    |     y0           y1         y2     
                                |                          |                                |
                    x0                        x1                            x2                              x3


        1D to 3D
        --------
        a = 11, x = ? , y = ?, s = ?

        x = a / (Y*S) = 11 / (3*2) = 1

        y = (a % (Y*S)) / S = (11 % 6) / 2 = 5 / 2 = 2

        s = 5 % 2 = 1

        So, x = 1, y = 2, s = 1

       
        3D to 1D
        --------
        x = 2, y = 2, s = 0, a = ?

        a = x * (Y*S) + y * S + s = 2 * 6 + 2 * 2 + 0 = 16

    */
   
	int vote_bin[BIN_COUNT_X * BIN_COUNT_Y * BIN_COUNT_S]= {0};
	vector<KeyPoint> hyp_points;  //Hypothesis points

    for(int i = 0; i < BIN_COUNT; i++)
    {
        int x, y, s;
        Get_3D_Bin_Indices(i, x, y, s);

		KeyPoint bin_origin(x * BIN_SIZE_X, y * BIN_SIZE_Y, s * BIN_SIZE_S);
        KeyPoint bin_center(bin_origin.x + BIN_SIZE_X / 2, bin_origin.y + BIN_SIZE_Y / 2, bin_origin.s + BIN_SIZE_S / 2);

        for(int j = 0; j < V.size(); j++)    //for all votes
        {
            Vote v = V[j];
                      
			//if xk is inside K(x)
            if(Gaussian_Kernel(Util::EuclideanDistance(bin_center, v.x)) < KERNEL_BOUND)
				vote_bin[i]++;
        }
    }  

	for(int i = 0; i < BIN_COUNT; i++)
    {
		int x, y, s;
        Get_3D_Bin_Indices(i, x, y, s);

		KeyPoint bin_origin(x * BIN_SIZE_X, y * BIN_SIZE_Y, s * BIN_SIZE_S);
        KeyPoint bin_center(bin_origin.x + BIN_SIZE_X / 2, bin_origin.y + BIN_SIZE_Y / 2, bin_origin.s + BIN_SIZE_S / 2);
		KeyPoint bin_x = bin_center;

		//if the bin is a local maximum in a 3 × 3 neighborhood then [temporarily if the bin score > threshold]
		if(vote_bin[i] > VOTE_BIN_THRESHOLD)
		{
			int score = 0;		
			int ms_dist = 0; //mean shift distance

			do
			{
				score = 0;
				KeyPoint new_x(0,0,0);
				int sum = 0;

				for(int j = 0; j < V.size(); j++)    //for all votes
				{
					Vote v = V[j];
                      
					//if xk is inside K(x)
					int k = (int) Gaussian_Kernel(Util::EuclideanDistance(bin_x, v.x));
					if(k < KERNEL_BOUND)
					{
						score += v.w * k;
						sum += k;

						new_x.x += v.x.x * k;
						new_x.y += v.x.y * k;
						new_x.s += v.x.s * k;
					}
				}

				score /= KERNEL_BANDWIDTH;

				KeyPoint prev_bin_x(bin_x);

				bin_x.x = new_x.x / sum;
				bin_x.y = new_x.y / sum;
				bin_x.s = new_x.s / sum;

				ms_dist = Util::EuclideanDistance(prev_bin_x, bin_x);
			}while(ms_dist > MEANSHIFT_CONVERGING_THRESHOLD);     //until converged

			if(score >= HYPOTHESIS_THRESHOLD)
				hyp_points.push_back(bin_x);
		}
	}
}

double ISM::Gaussian_Kernel(double x)
{
	//Kernel_1_gamma(x) = exp(- (||x|| ^ 2) / gamma)
	return exp(-1 * pow(x, 2) / KERNEL_BANDWIDTH);
}

void ISM::Get_3D_Bin_Indices(int a, int &x, int &y, int &s)
{
    x = a / (BIN_COUNT_Y * BIN_COUNT_S);

    y = (a % (BIN_COUNT_Y * BIN_COUNT_S)) / BIN_COUNT_S;

    s = (a % (BIN_COUNT_Y * BIN_COUNT_S)) % BIN_COUNT_S;
}

int ISM::Get_1D_Bin_Index(int x, int y, int s)
{
    return x * (BIN_COUNT_Y * BIN_COUNT_S) + y * BIN_COUNT_S + s;
}

/*    MSME(x, gamma)
        Begin Algorithm
        1.  Initialization: given gamma, random choose f_1 C x, t = 1;
        2.  do {
                t++;

                w1=w2=0;
                for(j=0;j<m;j++)
                {
                    for(i=0;i<n;i++)
                    {
                        e_ij_t-1 = x_i - f_j_t-1 ;
                        w1 += Kernal_gamma(e_ij_t-1) * x_i;
                        w2 += Kernal_gamma(e_ij_t-1);
                    }

                    f_j_t = w1 / w2;
                }

                diff=0;
                for(j=0;j<m;j++)
                    diff += || f_j_t - f_j_t-1 ||;

            } while (diff <= Epsilon)   // converged
       
        3.    f_gamma  <-  { f_j_t } ;
        4.  Calculate  m_gamma by considering seed merging
        5.  output f_gamma and m_gamma

        End Algorithm

        Kernel_1_gamma(x) = exp(- (||x|| ^ 2) / gamma)

        Kernel_2_gamma(x) = 1 / [(1 + (||x|| ^ 2) / gamma)] ^ 2

        Kernel_3_gamma(x) = 1 / [(1 + (||x|| ^ 2) / gamma)]

        Kernel_4_gamma(x) = - gamma * exp(- (||x|| ^ 2) / gamma)

        Kernel_5_gamma(x) = - gamma / [(1 + (||x|| ^ 2) / gamma)] ^ 2

        Kernel_6_gamma(x) = gamma / ln [(1 + (||x|| ^ 2) / gamma)]

        Some adaptive interaction functions and potential functions when ||x|| ^ 2 <= gamma.
        They are 0 when ||x|| ^ 2 > gamma.
    */