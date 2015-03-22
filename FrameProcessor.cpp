// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

#include "stdafx.h"
#include "FrameProcessor.h"

FrameProcessor::FrameProcessor()
{

}

FrameProcessor::FrameProcessor(const cv::Size frameSize)
{
	//isWebCam = webCam;
	hands.clear();

	gestureAnalyzer = GestureAnalyzer(frameSize);

	frameNum = 0;
}

FrameProcessor::~FrameProcessor() {

}

// initialize the processor
bool FrameProcessor::initialize(bool isWebCam) {
	bool result = true;

	// don't show mask at first
	showMask = false;

	// don't show the contour at the beginning
	showContour = false;

	// show bounding box at the beginning
	showBoundingBox = false;

	// fingers are not shown at first
	showFingers = false;

	// the hand text is not shown at the beginning
	showHandText = true;

	// showing text information
	showInformation = false;

	// clear the hands list
	hands.clear();

	if (!(result &= handDetector.initialize(isWebCam))) {
		std::cout << "Error initializing Hand Detector object" << std::endl;
	}

	if (!(result &= handTracker.initialize())) {
		std::cout << "Error initializing the Hand Tracker!" << std::endl;
	}

	if (isWebCam) {
		handTracker.changeSkinMethod(SKIN_SEGMENT_YCRCB);
	}

	if (!(result &= faceCascade.load(FACE_DETECTOR_XML))) {
		std::cout << "\tError initializing face detector cascade!" << std::endl;
	}

	return result;
}

// process frame, find hands, detect gestures
void FrameProcessor::processFrame(cv::Mat& frame) {

	// perform hand detection and tracking in the frame
	detectAndTrack(frame);

	// analyze the hands, the contours, the parameters
	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		gestureAnalyzer.analyzeHand(*it);
	}

	// replace the frame with a mask if needed
	if (showMask == true) {
		handTracker.getSkinMask(frame);
	}

	// draw the pretty stuff on the frame
	try {
		drawFrame(frame);
	}
	catch (cv::Exception ex) {
		std::cout << "OpenCV Exception caught while drawing: " << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Some kind of exception caught while drawing!" << std::endl;
	}
}

