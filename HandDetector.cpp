#include "stdafx.h"
#include "HandDetector.h"


HandDetector::HandDetector()
{
	frameNumber = 0;
}


// initialize detectot
bool HandDetector::initialize() {
	bool result = true;

	if (!(result &= handCascade.load(HAND_DETECTOR_XML)))
		std::cout << "\tError initializing hand detector cascade!" << std::endl;

	try {
		pedestrianDetector.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
	} 
	catch (cv::Exception e) {
		std::cout << "\tError initializing hog pedestrian detector: " << e.what() << std::endl;
		result &= false;
	}
	
	return result;
}

// detect all hands in frame
void HandDetector::detectHands(const cv::Mat frame, std::vector<Hand>& hands, std::vector<cv::Rect>& rois) {
	// clear the vector
	hands.clear();

	// get frame's initial size
	int frameHeight = frame.rows;
	int frameWidth = frame.cols;

	// downsize the frame
	cv::Mat frameResized;
	cv::resize(frame, frameResized, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT));
	
	// pedestrians will be detected only every N drames
	if ((frameNumber++ % PEDESTRIANS_SKIP_FRAMES) == 0) {
		pedestrians.clear();
		pedestrianDetector.detectMultiScale(frameResized, pedestrians);

		if (pedestrians.empty()) {
			// if no pedestrians detected, process the entire image next
			pedestrians.push_back(cv::Rect(0, 0, frameWidth, frameHeight));
		}
		else {
			for (int i = 0; i < pedestrians.size(); i++) {
				// if pedestrians detected, rescale each ROI back to original image size
				pedestrians[i].x *= (frameWidth / RESIZE_WIDTH);
				pedestrians[i].y *= (frameHeight / RESIZE_HEIGHT);
				pedestrians[i].height *= (frameHeight / RESIZE_HEIGHT);
				pedestrians[i].height *= 0.6; // limit to only upper-body region
				pedestrians[i].width *= (frameWidth / RESIZE_WIDTH);
				// additionally enlarge the region a little (make it 20% cooler)
				pedestrians[i] -= cv::Point(pedestrians[i].width * 0.1, pedestrians[i].height * 0.1);
				pedestrians[i] += cv::Size(pedestrians[i].width * 0.2, pedestrians[i].height * 0.2);
			}
		}
	}

	// return the pedestrians as ROIs
	rois = pedestrians;
	
	// detect hands in ROIs
	for (int i = 0; i < pedestrians.size(); i++) {
		// prepare a temporary hand-Rects vector
		std::vector<cv::Rect> handRects;

		// cut out the ROI from the original frame
		cv::Mat frameCrop = cv::Mat(frame, pedestrians[i]);

		// look for hand objects in the frame
		handCascade.detectMultiScale(frameCrop, handRects, 1.1, 4, CV_HAAR_FIND_BIGGEST_OBJECT);

		// iterate through located objects
		for (int j = 0; j < handRects.size(); j++) {
			// create temporary Hand object
			Hand tempHand;

			//convert the rect into hand
			rect2Hand(handRects[j], tempHand, pedestrians[i].tl());

			// assign the hands ROI (the pedestrian rectange)
			tempHand.roiRectange = pedestrians[i];

			//add the hand to the hands list
			hands.push_back(tempHand);
		}
	}
}

// convert cv::Rect to cv::RotatedRect
void HandDetector::rect2rotatedRect(const cv::Rect inputRect, cv::RotatedRect& outputRect) {
	// Get Rectangle Size
	cv::Size2f size;
	size.width = inputRect.width;
	size.height = inputRect.height;

	// Get Center Point
	cv::Point2f center;
	center.x = inputRect.x + (int)(inputRect.width / 2);
	center.y = inputRect.y + (int)(inputRect.height / 2);

	// Create RotatedRect with angle 0.0
	outputRect = cv::RotatedRect(center, size, 0.0f);
}

// convert cv::Rect to Hand
void HandDetector::rect2Hand(const cv::Rect inputRect, Hand& outputHand, const cv::Point shift) {
	// create a Hand object
	Hand hand;
	
	// create a RotatedRect bounding box
	cv::RotatedRect bbox;

	// convert Rect to RotatedRect
	rect2rotatedRect(inputRect + shift, bbox);

	// set the hand bounding box
	hand.handBox = bbox;

	// init trackers
	hand.initTracker();

	// return
	outputHand = hand;
}
