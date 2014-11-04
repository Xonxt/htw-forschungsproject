// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "Hand.h"

Hand::Hand() {
	// new hand created!
	Parameters.moveAngle = NAN;
	Parameters.moveSpeed = -1;
}

Hand::~Hand() {
	//Tracker.hist.release();
	Tracker.KalmanTracker.KF.~KalmanFilter();
}

// add a new measurement for the Kalman tracker
void Hand::addKalmanMeasurement(const cv::Point coords) {
	// predict the location with KalmanFilter
	cv::Mat prediction = Tracker.KalmanTracker.KF.predict();

	Tracker.KalmanTracker.measurement(0) = coords.x;
	Tracker.KalmanTracker.measurement(1) = coords.y;
}

// get the latest predicted position by the Kalman
cv::Point Hand::getLatestKalmanPrediction() {
	// estimate the new position of the hand
	cv::Mat estimated = Tracker.KalmanTracker.KF.correct(Tracker.KalmanTracker.measurement);
	cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));

	Tracker.kalmTrack.push_back(statePt);

	return statePt;
}

void Hand::assignNewLocation(const Hand newHand) {
	handBox = newHand.handBox;
	Tracker.trackWindow = handBox.boundingRect();

	detectionBox = newHand.detectionBox;

	addKalmanMeasurement(cv::Point(newHand.handBox.center.x, newHand.handBox.center.y));
	Tracker.kalmTrack.push_back(getLatestKalmanPrediction());

	Tracker.camsTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));

	Tracker.isTracked = false;
}

void Hand::initTracker() {
	// init Tracker
	Tracker.trackWindow = handBox.boundingRect();
	Tracker.isTracked = false;

	Tracker.trackedFrames = 0;

	// init Kalman filter
	Tracker.KalmanTracker.KF = cv::KalmanFilter(6, 2, 0);
	Tracker.KalmanTracker.state = cv::Mat(6, 1, CV_32F);
	Tracker.KalmanTracker.processNoise = cv::Mat(6, 1, CV_32F);

	randn(Tracker.KalmanTracker.state, cv::Scalar::all(0), cv::Scalar::all(0.1));
	randn(Tracker.KalmanTracker.KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));

	Tracker.KalmanTracker.KF.transitionMatrix = *(cv::Mat_<float>(6, 6) <<
			1, 0, 1, 0, 0.5, 0,
			0, 1, 0, 1, 0, 0.5,
			0, 0, 1, 0, 1, 0,
			0, 0, 0, 1, 0, 1,
			0, 0, 0, 0, 1, 0,
			0, 0, 0, 0, 0, 1);

	Tracker.KalmanTracker.KF.measurementMatrix = *(cv::Mat_<float>(2, 6) << 
			1, 0, 1, 0, 0.5, 0,
			0, 1, 0, 1, 0, 0.5);

	Tracker.KalmanTracker.measurement = cv::Mat(2, 1, CV_32F);
	Tracker.KalmanTracker.measurement.setTo(cv::Scalar(0));

	Tracker.KalmanTracker.KF.statePre.at<float>(0) = handBox.center.x;
	Tracker.KalmanTracker.KF.statePre.at<float>(1) = handBox.center.y;
	Tracker.KalmanTracker.KF.statePre.at<float>(2) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(3) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(4) = 0;
	Tracker.KalmanTracker.KF.statePre.at<float>(5) = 0;

	cv::setIdentity(Tracker.KalmanTracker.KF.measurementMatrix);
	cv::setIdentity(Tracker.KalmanTracker.KF.processNoiseCov, cv::Scalar::all(1e-2));
	cv::setIdentity(Tracker.KalmanTracker.KF.measurementNoiseCov, cv::Scalar::all(1e-2));
	cv::setIdentity(Tracker.KalmanTracker.KF.errorCovPost, cv::Scalar::all(.1));

	Tracker.isKalman = false;

	Tracker.kalmTrack.clear();
	Tracker.kalmTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));

	Tracker.camsTrack.clear();
	Tracker.camsTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));
}