// detect and track hands in the frame
void FrameProcessor::detectAndTrack(const cv::Mat& frame) {
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

	// recalculate the color ranges for each hand:
	for (int i = 0; i < detectedHands.size(); i++) {
		//detectedHands[i].recalculateRange(frame, handTracker.getSkinMethod(), true);
	}

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
			// if the area of intersection is 75% of the hand's size or more
			cv::Rect intersection = tempHand.handBox.boundingRect() & (*it2).handBox.boundingRect();
			if (intersection.area() >= (tempHand.handBox.boundingRect().area() * 0.75)) {
				// then this hand is already being tracked, correct position
				(*it2).assignNewLocation(tempHand);
				//(*it2).recalculateRange(frame, handTracker.getSkinMethod(), true);
				sameHand = true;
				break;
			}
		}

		// if this hand  wasn't tracked before, add it to list
		if (!sameHand) {
			//tempHand.recalculateRange(frame, handTracker.getSkinMethod(), true);
			hands.push_back(tempHand);
			newHandsAdded = true;
		}
	}

	// remove intersecting regions (if two hands located in one position)
	if (hands.size() > 1) {
		int N = hands.size();
		for (int i = 0; i < N && N > 1; i++) {
			cv::Rect firstHand = hands[i].handBox.boundingRect();
			for (int j = 0; j < N && N > 1; j++) {
				if (i == j) {
					continue;
				}
				cv::Rect secondHand = hands[j].handBox.boundingRect();
				// intersection
				cv::Rect intersection = firstHand & secondHand;

				// take the smallest hand
				cv::Rect smallestHand = (firstHand.area() < secondHand.area()) ? firstHand : secondHand;

				// compare intersection sizes
				if (intersection.area() > 0 && hands.size() > 1) {
					if (hands[i].Tracker.kalmTrack.size() > hands[j].Tracker.kalmTrack.size()) {
						hands.erase(hands.begin() + j--);
					}
					else {
						hands.erase(hands.begin() + i--);
					}
					/*
					if (hands[i].Parameters.handContour.empty() && !hands[j].Parameters.handContour.empty()) {
					hands.erase(hands.begin() + j--);
					}
					else if (!hands[i].Parameters.handContour.empty() && hands[j].Parameters.handContour.empty()) {
					hands.erase(hands.begin() + i--);
					}
					else {
					// remove the largest region
					if ((firstHand.area() < secondHand.area())) {
					hands.erase(hands.begin() + i--);
					}
					else {
					hands.erase(hands.begin() + j--);
					}
					}*/
					//		std::cout << "hand removed because of intersection!" << std::endl;
					N--;
				}

				if (N < 2 || i == N || j == N) {
					break;
				}
			}
		}
	}

	// remove hands with bounding boxes, that go beyond the image borders
	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		Hand temp = *it;

		bool remove = false;

		if (temp.handBox.center.x <= 0 || temp.handBox.center.y <= 0)
			remove = true;

		if (temp.handBox.size.height <= 0 || temp.handBox.size.height >= temp.roiRectange.height || temp.handBox.size.height == NAN)
			remove = true;

		if (temp.handBox.size.width <= 0 || temp.handBox.size.width >= temp.roiRectange.width || temp.handBox.size.width == NAN)
			remove = true;

		if (remove) {
			it = hands.erase(it);
			//	std::cout << "hand removed because it was out of borders!" << std::endl;
		}

		if (it == hands.end())
			break;
	}

	// now check if new hands were added and then delete face regions
	if ((frameNum++ % 5) == 0 && hands.size() > 0) {
		std::vector<cv::Rect> faces;

		faces.clear();

		// iterate through pedestrians:
		for (std::vector<cv::Rect>::iterator pd = pedestrians.begin(); pd != pedestrians.end(); ++pd) {
			// cut out a part of the image
			cv::Mat cutOut = cv::Mat(frame, *pd);

			bool resize = false;

			if (cutOut.cols > RESIZE_WIDTH || cutOut.rows > RESIZE_HEIGHT) {
				resize = true;

				cv::resize(cutOut.clone(), cutOut, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT));
			}

			// prepare a temporary storage for faces
			std::vector<cv::Rect> tempFaces;

			// look for faces in the cutout
			faceCascade.detectMultiScale(cutOut, tempFaces);//, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT);

			// append the located faces to the Faces vector
			for (std::vector<cv::Rect>::iterator fc2 = tempFaces.begin(); fc2 != tempFaces.end(); ++fc2) {
				cv::Rect face = *fc2;

				if (resize) {
					face.x *= (frame.cols / RESIZE_WIDTH);
					face.y *= (frame.rows / RESIZE_HEIGHT);
					face.height *= (frame.rows / RESIZE_HEIGHT);
					face.width *= (frame.cols / RESIZE_WIDTH);
				}

				// apply shift
				face += (*pd).tl();

				// add the face to the vector
				faces.push_back(face);
			}
		}

		if (faces.size() > 0) {
			// iterate through hands
			for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
				// iterate through faces:
				for (std::vector<cv::Rect>::iterator fc = faces.begin(); fc != faces.end(); ++fc) {
					cv::Rect intersection = (*it).handBox.boundingRect() & (*fc);

					// check if the intersection area too big
					if (intersection.area() > 0) {
						it = hands.erase(it);
						//std::cout << "hand removed due to intersection with a face!" << std::endl;

						if (it == hands.end())
							break;
					}
				}
				if (it == hands.end())
					break;
			}
		}

		// also try to recalculate colors here
		//std::for_each(hands.begin(), hands.end(), [&](Hand& hand) {hand.recalculateRange(frame, handTracker.getSkinMethod(), true); });
	}
}

