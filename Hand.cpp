#include "stdafx.h"
#include "Hand.h"

Hand::Hand() {
	// new hand created!
    FingerParameters.cosThreshold = 0.5;
    FingerParameters.equalThreshold = 1e-7;
    FingerParameters.r = 40;
    FingerParameters.step = 16;
}

Hand::~Hand() {
    //Tracker.hist.release();
    Tracker.KalmanTracker.KF.~KalmanFilter();
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
    
    randn(Tracker.KalmanTracker.state, cv::Scalar::all(0), cv::Scalar::all(0.1));
    randn(Tracker.KalmanTracker.KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
    
    

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
    cv::setIdentity(Tracker.KalmanTracker.KF.processNoiseCov, cv::Scalar::all(1e-2));
    cv::setIdentity(Tracker.KalmanTracker.KF.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(Tracker.KalmanTracker.KF.errorCovPost, cv::Scalar::all(.1));
    
	Tracker.isKalman = false;

	Tracker.kalmTrack.clear();
	Tracker.kalmTrack.push_back(cv::Point(handBox.center.x, handBox.center.y));
}

// Determine if two floating point values are ~equal, with a threshold
bool Hand::isEqual(const double a, const double b) {
	return fabs(a - b) <= FingerParameters.equalThreshold;
}

// get the angle between two points in a contour
double Hand::getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt>0) ? contour[pt%size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt>r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;
	return (ux*vx + uy*vy) / sqrt((ux*ux + uy*uy)*(vx*vx + vy*vy));
}

// get the rotation between two points of the contour	
signed int Hand::getRotation(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt>0) ? contour[pt%size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt>r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;

	signed int res = (ux*vy - vx*uy);

	return res;
}

// extract the fingertips by processing the contour
int Hand::extractFingers() {
	int countFingers = 0;
	Parameters.fingers.clear();

	if (Parameters.handContour.size() <= 0)
		return -1;

	for (int j = 0; j < Parameters.handContour.size(); j += FingerParameters.step) {
		double cos0 = getPointsAangle(Parameters.handContour, j, FingerParameters.r);

		if ((cos0 > 0.5) && (j + FingerParameters.step < Parameters.handContour.size())) {
			double cos1 = getPointsAangle(Parameters.handContour, j - FingerParameters.step, FingerParameters.r);
			double cos2 = getPointsAangle(Parameters.handContour, j + FingerParameters.step, FingerParameters.r);
			double maxCos = std::max(std::max(cos0, cos1), cos2);
			bool equal = isEqual(maxCos, cos0);
			signed int z = getRotation(Parameters.handContour, j, FingerParameters.r);

			if (equal == 1 && z < 0) {
				// < init finger >
				Finger finger;
				finger.coordinates = Parameters.handContour[j];
				finger.Angles.orientationAngle = getAngle(handBox.center, finger.coordinates);

				finger.length = getDistance(finger.coordinates, handBox.center);
				// </ init finger>

				Parameters.fingers.push_back(finger);

				if (++countFingers >= 5)
					break;
			}
		}
	}
	return countFingers;
}