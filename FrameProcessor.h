#pragma once

#include "stdafx.h"
#include "Hand.h"
#include "HandDetector.h"
#include "Tracker.h"

#include <time.h>
#include <ctime>

#include <fstream>

#define FACE_DETECTOR_XML "lbpcascade_frontalface.xml"

class FrameProcessor
{
public:
	FrameProcessor();
    ~FrameProcessor();

	// initizlize the processor
	bool initialize();

	// the list of detected hands
	std::vector<Hand> hands;

	// process frame, find hands, detect gestures
	void processFrame(cv::Mat& frame);
	
	// draw everything on frame (hands, fingers, etc.)
	void drawFrame(cv::Mat& frame);

	// change skin segmentation method
	void changeSkinMethod();

	// toggle show boolean skin-mask
	void toggleShowMask();

private:
	// Tool for detecting every hand (open palm!) in the image
	HandDetector handDetector;

	// face cascade detector
	cv::CascadeClassifier faceCascade;

	// Tool fot tracking of hands
	Tracker handTracker;

	// show backprojection mask or not?
	bool showMask;
    
    // time measure
    clock_t startClock,finishClock;
    double timeCount;
    std::ofstream fout;
};

