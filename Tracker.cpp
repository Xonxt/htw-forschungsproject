// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "Tracker.h"


Tracker::Tracker() {
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
	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		// extract the boolean mask
        cv::Mat smallMask;
        mask = cv::Mat(image.rows, image.cols, CV_8U);
		mask = cv::Mat::zeros(image.rows, image.cols, CV_8U);
		skinDetector.extrackskinMask(cv::Mat(image, (*it).roiRectange), smallMask, skinSegmMethod);

		// filter out the small blobs
		removeSmallBlobs(smallMask, 100);

		// do the morphology
		//bwMorph(smallMask, cv::MORPH_CLOSE, cv::MORPH_ELLIPSE, 1);
        
		// upscale the mask to original resolution
        if (smallMask.rows < image.rows && smallMask.cols < image.cols) {
            smallMask.copyTo(mask((*it).roiRectange));
        }
        else
            smallMask.copyTo(mask);

		// locate the new position for the hand:
		bool result = getNewPosition(*it);

		// if the tracking was unsuccessful, remove the hand
		if (!result) {
			it = hands.erase(it);
		}
		else { // if successful, 
			//extract hand contour
			extractContour(*it);
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
	float v_range[] = { 0, 255 };
	const float* ranges[] = { h_range, s_range, v_range };

	int channels[] = { 0, 1, 2 };

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

	if (statePt.x != 0 && statePt.y != 0)
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
/*
	if (hand.Tracker.kalmTrack.size() > 2) {
        cv::Point _pt = hand.Tracker.kalmTrack[hand.Tracker.kalmTrack.size()-1];
        cv::Point2f pt(_pt.x, _pt.y);
        
		cv::RotatedRect tempRect(pt, cv::Size2f(hand.detectionBox.width*1.25, hand.detectionBox.width*1.25), 0);
		trackWindow = tempRect.boundingRect();
	}
*/
	// assign new positions for 
	hand.Tracker.trackWindow = trackWindow;
	hand.handBox = trackBox;

    somethingIsTracked = true;
	return true;
}

// extract hand contour from the mask
void Tracker::extractContour(Hand& hand) {
	// a vector of vectors of points, containing all the contours in the image
	std::vector< std::vector<cv::Point> > contours;

	// a vector of contour hierarchy
	std::vector<cv::Vec4i> hierarchy;

	// attempt to extract the contours in the image
	try {
        // icrease the size of the bbox by 25%;
        cv::RotatedRect handBox = hand.handBox;
        handBox.size.width *= 1.25;
        handBox.size.height *= 1.15;
        cv::Rect handBoxRect = handBox.boundingRect();

		cv::Mat crop;
		cropRoi(mask, crop, handBoxRect);
        
		findContours(crop, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, handBox.boundingRect().tl());

		// if any contours found
		if (!contours.empty()) {
			// look for the largest contour
			int maxArea = -1, indx = 0;
			for (int i = 0; i<contours.size(); i++) {
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
	}
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