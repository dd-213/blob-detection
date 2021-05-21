//blob.cpp
#include "Blob.h"

Blob::Blob(cv::Rect _boundingRect) {
	boundingRect = _boundingRect;
	cv::Point currentCenter;

	currentCenter.x = boundingRect.x + boundingRect.width / 2;
	currentCenter.y = boundingRect.y + boundingRect.height / 2;

	stillTracked = true;
	newOrMatched = true;
	numOfConsecutiveFramesWithoutMatch = 0;
}