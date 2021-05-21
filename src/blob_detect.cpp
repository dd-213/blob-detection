/*

blob_detect.cpp : by dd213 05.2020 Wrok In Progress

Implemented blob detection for vehicles on two-way roads

TODO at the end

*/

#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <stdlib.h>		//for clearing warnings in console - possibly camera access?
#include "Blob.h"

void ExtractBlobsFromFrame(cv::Mat &frame1, cv::Mat &frame2, std::vector<Blob> &currentBlobs); 
void MatchBlobs(std::vector<Blob> &act_blobs, std::vector<Blob> &all_blobs, const unsigned int border);
bool CheckBorderPass();	
void DrawVisuals(std::vector<Blob> &blobs, cv::Mat &frame);

int main() {
	cv::VideoCapture video;
	cv::Mat frame1;
	cv::Mat frame2;
	std::vector<Blob> blobs;
	cv::Point borders[2];	

	video.open("TEST VIDEO.mp4");

	if (!video.isOpened()) {
		std::cout << "error reading video file\n";
		//_getch();
		return(0); 
	}

	if (video.get(cv::CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: not enough frames < 2";
		//_getch();
		return(0);
	}

	video.read(frame1);
	video.read(frame2);

	/*
	if (!video.read(frame1) || !(video.read(frame2)){

	}
	*/

	bool blob_init = true;
	char keyPressed = 0;	//checked value for esc

	while (video.isOpened() && keyPressed != 27) {
		std::vector<Blob> currentBlobs;
		cv::Mat frame1copy = frame1.clone();
		cv::Mat frame2copy = frame2.clone();
		cv::Mat frame_colour = frame2.clone();
		cv::Point frame_size = frame1copy.size();
		const unsigned int border_mid = frame_size.x / 2;

		ExtractBlobsFromFrame(frame1copy,frame2copy, currentBlobs);
		MatchBlobs(blobs, currentBlobs, border_mid);

		DrawVisuals(blobs, frame_colour);
		cv::imshow("Blobs", frame_colour);
		cv::waitKey(0);

		video.read(frame1);
		video.read(frame2);
		
	}

}
/*
void ExtractBlobsFromFrame:
	- implemented blob detection
	- exracting contours of blobs
	- contours approximation to rectangles and filtering
*/
void ExtractBlobsFromFrame(cv::Mat &frame1, cv::Mat &frame2, std::vector<Blob> &currentBlobs) {
	using namespace cv;

	Mat frameDiff;
	Mat frameTresh;
	Mat frameDil;
	Mat frameEr;
	std::vector<std::vector<Point> > contours;
	//system("CLS");

	cvtColor(frame1, frame1, COLOR_BGR2GRAY);
	cvtColor(frame2, frame2, COLOR_BGR2GRAY);
	//noise removal
	GaussianBlur(frame1, frame1, Size(5, 5), 0);
	GaussianBlur(frame2, frame2, Size(5, 5), 0);
	//motion detection by frame difference
	absdiff(frame1, frame2, frameDiff);
	threshold(frameDiff, frameTresh, 30, 255, THRESH_BINARY);

	// size of kernel for morphological transformations
	Mat structuringElement = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(frameTresh, frameDil, structuringElement);
	dilate(frameDil, frameDil, structuringElement);
	erode(frameDil, frameEr, structuringElement);

	imshow("All maybe-contours", frameEr);
	waitKey(0);

	findContours(frameEr, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::vector<Point> contours_poly(contours.size());
	Rect boundRect;

	for (auto& contour : contours)
	{
		approxPolyDP(contour, contours_poly, 3, true);
		boundRect = boundingRect(contours_poly);
		float dimensions_ratio = (float)boundRect.width / (float)boundRect.height;
		if (abs(boundRect.width - boundRect.height) < 25
			and (boundRect.width > 17 or boundRect.height > 17)
			and (1.3 > dimensions_ratio) 
			and (dimensions_ratio > 0.8)) {
			currentBlobs.push_back(Blob(boundRect));
		}
	}
}

/*
void MatchBlobs:
	- if no previous blobs, all contours added
	- matching filtered contours with existing blobs based on possible distance between frames
	- if not matched, contour added as blob (vehicle)
	- removing unmatched original blobs
*/
void MatchBlobs(std::vector<Blob> &blobs, std::vector<Blob> &currentBlobs, const unsigned int border) {

	size_t blobs_initial_size = blobs.size();	

	if (blobs.size()) {

		for (auto &blob : blobs) {
		blob.numOfConsecutiveFramesWithoutMatch++;
		}

		for (size_t j = 0; j < currentBlobs.size(); j++) {
			size_t i = 0;

			for (i = 0; i < blobs_initial_size; i++) {
				/*
				if(checkBorderPass(current_blobs[i])){
					//+upper/lower border
				}
				*/
				unsigned int blob_x = blobs[i].boundingRect.x;
				unsigned int blob_y = blobs[i].boundingRect.y;
				unsigned int curr_x = currentBlobs[j].boundingRect.x;
				unsigned int curr_y = currentBlobs[j].boundingRect.y;

				if (std::abs(int(blob_y - curr_y)) < 20 and blob_x - curr_x < 20) {
					if (blob_y >= curr_y and blob_x > border) {
						blobs[i] = currentBlobs[j];
						break;
					}
					else if (blob_y <= curr_y and blob_x < border) {
						blobs[i] = currentBlobs[j];
						break;
					}
				}
			}
			//add blob
			if (i == blobs_initial_size)
			{
				blobs.push_back(currentBlobs[j]);
			}
		}
	}
	else {
		blobs = currentBlobs;
	}

	for (size_t i = 0; i < blobs.size(); i++) {
		if (blobs[i].numOfConsecutiveFramesWithoutMatch > 0) {
			blobs.erase(blobs.begin() + i);
		}
	}
}

/*
void DrawVisuals:
	- showing outcome such as:
		used borders
		identified blobs as probable vehicles
		possible added count?
*/
void DrawVisuals(std::vector<Blob>& blobs, cv::Mat& frame) {
	for (auto& blob : blobs) {
		cv::rectangle(frame, blob.boundingRect.tl(), blob.boundingRect.br(), (0, 255, 0), 2);
	}
}


/*
TODO:
- Point borders - dodać up/down, brzegi?
- opcja esc
- dodać ograniczenie stosunku wymiarów 
- dodać ograniczenie rozmiaru zależe od położenia (dwa lub więcej łapane na raz) - może wyelimninować ciężarówki w przypadkach kiedy się normalnie złapią (jeśli przeżyją eliminację przez stosunek wymiarów)
- dodać górną i dolną granicę kwalifikacji
- dodać opcje zapisu

Additional notes:
- erode, dil, dil, erode nie działa, erode na początku to jakis olbrzym (moze z mniejszym struct elem, ale zezarło wszytsko; i tak chyba jestesmy na 5)
- dil, dil, erode - problem z kraweżnikami

*/