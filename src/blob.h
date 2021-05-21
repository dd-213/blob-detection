//blob.h
#ifndef BLOB 
#define BLOB
#include <opencv2/opencv.hpp>

class Blob {
public:
	cv::Rect boundingRect;
	bool newOrMatched;
	bool stillTracked;
	int numOfConsecutiveFramesWithoutMatch;

	Blob(cv::Rect _boundingRect);
};


#endif