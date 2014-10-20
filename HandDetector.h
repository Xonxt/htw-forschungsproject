// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#pragma once

#define HAND_DETECTOR_XML "hogcascade_righthand_v3.0.xml"
#define HAND_DETECTOR_BAD_XML "RightHandWnd_0.01.xml"

#define RESIZE_HEIGHT 240
#define RESIZE_WIDTH  320

// amount of frames skipped for pedestrian detection
#define PEDESTRIANS_SKIP_FRAMES 25

#include "stdafx.h"
#include "Hand.h"

class HandDetector
{
public:
	HandDetector();

	// initialize detectot
	bool initialize(bool webCam = false);

	// detect all hands in frame
	void detectHands(const cv::Mat frame, std::vector<Hand>& hands, std::vector<cv::Rect>& rois);

private:

	// convert cv::Rect to cv::RotatedRect
	void rect2rotatedRect(const cv::Rect inputRect, cv::RotatedRect& outputRect);

	// convert cv::Rect to Hand
	void rect2Hand(const cv::Rect inputRect, Hand& outputHand, const cv::Point shift = cv::Point(0, 0));

	/* -------------------------------------------------------------------------- */

	// my cascade hand detector
	cv::CascadeClassifier handCascade;

	// HOG pedestrian detector
	cv::HOGDescriptor pedestrianDetector;

	// current frame number
	int frameNumber;
	
	// a list of pedestrians in frame
	std::vector<cv::Rect> pedestrians;

	// no pedestriands in frame
	bool noPedestrians;

	// is a web cam?
	bool isWebCam;
};