// draw everything on frame (hands, fingers, etc.)
void FrameProcessor::drawFrame(cv::Mat& frame) {
	// iterate through our vector of hands
	int clr = 0;

	// draw a graphical promt:
	if (hands.empty()) {

		gestureList.clear();

		cv::Mat promt = cv::imread("promt.jpg");

		cv::Rect rect;
		rect.x = (frame.cols / 2) - (promt.cols / 2);
		rect.y = (frame.rows / 2) - (promt.rows / 2);
		rect.width = promt.cols;
		rect.height = promt.rows;

		//cv::Mat imageRoi = frame(rect);
		cv::addWeighted(frame(rect), 1.0, promt, 0.9, 0, frame(rect));

	}

	for (Hand hand : hands) {
		// put a rectangle|ellipse on the image
		if (showBoundingBox) {
			if (!hand.Tracker.isKalman)
				cv::ellipse(frame, hand.handBox, FP_COLOR_WHITE, 2);
			else
				cv::rectangle(frame, hand.handBox.boundingRect(), FP_COLOR_WHITE, 2);

			//cv::rectangle(frame, hand.Tracker.trackWindow, FP_COLOR_GREEN, 2);
		}

		// show the ROIs
		//cv::rectangle(frame, (*it).roiRectange, fpColors[clr], 2);

		// show the contours
		if (showContour && !hand.Parameters.handContour.empty()) {
			std::vector<std::vector<cv::Point> > contour;
			contour.push_back(hand.Parameters.handContour);
			cv::drawContours(frame, contour, 0, FP_COLOR_WHITE, 2);
		}

		// show fingertips
		if (showFingers && !hand.Parameters.fingers.empty()) {
			int radius = floor(((hand.handBox.size.height + hand.handBox.size.width) / 2) * 0.05);

			for (std::vector<Finger>::iterator fg = hand.Parameters.fingers.begin(); fg != hand.Parameters.fingers.end(); ++fg) {
				//inner
				cv::circle(frame, (*fg).coordinates, radius*0.5, FP_COLOR_WHITE, -1);
				// outer
				cv::circle(frame, (*fg).coordinates, radius*1.5, FP_COLOR_WHITE, 4);
			}
		}

		// SHOW GESTURE NAME
		if (showHandText) {
			if (hand.handGesture.newGesture) {
				cv::Point textPoint(hand.handBox.boundingRect().br().x, hand.handBox.boundingRect().tl().y);
				//cv::putText(frame, hand.handGesture.getGestureName(), textPoint, CV_FONT_HERSHEY_PLAIN, 4, FP_COLOR_RED, 7);
				//drawGlowText(frame, textPoint, hand.handGesture.getGestureName());

				gestureList.push_back(hand.handGesture.getGestureType());

				int N = gestureList.size();

				if (N >= 3) {

					HandGesture temp = gestureList.back();

					int i = N - 1;
					bool isDrawing = false;
					while (i-- > 0) {
						if ((int)gestureList[i] >= (int)GESTURE_SWIPE_UP) {
							isDrawing = true;
							continue;
						}
						else {
							if (gestureList[i] == temp && isDrawing) {
								gestureList.erase(gestureList.end() - 1);
								break;
							}
						}
					}

					/* if ((int)gestureList[N-2] >= 9 && gestureList.back() == gestureList[N-3]) {
							 gestureList.erase(gestureList.end() - 1);
							 }*/
				}

				if (gestureList.size() > 5) {
					gestureList.erase(gestureList.begin());
				}
			}
		}

		clr++;
	}

	frame.convertTo(frame, -1, 1, -35);

	// add glowy effect
	if (!showMask) {
		drawGlowyHands(frame, hands);
	}

	// draw glowy lines
	drawGlowyLines(frame, hands);

	// display system information text
	std::vector<std::string> strings;

	if (showMask)
		strings.push_back("showing mask");

	if (showContour)
		strings.push_back("showing contour");

	if (showFingers)
		strings.push_back("showing fingertips");

	switch (handTracker.getSkinMethod()) {
	case SKIN_SEGMENT_ADAPTIVE:
		strings.push_back("Skin segmentation: adaptive");
		break;
	case SKIN_SEGMENT_HSV:
		strings.push_back("Skin segmentation: HSV");
		break;
	case SKIN_SEGMENT_YCRCB:
		strings.push_back("Skin segmentation: YCrCb");
		break;
	default:
		break;
	}

	// show the information about the system
	if (showInformation) {

		int maxlen = 0;
		for (std::string str : strings) {
			if (str.length() > maxlen)
				maxlen = str.length();
		}

		drawRectangle(frame, cv::Rect(10, 10, maxlen * 19, strings.size() * 25 + 25), 100);
		for (int i = 0; i < strings.size(); i++) {
			cv::putText(frame, strings[i], cv::Point(20, 20 + (i + 1) * 25), CV_FONT_HERSHEY_PLAIN, 2, FP_COLOR_WHITE, 2);
		}
	}

	// show gestures as a list in right-bottom corner
	if (showHandText && !hands.empty()) {
		int scalarWidth = 14;
		int scalarHeight = 5;

		cv::Rect rect(10, 10, scalarWidth * 28, scalarHeight * 45 + 25);
		rect.x = frame.cols - rect.width - 10;
		rect.y = frame.rows - rect.height - 10;

		drawRectangle(frame, rect);
		for (int i = 0; i < gestureList.size(); i++) {
			cv::putText(frame, GestureNames[gestureList[i]], cv::Point(rect.x + 10, rect.y + 10 + (i + 1) * 45), CV_FONT_HERSHEY_PLAIN, 3, FP_COLOR_WHITE, 5);
		}
	}


	// draw the gesture sequence on the top:
	if (gestureList.size() > 1) {
		RecognizedSequence _recognizedSequence = sequenceRecognizer.recognizeSequence(gestureList);
		if (_recognizedSequence.getScore() >= 0.6) {
			sequenceResult = RecognizedSequence(_recognizedSequence.getName(), _recognizedSequence.getScore());
			gestureList.clear();
		}
	}
	/*
			int N = gestureList.size();

			if (N > 0) {
			if ((int)gestureList[N-1] >= (int)GESTURE_DRAWING_CIRCLE) {
			sequenceResult = RecognizedSequence(GestureNames[gestureList[N-1]], 1.0);
			}
			}
			*/
	if (sequenceResult.getScore() > 0 && gestureList.size() <= 2) {
		cv::Rect rect(10, 10, sequenceResult.getName().length() * 34, strings.size() * 45 + 25);
		rect.x = frame.cols / 2 - rect.width / 2;
		rect.y = frame.rows - rect.height - 50;
		drawRectangle(frame, rect, 100);

		cv::putText(frame, sequenceResult.getName(), cv::Point(rect.x + 10, rect.y + rect.height * 2 / 3),
			CV_FONT_HERSHEY_PLAIN, 3, FP_COLOR_WHITE, 5);
	}

	if (gestureList.size() > 3)
		sequenceResult = RecognizedSequence(" ", 0.0f);
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

// toggle show hand contours
void FrameProcessor::toggleShowContour() {
	showContour = !showContour;
}

// toggle showing the bounding box
void FrameProcessor::toggleShowBoundingBox() {
	showBoundingBox = !showBoundingBox;
}

void FrameProcessor::toggleShowFingers() {
	showFingers = !showFingers;
}

// toggle showing information about the hand
void FrameProcessor::toggleShowHandText() {
	showHandText = !showHandText;
}

// toggle showing on-screen display information
void FrameProcessor::toggleShowInformation() {
	showInformation = !showInformation;
}

// a function to draw a darkened rectangle
void FrameProcessor::drawRectangle(cv::Mat& frame, const cv::Rect rectangle, int shift) {

	for (int i = rectangle.x; i < rectangle.x + rectangle.width; i++) {
		for (int j = rectangle.y; j < rectangle.y + rectangle.height; j++) {
			cv::Vec3b color = frame.at<cv::Vec3b>(j, i);
			color -= cv::Vec3b(shift, shift, shift);
			if (color[0] < 0)
				color[0] = 0;
			if (color[1] < 0)
				color[1] = 0;
			if (color[2] < 0)
				color[2] = 0;
			frame.at<cv::Vec3b>(j, i) = color;
		}
	}
	cv::rectangle(frame, rectangle, FP_COLOR_WHITE, 2);
}

// draw a text on the image, making in glowy and pretty
void FrameProcessor::drawGlowText(cv::Mat& frame, cv::Point& point, const std::string& text) {
	cv::Size size(text.length() * 25 + 30, 30 + 30);
	cv::Mat image = cv::Mat(size, CV_8U);
	image = cv::Mat::zeros(size, CV_8U);
	cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);
	cv::putText(image, text, cv::Point(15, 40), CV_FONT_HERSHEY_DUPLEX & CV_FONT_BOLD, 1.25, FP_COLOR_WHITE, 3);
	bwMorph(image, cv::MORPH_DILATE, cv::MORPH_ELLIPSE, 3);
	cv::blur(image, image, cv::Size(10, 10));
	cv::putText(image, text, cv::Point(15, 40), CV_FONT_HERSHEY_DUPLEX & CV_FONT_BOLD, 1.25, FP_COLOR_WHITE, 3);

	//size.width *= 0.8;

	cv::resize(image, image, size);

	cv::Rect roi = cv::Rect(point.x, point.y, size.width, size.height);
	if (roi.x + roi.width >= frame.cols)
		roi.x -= ((roi.x + roi.width) - frame.cols);
	if (roi.y + roi.height >= frame.rows)
		roi.y -= ((roi.y + roi.height) - frame.rows);

	cv::Mat imageRoi = frame(roi);
	cv::addWeighted(imageRoi, 1.0, image, 1.0, 0, imageRoi);
}

