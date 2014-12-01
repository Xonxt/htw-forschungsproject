// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#pragma once

#include "stdafx.h"

#include "Finger.h"
#include "Gesture.h"

class Hand
{
public:
	Hand();
    
   ~Hand();

	// init the tracker
	void initTracker();

	// assign new location to the hand
	void assignNewLocation(const Hand newHand);

	// add a new measurement for the Kalman tracker
	void addKalmanMeasurement(const cv::Point coords);

	// get the latest predicted position by the Kalman
	cv::Point getLatestKalmanPrediction();

	// recalculate the hand's thresholding ranges:
	void recalculateRange(const cv::Mat frame, SkinSegmMethod method);

	// the hand's bounding box
	cv::RotatedRect handBox;

	// TEMP: the hand's trackbox
	cv::Rect detectionBox;

	// the hands ROI
	cv::Rect roiRectange;

	// Hand gesture
	Gesture handGesture;

	float sizeDiff;

	// The hand's color ranges
	struct YCbCrBounds YCbCr;
	struct HsvBounds   HSV;

	// the information for the gesture
	struct HandInformation {
		// the speed of hand movement (normalized to ROI width)
		double moveSpeed;
		// the angle of hand movenent in degrees (0..359)
		double moveAngle;
		// the hand contour
		std::vector<cv::Point> handContour;
		// list of fingertips
		std::vector<Finger> fingers;
	} Parameters;

	// all the tracking information
	struct TrackingInformation 
	{
		// the tracking window for the CamShift algorithm
		cv::Rect trackWindow;

		// is the hand being tracked at the moment?
		bool isTracked;

		// how many frames till histogram recalculation
		int trackedFrames;

		// hand histogram
		cv::Mat hist;

		// is it tracked by KalmanFilter?
		bool isKalman;

		// kalman track
		std::vector<cv::Point> kalmTrack;
        
    // camshift track
    std::vector<cv::Point> camsTrack;

		// Kalman Filter Data
		struct KalmanFilterTracker {
			cv::Mat_<float> measurement;
			cv::KalmanFilter KF;
			cv::Mat state;
			cv::Mat processNoise;
		} KalmanTracker;

	} Tracker;

private:
};

