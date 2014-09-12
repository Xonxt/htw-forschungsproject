// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "SkinDetector.h"


SkinDetector::SkinDetector() {
	// YCrCb thresholds
	YCbCr.Y_MIN  = 0;
	YCbCr.Y_MAX  = 255;
	YCbCr.Cr_MIN = 133;
	YCbCr.Cr_MAX = 173;
	YCbCr.Cb_MIN = 77;
	YCbCr.Cb_MAX = 127;

	// HSV thresholds
	HSV.H_MIN = 0;
	HSV.H_MAX = 15;
	HSV.S_MIN = 76;
	HSV.S_MAX = 118;
	HSV.V_MIN = 0;
	HSV.V_MAX = 255;

	// initialize the adaptive skin detector
	// filter = CvAdaptiveSkinDetector(1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE);
}

// extract the skin mask
void SkinDetector::extrackskinMask(const cv::Mat inputFrame, cv::Mat& outputMask, SkinSegmMethod method) {
	// get the skin detection method and extract mask
	switch (method) {
	case SKIN_SEGMENT_ADAPTIVE:
		getSkinAdaptiveDetector(inputFrame, outputMask);
		break;
	case SKIN_SEGMENT_HSV:
		getSkinHsv(inputFrame, outputMask);
		break;
	case SKIN_SEGMENT_YCRCB:
		getSkinYcbcr(inputFrame, outputMask);
		break;
	default:
		throw std::string("Unknown skin segmentation method!");
		break;
	}
}

void SkinDetector::getSkinYcbcr(const cv::Mat inputFrame, cv::Mat& outputMask) {
	//first convert our RGB image to YCrCb
	cv::cvtColor(inputFrame, outputMask, cv::COLOR_BGR2YCrCb);

	//filter the image in YCrCb color space
	cv::inRange(outputMask, cv::Scalar(MIN(YCbCr.Y_MIN, YCbCr.Y_MAX), MIN(YCbCr.Cr_MIN, YCbCr.Cr_MAX), MIN(YCbCr.Cb_MIN, YCbCr.Cb_MAX)),
							cv::Scalar(MAX(YCbCr.Y_MIN, YCbCr.Y_MAX), MAX(YCbCr.Cr_MIN, YCbCr.Cr_MAX), MAX(YCbCr.Cb_MIN, YCbCr.Cb_MAX)),
				outputMask);
}

// perform HSV skin segmentation
void SkinDetector::getSkinHsv(const cv::Mat inputFrame, cv::Mat& outputMask) {
	// convert input image to HSC
	cv::cvtColor(inputFrame, outputMask, cv::COLOR_BGR2HSV);

	cv::inRange(outputMask, cv::Scalar(MIN(HSV.H_MIN, HSV.H_MAX), MIN(HSV.S_MIN, HSV.S_MAX), MIN(HSV.V_MIN, HSV.V_MAX)),
							cv::Scalar(MAX(HSV.H_MIN, HSV.H_MAX), MAX(HSV.S_MIN, HSV.S_MAX), MAX(HSV.V_MIN, HSV.V_MAX)), 
				outputMask);
}

// use tha OpenCV sdaptive skin detector
void SkinDetector::getSkinAdaptiveDetector(const cv::Mat inputFrame, cv::Mat& outputMask) {
	// Convert input Mat-type to old-style IplImage-type
	IplImage* _frame = new IplImage(inputFrame);

	// Create IplImage-type mask
	IplImage* _mask = cvCreateImage(cvSize(inputFrame.cols, inputFrame.rows), IPL_DEPTH_8U, 1);

	// perform filtering
	filter.process(_frame, _mask);

	// convert mask to Mat-type and make it black&white
	outputMask = cv::Mat(_mask);
	inRange(outputMask, cv::Scalar(1), cv::Scalar(180), outputMask);
}

// change the HSV bounds
void SkinDetector::setNewHsvBounds(const struct HsvBounds hsv) {
	HSV.H_MIN = hsv.H_MIN;
	HSV.H_MAX = hsv.H_MAX;
	HSV.S_MIN = hsv.S_MIN;
	HSV.S_MAX = hsv.S_MAX;
	HSV.V_MIN = hsv.V_MIN;
	HSV.V_MAX = hsv.V_MAX;
}

// change Ycbcr bounds
void SkinDetector::setNewYcbcrBounds(const struct YCbCrBounds ycbcr) {
	YCbCr.Y_MIN  = ycbcr.Y_MIN;
	YCbCr.Y_MAX  = ycbcr.Y_MAX;
	YCbCr.Cr_MIN = ycbcr.Cr_MIN;
	YCbCr.Cr_MAX = ycbcr.Cr_MAX;
	YCbCr.Cb_MIN = ycbcr.Cb_MIN;
	YCbCr.Cb_MAX = ycbcr.Cb_MAX;
}
