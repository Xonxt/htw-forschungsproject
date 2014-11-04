// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "Tracker.h"
#include <sstream>

Tracker::Tracker() {
	k = 0;
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

	// convert the image to HSV
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

	// clear the backproj mask
	backprojection = cv::Mat::zeros(hsv.rows, hsv.cols, CV_8U);
	backprojection = cv::Scalar::all(0);

	// iterate through all hands
	//for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
	for (int i = 0; i < hands.size(); i++) {
		// extract the boolean mask
		cv::Mat smallMask;
		mask = cv::Mat(image.rows, image.cols, CV_8U);
		mask = cv::Mat::zeros(image.rows, image.cols, CV_8U);
		skinDetector.extrackskinMask(cv::Mat(image, hands[i].roiRectange), smallMask, skinSegmMethod);

		// filter out the small blobs
		removeSmallBlobs(smallMask, 100);

		// do the morphology
		//bwMorph(smallMask, cv::MORPH_CLOSE, cv::MORPH_ELLIPSE, 1);

		// upscale the mask to original resolution
		if (smallMask.rows < image.rows && smallMask.cols < image.cols) {
			smallMask.copyTo(mask(hands[i].roiRectange));
		}
		else
			smallMask.copyTo(mask);

		// locate the new position for the hand:
		bool result = getNewPosition(hands[i]);

		// if the tracking was unsuccessful, remove the hand
		if (!result) {
			hands.erase(hands.begin() + i);
		}
		else { // if successful, 
			//extract hand contour
			if (extractContour(hands[i]) == -1) {
				// if hand empty, remove it
				hands.erase(hands.begin() + i);
			}
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
//	float v_range[] = { 0, 255 };
	const float* ranges[] = { h_range, s_range};

	int channels[] = { 0, 1};	

	// if the hand wasn't tracked yet, calculate histograms
	if (!hand.Tracker.isTracked) {
		cv::Mat roi(hsv, selection), maskroi(mask, selection);

		// Get the Histogram and normalize it
		calcHist(&roi, 1, channels, maskroi, hand.Tracker.hist, 2, histSize, ranges, true, false);		
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

	// Kalman prediction
	cv::Mat prediction = hand.Tracker.KalmanTracker.KF.predict();

	// check the track window
	if (trackWindow.area() <= 1) {
		badTracking = true;
		// return false;
		cv::Point predictedPt(prediction.at<float>(0), prediction.at<float>(1));
		hand.Tracker.KalmanTracker.measurement(0) = predictedPt.x;
		hand.Tracker.KalmanTracker.measurement(1) = predictedPt.y;
	}
	else {
		hand.Tracker.KalmanTracker.measurement(0) = trackBox.center.x;
		hand.Tracker.KalmanTracker.measurement(1) = trackBox.center.y;		
	}

	// Kalman estimate
	cv::Mat estimated = hand.Tracker.KalmanTracker.KF.correct(hand.Tracker.KalmanTracker.measurement);
	cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
	hand.Tracker.kalmTrack.push_back(statePt);

	// Try to forbid sudden HUGE changes in the window size
	float size_A = trackBox.size.height, size_B = hand.detectionBox.height;
	float sizeDiff = size_A / size_B; //MAX(size_A, size_B) / MIN(size_A, size_B);
	hand.sizeDiff = sizeDiff;

	// fix the trackbox using Kalman:
//	trackBox.center.x = statePt.x;
//	trackBox.center.y = statePt.y;
//	trackBox.size = cv::Size2f(hand.detectionBox.width, hand.detectionBox.height);

	// assign new values
	hand.Tracker.trackWindow = trackWindow;
	hand.handBox = trackBox;

	somethingIsTracked = true;

	// write
	/*std::ostringstream ofs;
	k++;	
	ofs << "backproj_" << k << ".jpg";
	cv::imwrite(ofs.str(), backprojection);
	ofs.flush();
	ofs.str("");
	ofs << "mask_" << k << ".jpg";
	cv::imwrite(ofs.str(), mask);*/

	return true;
}

// extract hand contour from the mask
int Tracker::extractContour(Hand& hand) {

	if (hand.handBox.size.width <= 0 || hand.handBox.size.height <= 0)
		return -1;

	// a vector of vectors of points, containing all the contours in the image
	std::vector< std::vector<cv::Point> > contours;

	// a vector of contour hierarchy
	std::vector<cv::Vec4i> hierarchy;

	// attempt to extract the contours in the image
	try {

		// icrease the size of the bbox by 25%;
		cv::RotatedRect handBox = hand.handBox;
		handBox.size.width *= 1.3;
		handBox.size.height *= 1.2;
		cv::Rect handBoxRect = handBox.boundingRect();

		cv::Mat crop;
		cropRoi(mask, crop, handBoxRect);
		
		// check if there's enough color inside this region
		float whiteRatio = (float)cv::countNonZero(crop) / (float)handBoxRect.area();

		if (whiteRatio < 0.05) {
			return -1;
		}

		findContours(crop, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, handBox.boundingRect().tl());

		// if any contours found
		if (!contours.empty()) {
			// look for the largest contour
			int maxArea = -1, indx = 0;
			for (int i = 0; i < contours.size(); i++) {
				if (cv::contourArea(contours[i]) > maxArea) {
					maxArea = cv::contourArea(contours[i]);
					indx = i;
				}
			}
			hand.Parameters.handContour = contours[indx];
		}

	} // end-try
	catch (cv::Exception& ex) {
		std::cout << "Exception caught while extracting a contour: " << std::endl << ex.what() << std::endl;
		hand.Parameters.handContour.clear();
		return -2;
	}

	return 0;
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
	else {
		image.copyTo(outputSkinMask);
	}
}

// filter out the blobs smaller than a threshold
void Tracker::removeSmallBlobs(cv::Mat& inputImage, const double blobSize) {
	// Only accept CV_8UC1
	if (inputImage.channels() != 1 || inputImage.type() != CV_8U) {
		std::cout << "chan != 1 or !cv_u8" << std::endl;
		return;
	}

	// Find all contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(inputImage.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	for (int i = 0; i < contours.size(); i++) {
		// Calculate contour area
		double area = cv::contourArea(contours[i]);

		// Remove small objects by drawing the contour with black color
		if (area >= 0) {
			if (area <= blobSize)
				cv::drawContours(inputImage, contours, i, FP_COLOR_BLACK, -1);
			else
				cv::drawContours(inputImage, contours, i, FP_COLOR_WHITE, -1);
		}
	}
}

void Tracker::bwMorph(cv::Mat& inputImage, const int operation, const int mShape, const int mSize) {
	cv::Mat element = cv::getStructuringElement(mShape, cv::Size(2 * mSize + 1, 2 * mSize + 1),
		cv::Point(mSize, mSize));
	cv::morphologyEx(inputImage, inputImage, operation, element);
}

void Tracker::cropRoi(const cv::Mat inputImage, cv::Mat& outputCrop, cv::Rect& roiRectangle) {
	cv::Rect roiRect;

	roiRect.x = (roiRectangle.x < 0) ? 0 : roiRectangle.x;
	roiRect.y = (roiRectangle.y < 0) ? 0 : roiRectangle.y;

	roiRect.width = (roiRect.x - 1 + roiRectangle.width) >= inputImage.cols ? (inputImage.cols - 1 - roiRect.x) : roiRectangle.width;
	roiRect.height = (roiRect.y - 1 + roiRectangle.height) >= inputImage.rows ? (inputImage.rows - 1 - roiRect.y) : roiRectangle.height;

	roiRect.width -= 1;
	roiRect.height -= 1;

	roiRect.x = abs(roiRect.x);
	roiRect.y = abs(roiRect.y);
	roiRect.width = abs(roiRect.width);
	roiRect.height = abs(roiRect.height);

	outputCrop = cv::Mat(inputImage, roiRect);

	roiRectangle = roiRect;
}