#pragma once
#include<cv.h>
#include <highgui.h>

#include "Util.h"

vector<Feature> Util::ExtractFeatures(IplImage* input_image) 
{
	IplImage* grey_image = cvCreateImage(cvGetSize(input_image), 8, 1);
	cvCvtColor(input_image, grey_image, CV_RGB2GRAY );

	uchar* color_data = (uchar*) input_image->imageData;
	uchar* grey_data = (uchar*) grey_image->imageData;
	int color_steps = input_image->widthStep;
	int grey_steps = grey_image->widthStep;
	
	vector<Feature> features;
	
	CvMemStorage* storage = cvCreateMemStorage(0);

    CvSeq *keypoints = 0, *descriptors = 0;
    
    CvSURFParams params = cvSURFParams(500, 1);

    cvExtractSURF(grey_image, 0, &keypoints, &descriptors, storage, params );
    
	CvSeqReader reader, kreader;
    cvStartReadSeq( keypoints, &kreader );
    cvStartReadSeq( descriptors, &reader );

	int length = (int)(descriptors->elem_size/sizeof(float));
	assert( length % 4 == 0 );

	for(int i = 0; i < keypoints->total; i++ )
    {
        CvSURFPoint* r = (CvSURFPoint*)kreader.ptr;
		float* descriptor = (float*)reader.ptr;
		
		Feature feature;
		feature.kp.x = cvRound(r->pt.x);
		feature.kp.y = cvRound(r->pt.y);
        feature.kp.s = cvRound(r->size); //cvRound(r->size*1.2/9.*2);
		
		for(int j = 0;j < length; j++)
			feature.descriptor.push_back((double) descriptor[j]);

		features.push_back(feature);

		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
    }
	
	return features;
}

vector<Descriptor> Util::ExtractFeatureDescriptors(IplImage* input_image) 
{
	IplImage* grey_image = cvCreateImage(cvGetSize(input_image), 8, 1);
	cvCvtColor(input_image, grey_image, CV_RGB2GRAY );

	uchar* color_data = (uchar*) input_image->imageData;
	uchar* grey_data = (uchar*) grey_image->imageData;
	int color_steps = input_image->widthStep;
	int grey_steps = grey_image->widthStep;
	
	vector<Descriptor> featureDescriptors;
	
	CvMemStorage* storage = cvCreateMemStorage(0);

    CvSeq *keypoints = 0, *descriptors = 0;
    
    CvSURFParams params = cvSURFParams(500, 1);

    cvExtractSURF(grey_image, 0, &keypoints, &descriptors, storage, params );
    
	CvSeqReader reader, kreader;
    cvStartReadSeq( keypoints, &kreader );
    cvStartReadSeq( descriptors, &reader );

	int length = (int)(descriptors->elem_size/sizeof(float));
	assert( length % 4 == 0 );

	for(int i = 0; i < keypoints->total; i++ )
    {
        CvSURFPoint* r = (CvSURFPoint*)kreader.ptr;
		float* descriptor = (float*)reader.ptr;
		
		Descriptor featureDescriptor;
		
		for(int j = 0;j < length; j++)
			featureDescriptor.push_back((double) descriptor[j]);

		featureDescriptors.push_back(featureDescriptor);

		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
        CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
    }
	
	return featureDescriptors;
}

double Util::EuclideanDistance(Descriptor d1, Descriptor d2)
{
	double dist = 0.0;
	for(int i = 0;i<d1.size();i++)
		dist += pow(d1[i] - d2[i], 2);

	return sqrt(dist);
}

double Util::EuclideanDistance(KeyPoint p1, KeyPoint p2)
{
    return sqrt(pow((p1.x - p2.x) * 1.0, 2) + pow((p1.y - p2.y) * 1.0, 2) + pow((p1.s - p2.s) * 1.0, 2));
}