// draw a pretty hands
void FrameProcessor::drawGlowyHands(cv::Mat& frame, const std::vector<Hand> hands) {

	if (hands.empty())
		return;

	cv::Mat image = cv::Mat(frame);
	image = cv::Mat::zeros(frame.size(), CV_8U);
	cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	for (Hand hand : hands) {
		if (!hand.Parameters.handContour.empty()) {
			std::vector<std::vector<cv::Point> > contour;
			contour.push_back(hand.Parameters.handContour);
			cv::drawContours(image, contour, 0, FP_COLOR_GREEN, -1);
		}
	}

	//bwMorph(image, cv::MORPH_DILATE, cv::MORPH_ELLIPSE, 5);

	cv::blur(image, image, cv::Size(randomNumber(20, 35), randomNumber(30, 45)));

	for (Hand hand : hands) {
		if (!hand.Parameters.handContour.empty()) {
			std::vector<std::vector<cv::Point> > contour;
			contour.push_back(hand.Parameters.handContour);
			cv::drawContours(image, contour, 0, FP_COLOR_BLACK, -1);
		}
	}

	//cv::Mat imageRoi = frame(cv::Rect(0, 0, frame.cols, frame.rows));
	cv::addWeighted(frame, 1.0, image, 0.55, 0, frame);
}

