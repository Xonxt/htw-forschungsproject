#include "stdafx.h"
#include "FrameProcessor.h"

FrameProcessor::FrameProcessor()
{
	hands.clear();
}


// initialize the processor
bool FrameProcessor::initialize() {
	bool result = true;

	// don't show mask at first
	showMask = false;

	// clear the hands list
	hands.clear();

	if (!(result &= handDetector.initialize())) {
		std::cout << "Error initializing Hand Detector object" << std::endl;
	}

	if (!(result &= handTracker.initialize())) {
		std::cout << "Error initializing the Hand Tracker!" << std::endl;
	}

	if (!(result &= faceCascade.load(FACE_DETECTOR_XML)))
		std::cout << "\tError initializing face detector cascade!" << std::endl;

	return result;
}

// process frame, find hands, detect gestures
void FrameProcessor::processFrame(cv::Mat& frame) {
	// create the hands vector
	std::vector<Hand> detectedHands;
	detectedHands.clear();

	// a list of pedestrian ROIs
	std::vector<cv::Rect> pedestrians;

	// first track the present hands in the frame
	try {
		handTracker.trackHands(frame, hands);
	}
	catch (cv::Exception ex) {
		std::cout << "Exception caught while tracking: " << std::endl << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Some other kind of exception caught" << std::endl;
	}

	// now detect (new) hands in the frame
	handDetector.detectHands(frame, detectedHands, pedestrians);

	// were any new hands added?
	bool newHandsAdded = false;

	// add new hands to the tracking list,
	// but remove those already being tracked
	for (std::vector<Hand>::iterator it = detectedHands.begin(); it != detectedHands.end(); ++it) {
		// take the temporary hand
		Hand tempHand = *it;

		// does the position of the already tracked and newly detected hands coincide?
		bool sameHand = false;

		// check if this hand's position intersects with the position of a tracked hand
		for (std::vector<Hand>::iterator it2 = hands.begin(); it2 != hands.end(); ++it2) {
			// if the area of intersection is 50% of the hand's size or more
			cv::Rect intersection = tempHand.handBox.boundingRect() & (*it2).handBox.boundingRect();
			if (intersection.area() >= (tempHand.handBox.boundingRect().area() * 0.5)) {
				// then this hand is already being tracked, correct position
				(*it2).assignNewLocation(tempHand.handBox);
				sameHand = true;
				break;
			}
		}

		// if this hand  wasn't tracked before, add it to list
		if (!sameHand) {
			hands.push_back(tempHand);
			newHandsAdded = true;
		}
	}	

	// now check if new hands were added and then delete face regions
	if (newHandsAdded) {
		std::vector<cv::Rect> faces;

		// look for faces in the frame
		faceCascade.detectMultiScale(frame, faces, 1.1, 3);

		// iterate through hands
		for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
			// iterate through faces:
			for (std::vector<cv::Rect>::iterator fc = faces.begin(); fc != faces.end(); ++fc) {
				cv::Rect intersection = (*it).handBox.boundingRect() & (*fc);

				// check if the intersection area too big
				if (intersection.area() >= ((*it).handBox.boundingRect().area() * 0.5)) {
					// delete that hand from list
					hands.erase(it);
				}
			}
		}
	}

	// replace the frame with a mask if needed
	if (showMask) {
		handTracker.getSkinMask(frame);
	}

	// draw the AR-things on the frame
	drawFrame(frame);
}

// draw everything on frame (hands, fingers, etc.)
void FrameProcessor::drawFrame(cv::Mat& frame) {
	// iterate through our vector of hands
	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		// put a rectangle|ellepse on the image
		cv::ellipse(frame, (*it).handBox, cv::Scalar(0, 0, 255), 2);
	}
}

// change skin segmentation method
void FrameProcessor::changeSkinMethod() {
	SkinSegmMethod method = handTracker.getSkinMethod();

	switch (method) {
	case SKIN_SEGMENT_ADAPTIVE:
		handTracker.changeSkinMethod(SKIN_SEGMENT_HSV);
		break;
	case SKIN_SEGMENT_HSV:
		handTracker.changeSkinMethod(SKIN_SEGMENT_YCRCB);
		break;
	case SKIN_SEGMENT_YCRCB:
		handTracker.changeSkinMethod(SKIN_SEGMENT_ADAPTIVE);
		break;
	default:
		break;
	}
}

// toggle show boolean skin-mask
void FrameProcessor::toggleShowMask() {
	showMask = !showMask;
}