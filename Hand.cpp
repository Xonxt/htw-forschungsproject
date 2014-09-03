#include "stdafx.h"
#include "Hand.h"

Hand::Hand() {
	// new hand created!
}

void Hand::assignNewLocation(const cv::RotatedRect& newBbox) {
	handBox = newBbox;

	// predict the location with KalmanFilter
	cv::Mat prediction = Tracker.KalmanTracker.KF.predict();

	Tracker.KalmanTracker.measurement(0) = newBbox.center.x;
	Tracker.KalmanTracker.measurement(1) = newBbox.center.y;

	// estimate the new position of the hand
	cv::Mat estimated = Tracker.KalmanTracker.KF.correct(Tracker.KalmanTracker.measurement);
}

void Hand::initTracker() {
	// init Tracker
	Tracker.trackWindow = handBox.boundingRect();
	Tracker.isTracked = false;

	// init Kalman filter
	Tracker.KalmanTracker.KF = cv::KalmanFilter(6, 2, 0);
	Tracker.KalmanTracker.state = cv::Mat(6, 1, CV_32F);
	Tracker.KalmanTracker.processNoise = cv::Mat(6, 1, CV_32F);

	Tracker.KalmanTracker.KF.transitionMatrix = *(cv::Mat_<float>(6, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5,
																		   0, 0, 1, 0,   1, 0, 0, 0, 0, 1, 0,   1, 
																		   0, 0, 0, 0,   1, 0, 0, 0, 0, 0, 0,   1);

	Tracker.KalmanTracker.KF.measurementMatrix = *(cv::Mat_<float>(2, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5);
	Tracker.KalmanTracker.measurement = cv::Mat(2, 1, CV_32F);
	Tracker.KalmanTracker.measurement.setTo(cv::Scalar(0));
    
    Tracker.KalmanTracker.KF.statePre.at<float>(0) = handBox.center.x;
    Tracker.KalmanTracker.KF.statePre.at<float>(1) = handBox.center.y;
    Tracker.KalmanTracker.KF.statePre.at<float>(2) = 0;
    Tracker.KalmanTracker.KF.statePre.at<float>(3) = 0;
    Tracker.KalmanTracker.KF.statePre.at<float>(4) = 0;
    Tracker.KalmanTracker.KF.statePre.at<float>(5) = 0;
    
    cv::setIdentity(Tracker.KalmanTracker.KF.measurementMatrix);
    cv::setIdentity(Tracker.KalmanTracker.KF.processNoiseCov, cv::Scalar::all(1e-4));
    cv::setIdentity(Tracker.KalmanTracker.KF.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(Tracker.KalmanTracker.KF.errorCovPost, cv::Scalar::all(.1));
    
	Tracker.isKalman = false;

	Tracker.kalmTrack.clear();
	Tracker.kalmTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));
}