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

	// locate new hand position
	bool getNewPosition(Hand& hand);

	// extract hand contour from the mask
	void extractContour(Hand& hand);

	// filter out the blobs smaller than a threshold
	void removeSmallBlobs(cv::Mat& inputImage, const double blobSize);

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