void FrameProcessor::drawGlowyLines(cv::Mat& frame, const std::vector<Hand> hands) {
	if (hands.empty()) {
		return;
	}

	int N = 4;
	int thickness = 7;

	cv::Mat image = cv::Mat(frame);
	image = cv::Mat::zeros(frame.size(), CV_8U);
	cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	for (Hand hand : hands) {
		if (!hand.Tracker.kalmTrack.empty()) {
			if (hand.Tracker.kalmTrack.size() > N) {
				for (int i = N - 1; i < hand.Tracker.kalmTrack.size() - 1; i++) {
					cv::line(image, hand.Tracker.kalmTrack[i], hand.Tracker.kalmTrack[i + 1], FP_COLOR_PURPLE, randomNumber(thickness - 2, thickness + 3));
				}
			}
		}
	}

	cv::blur(image, image, cv::Size(randomNumber(15, 20), randomNumber(15, 20)));

	for (Hand hand : hands) {
		if (!hand.Tracker.kalmTrack.empty()) {
			if (hand.Tracker.kalmTrack.size() > N) {
				for (int i = N - 1; i < hand.Tracker.kalmTrack.size() - 1; i++) {
					cv::line(image, hand.Tracker.kalmTrack[i], hand.Tracker.kalmTrack[i + 1], FP_COLOR_WHITE, randomNumber(thickness - 1, thickness + 1));
				}
			}
		}
	}

	cv::addWeighted(frame, 1.0, image, 1.0, 0, frame);
}

void FrameProcessor::bwMorph(cv::Mat& inputImage, const int operation, const int mShape, const int mSize) {
	cv::Mat element = cv::getStructuringElement(mShape, cv::Size(2 * mSize + 1, 2 * mSize + 1),
		cv::Point(mSize, mSize));
	cv::morphologyEx(inputImage, inputImage, operation, element);
}

int FrameProcessor::randomNumber(int limi, int lims)
{
	return (rand() % (lims - limi)) + limi;
}