// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

#pragma once

#include "stdafx.h"
#include "opencv2/contrib/contrib.hpp"

#include "Hand.h"

class SkinDetector {
public:
	SkinDetector();

	// extract the skin mask
	void extrackskinMask(const cv::Mat& inputFrame, cv::Mat& outputMask, const Hand& hand, SkinSegmMethod method);

	// change the HSV bounds
	void setNewHsvBounds(const struct HsvBounds hsv);

	// change Ycbcr bounds
	void setNewYcbcrBounds(const struct YCbCrBounds ycbcr);

private:
	// perform Ycbcr skin segmentation
	void getSkinYcbcr(const cv::Mat& inputFrame, cv::Mat& outputMask, const Hand& hand);

	// perform HSV skin segmentation
	void getSkinHsv(const cv::Mat& inputFrame, cv::Mat& outputMask, const Hand& hand);

	// use tha OpenCV sdaptive skin detector
	void getSkinAdaptiveDetector(const cv::Mat& inputFrame, cv::Mat& outputMask);

	// the thresholding bounds
	struct YCbCrBounds YCbCr;
	struct HsvBounds   HSV;

	// the openCV adaptive skin detector
	CvAdaptiveSkinDetector filter;
};

