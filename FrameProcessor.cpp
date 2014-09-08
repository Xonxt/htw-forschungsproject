#include "stdafx.h"
#include "FrameProcessor.h"

FrameProcessor::FrameProcessor()
{
	//isWebCam = webCam;
	hands.clear();
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
	showBoundingBox = true;

	// clear the hands list
	hands.clear();

	if (!(result &= handDetector.initialize(isWebCam))) {
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
			// if the area of intersection is 75% of the hand's size or more
			cv::Rect intersection = tempHand.handBox.boundingRect() & (*it2).handBox.boundingRect();
			if (intersection.area() >= (tempHand.handBox.boundingRect().area() * 0.75)) {
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

	// remove intersecting regions (if two hands located in one position)
	if (hands.size() > 1) {
		int N = hands.size();
		for (int i = 0; i < N - 1 && N > 1; i++) {
			cv::Rect firstHand = hands[i].handBox.boundingRect();
			for (int j = i + 1; j < N && N > 1; j++) {
				cv::Rect secondHand = hands[j].handBox.boundingRect();
				// intersection
				cv::Rect intersection = firstHand & secondHand;

				// take the smallest hand
				cv::Rect smallestHand = (firstHand.area() < secondHand.area()) ? firstHand : secondHand;

				// compare intersection sizes
				if (intersection.area() > 0 && hands.size() > 1) {
					// remove the largest region
					if ((firstHand.area() < secondHand.area())) {
						hands.erase(hands.begin() + i--);
					}
					else {
						hands.erase(hands.begin() + j--);
					}
					N--;
				}
				
				if (N < 2 || i == N || j == N) {
					break;
				}
			}
		}
	}
	/*
	// the same thing but with iterators. Gives an exception in Visual Studio :(
	// remove heavily intersecting regions
    if (hands.size() > 1) {
        for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end()-1; ++it) {
            cv::Rect firstHand = (*it).handBox.boundingRect();

            // iterate once again:
            for (std::vector<Hand>::iterator it2 = (it + 1); it2 != hands.end() && hands.size() > 1; ++it2) {
                cv::Rect secondHand = (*it2).handBox.boundingRect();

                // intersection
                cv::Rect intersection = firstHand & secondHand;

                // take the smallest hand
                cv::Rect smallestHand = (firstHand.area() < secondHand.area()) ? firstHand : secondHand;

                // compare intersection sizes
                if (intersection.area() > 0 && hands.size() > 1) {                    
                    // remove the largest one
                    if ((firstHand.area() > secondHand.area()))
                        it = hands.erase(it);
                    else
                        it2 = hands.erase(it2);
                    
                    if (it2 == hands.end() || it == hands.end())
                        break;
                }
            }
            if (it == hands.end() || hands.size() < 2)
                break;
        }
    }
    */
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
        
        if (remove)
            it = hands.erase(it);
        
        if (it == hands.end())
            break;
    }
	
	// now check if new hands were added and then delete face regions
	if (newHandsAdded && hands.size() > 0) {
		std::vector<cv::Rect> faces;

		faces.clear();

		// iterate through pedestrians:
		for (std::vector<cv::Rect>::iterator pd = pedestrians.begin(); pd != pedestrians.end(); ++pd) {
			// cut out a part of the image
			cv::Mat cutOut = cv::Mat(frame, *pd);

			// prepare a temporary storage for faces
			std::vector<cv::Rect> tempFaces;

			// look for faces in the cutout
			faceCascade.detectMultiScale(cutOut, tempFaces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT);

			// append the located faces to the Faces vector
			for (std::vector<cv::Rect>::iterator fc2 = tempFaces.begin(); fc2 != tempFaces.end(); ++fc2) {
				cv::Rect face = *fc2;

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

						if (it == hands.end())
							break;
					}
				}
				if (it == hands.end())
					break;
			}
		}
	}
	
	// replace the frame with a mask if needed
	if (showMask) {
		handTracker.getSkinMask(frame);
	}

	// draw the AR-things on the frame
    try {
        drawFrame(frame);
    } catch (cv::Exception ex) {
        std::cout << "OpenCV Exception caught: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cout << "Some kind of exception caught!" << std::endl;
    }
}

// draw everything on frame (hands, fingers, etc.)
void FrameProcessor::drawFrame(cv::Mat& frame) {
	// iterate through our vector of hands
    int clr = 0;

	for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
		// put a rectangle|ellepse on the image
		if (showBoundingBox) {
			cv::ellipse(frame, (*it).handBox, fpColors[clr], 2);
		}

		// show the contours
		if (showContour && (*it).Parameters.handContour.size() > 0) {
			cv::drawContours(frame, (*it).Parameters.handContour, 0, fpColors[clr]);
		}
        clr++;
	}

	// display text
	std::vector<std::string> strings;
	
	if (showMask)
		strings.push_back("showing mask");

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

	for (int i = 0; i < strings.size(); i++) {
        cv::putText(frame, strings[i], cv::Point(20, (i + 1) * 35), CV_FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 255), 3);
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

// toggle show hand contours
void FrameProcessor::toggleShowContour() {
	showContour = !showContour;
}

// toggle showing the bounding box
void FrameProcessor::toggleShowBoundingBox() {
	showBoundingBox = !showBoundingBox;
}