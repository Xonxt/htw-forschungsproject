// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#pragma once

#include "stdafx.h"
#include "Hand.h"
#include "SkinDetector.h"

class Tracker {
public:
	Tracker();

	//initialize the tracker
	bool initialize();

	// track the hands in the frame
	void trackHands(const cv::Mat inputFrame, std::vector<Hand>& hands);

	// change the skin segmentation method
	void changeSkinMethod(SkinSegmMethod method);

	// get current segmentation method
	SkinSegmMethod getSkinMethod();

	// retrieve the skin mask for debugging purposes
	void getSkinMask(cv::Mat& outputSkinMask);

private:
	int k;

	// locate new hand position
	bool getNewPosition(Hand& hand);

	// extract hand contour from the mask
	// return values:
	// 0 : everything ok
	// -1: false detection, delete hand
	// -2: exception caught, return empty contour
	int extractContour(Hand& hand);

	// filter out the blobs smaller than a threshold
	void removeSmallBlobs(cv::Mat& inputImage, const double blobSize);

	/*
	perform a morphological operation of type 'operation', with a structural element 'mShape' of size 'mSize'
	where operations are: 
	MORPH_OPEN - an opening operation
	MORPH_CLOSE - a closing operation
	MORPH_GRADIENT - a morphological gradient
	MORPH_TOPHAT - “top hat”
	MORPH_BLACKHAT - “black hat”
	*/
	void bwMorph(cv::Mat& inputImage, const int operation, const int mShape, const int mSize);
    
  // crop roi
  void cropRoi(const cv::Mat image, cv::Mat& roi, cv::Rect& rect);

	// the skin detector/segmentation tool
	SkinDetector skinDetector;

	// the current skin segmentation method
	SkinSegmMethod skinSegmMethod;

	// current frame image
	cv::Mat image;

	// the image converted to hsv
	cv::Mat hsv;

	// the current boolean mask
	cv::Mat mask;

	// the calculated back-projection
	cv::Mat backprojection;
    
  // something is being tracked
  bool somethingIsTracked;
};

