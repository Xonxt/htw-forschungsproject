// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "HandDetector.h"


HandDetector::HandDetector()
{
	frameNumber = 0;
}


// initialize detectot
bool HandDetector::initialize(bool webCam) {
	bool result = true;

	noPedestrians = false;

	if (!(result &= handCascade.load(HAND_DETECTOR_XML)))
		std::cout << "\tError initializing hand detector cascade!" << std::endl;

	try {
		pedestrianDetector.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
		//pedestrianDetector.load("hogcascade_pedestrians.xml");
	}
	catch (cv::Exception e) {
		std::cout << "\tError initializing hog pedestrian detector: " << e.what() << std::endl;
		result &= false;
	}

	isWebCam = webCam;

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
		// run pedestrian detector only if it's not a web cam
		// otherwise the entire frame is processed
		if (!isWebCam)
			pedestrianDetector.detectMultiScale(frameResized, pedestrians);

		if (pedestrians.empty()) {
			// if no pedestrians detected, process the entire image next
			noPedestrians = true;
			pedestrians.push_back(cv::Rect(0, 0, frameWidth, frameHeight));
		}
		else {
			noPedestrians = false;

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

	// remove pedestrian intersections
	if (pedestrians.size() > 1) {
		for (std::vector<cv::Rect>::iterator it = pedestrians.begin(); it != pedestrians.end() - 1; ++it) {
			cv::Rect first = *it;

			// iterate once again:
			for (std::vector<cv::Rect>::iterator it2 = (it + 1); it2 != pedestrians.end(); ++it2) {
				cv::Rect second = *it2;

				// intersection
				cv::Rect intersection = first & second;

				// compare intersection sizes
				if (intersection.area() >= (first.area() * 0.75)) {
					// remove the largest one
					pedestrians.erase(it2);
				}
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
		//cv::Mat frameCrop = (isWebCam || noPedestrians) ? frame.clone() : cv::Mat(frame.clone(), pedestrians[i]);
		cv::Mat frameCrop = (isWebCam || noPedestrians) ? frameResized : cv::Mat(frame, pedestrians[i]);

		// look for hand objects in the frame
		handCascade.detectMultiScale(frameCrop, handRects, 1.05, 4, CV_HAAR_FIND_BIGGEST_OBJECT);

		// iterate through located objects
		for (int j = 0; j < handRects.size(); j++) {
			// a temporary rect
			cv::Rect rect = handRects[i];

			// create temporary Hand object
			Hand tempHand;

			// if it was a webcam:
			if (isWebCam) {
				rect.x *= (frameWidth / RESIZE_WIDTH);
				rect.y *= (frameHeight / RESIZE_HEIGHT);
				rect.height *= (frameHeight / RESIZE_HEIGHT);
				rect.width *= (frameWidth / RESIZE_WIDTH);
			}

			//convert the rect into hand
			rect2Hand(rect, tempHand, pedestrians[i].tl());

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

	// detection box
	hand.detectionBox = inputRect;

	// init trackers
	hand.initTracker();

	// return
	outputHand = hand;
}
