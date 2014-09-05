#include "stdafx.h"
#include "Tracker.h"


Tracker::Tracker() {
	fout.open("tracker_time.csv", std::ios::trunc | std::ios::out);
	fout << "skin; blobs; camshift" << std::endl;
}

//initialize the tracker
bool Tracker::initialize() {
	bool result = true;
	
	skinSegmMethod = SKIN_SEGMENT_HSV;
    
    somethingIsTracked = false;

	return result;
}

// track the hands in the frame
void Tracker::trackHands(const cv::Mat inputFrame, std::vector<Hand>& hands) {
	// cope the current frame into an external variable
	inputFrame.copyTo(image);
    
    // nothing is tracked yet
    somethingIsTracked = false;

	//startClock = clock();
	// convert the image to HSV
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	//fout << clock() - startClock << "; ";

	// clear the backproj mask
	backprojection = cv::Mat::zeros(hsv.rows, hsv.cols, CV_8U);
	backprojection = cv::Scalar::all(0);

	// iterate through all hands
	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		startClock = clock();
		// extract the boolean mask
		skinDetector.extrackskinMask(image, mask, skinSegmMethod);
		fout << clock() - startClock << "; ";

		// delete everything outside the ROI
		cv::Mat rectMask = cv::Mat::zeros(mask.rows, mask.cols, CV_8U);
		cv::rectangle(rectMask, (*it).roiRectange, cv::Scalar(255), CV_FILLED);
		mask &= rectMask;

		startClock = clock();
		// filter ot the small blobs
		removeSmallBlobs(mask, 100);
		fout << clock() - startClock << "; ";

		startClock = clock();
		// locate the new position for the hand:
		bool result = getNewPosition(*it);
		fout << clock() - startClock << "; " << std::endl;

		// if the tracking was unsuccessful, remove the hand
		if (!result) {
			hands.erase(it);
		}
	}
}

// locate new hand position
bool Tracker::getNewPosition(Hand& hand) {
	// prepare the ROI
	cv::Rect selection = hand.handBox.boundingRect();
	cv::Rect trackWindow = hand.Tracker.trackWindow;

	// prepare the histogram
	int h_bins = 30; int s_bins = 32;
	int histSize[] = { h_bins, s_bins };

	float h_range[] = { 0, 179 };
	float s_range[] = { 0, 255 };
	const float* ranges[] = { h_range, s_range };

	int channels[] = { 0, 1 };

	// if the hand wasn't tracked yet, calculate histograms
	if (!hand.Tracker.isTracked) {
		// cut out a ROI
		cv::Mat roi(hsv, selection), maskroi(mask, selection);

		// Get the Histogram and normalize it
		calcHist(&hsv, 1, channels, mask, hand.Tracker.hist, 2, histSize, ranges, true, false);
		cv::normalize(hand.Tracker.hist, hand.Tracker.hist, 0, 255, cv::NORM_MINMAX, -1, cv::Mat());

		// set the hand to 'being tracked'
		hand.Tracker.isTracked = true;
	}

	// calculate the back projection
	cv::Mat backproj;
	cv::calcBackProject(&hsv, 1, channels, hand.Tracker.hist, backproj, ranges, 1, true);

	// apply the binary mask
	backproj &= mask;

	// perform the CamShift algorithm
	cv::RotatedRect trackBox = CamShift(backproj, trackWindow, 
										cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));

	// append the backprojection mask
	backprojection |= backproj;

	// was the tracking successful?
	bool badTracking = false;
	
	// check the track window
	if (trackWindow.area() <= 1) {
		badTracking = true;
	}

	// predict the location with KalmanFilter
	cv::Mat prediction = hand.Tracker.KalmanTracker.KF.predict();
	cv::Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

	// if the tracking was successful, add the measurement to KalmanFilter
	if (!badTracking) {
		hand.Tracker.isKalman = false;
	
		hand.Tracker.KalmanTracker.measurement(0) = trackBox.center.x;
		hand.Tracker.KalmanTracker.measurement(1) = trackBox.center.y;
	}

	// estimate the new position of the hand
	cv::Mat estimated = hand.Tracker.KalmanTracker.KF.correct(hand.Tracker.KalmanTracker.measurement);
	cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));

	hand.Tracker.kalmTrack.push_back(statePt);

	// if the tracking was unsuccessful, assign position by Kalman Filter
	if (badTracking) {
		trackBox = hand.handBox;

		// if it's a bad prediction
		if (statePt.x <= 0 || statePt.y <= 0) {
			trackBox.size.width += (trackBox.size.width * 0.05);
			trackBox.size.height += (trackBox.size.height * 0.05);
		}
		else { // use kalman prediction
			trackBox.center.x = statePt.x;
			trackBox.center.y = statePt.y;
			hand.Tracker.isKalman = true;
		}

		trackWindow = trackBox.boundingRect();		
	}

	// assign new positions for 
	hand.Tracker.trackWindow = trackWindow;
	hand.handBox = trackBox;

    somethingIsTracked = true;
	return true;
}

// extract hand contour from the mask
void Tracker::extractContour(Hand& hand) {

}

// change the skin segmentation method
void Tracker::changeSkinMethod(SkinSegmMethod method) {
	skinSegmMethod = method;
}

// get current segmentation method
SkinSegmMethod Tracker::getSkinMethod() {
	return skinSegmMethod;
}

// retrieve the skin mask for debugging purposes
void Tracker::getSkinMask(cv::Mat& outputSkinMask) {
    if (somethingIsTracked)
        cv::cvtColor(backprojection, outputSkinMask, cv::COLOR_GRAY2BGR);
    else
        image.copyTo(outputSkinMask);
}

// filter out the blobs smaller than a threshold
void Tracker::removeSmallBlobs(cv::Mat& inputImage, const double blobSize) {
	// Only accept CV_8UC1
	if (inputImage.channels() != 1 || inputImage.type() != CV_8U) {
		std::cout << "chan != 1 or !cv_u8" << std::endl;
		return;
	}

	cv::Scalar black = CV_RGB(0, 0, 0);
	cv::Scalar white = CV_RGB(255, 255, 255);

	// Find all contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(inputImage.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	for (int i = 0; i < contours.size(); i++) {
		// Calculate contour area
		double area = cv::contourArea(contours[i]);

		// Remove small objects by drawing the contour with black color
		if (area >= 0) {
			if (area <= blobSize)
				cv::drawContours(inputImage, contours, i, black, -1);
			else
				cv::drawContours(inputImage, contours, i, white, -1);
		}
	}
}