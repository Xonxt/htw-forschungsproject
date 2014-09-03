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

	// now remove heavily intersecting regions
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
                //if (intersection.area() >= (smallestHand.area() * 0.5)) {
                if (intersection.area() > 0 && hands.size() > 1) {
                    
                    // remove the largest one
                    if ((firstHand.area() > secondHand.area()))
                        it = hands.erase(it);
                    else
                        it2 = hands.erase(it2);
                    
                    if (it2 == hands.end() || it == hands.end())
                        break;
                    
                  //  hands.erase((firstHand.area() > secondHand.area()) ? it : it2);
                }
            }
            if (it == hands.end() || hands.size() < 2)
                break;
        }
    }
    
    // remove strange regions
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

		// look for faces in the frame
		faceCascade.detectMultiScale(frame, faces, 1.1, 2, CV_HAAR_FIND_BIGGEST_OBJECT);

		// iterate through hands
		for (std::vector<Hand>::iterator it = hands.begin(); it != hands.end(); ++it) {
			// iterate through faces:
			for (std::vector<cv::Rect>::iterator fc = faces.begin(); fc != faces.end(); ++fc) {
				cv::Rect intersection = (*it).handBox.boundingRect() & (*fc);

				// check if the intersection area too big
				//if (intersection.area() >= ((*fc).area() * 0.5)) {
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
		// draw track lines
		/*
		if ((*it).Tracker.kalmTrack.size() >= 2) {
			for (int i = 0; i < (*it).Tracker.kalmTrack.size() - 1; i++) {
				cv::line(frame, (*it).Tracker.kalmTrack[i], (*it).Tracker.kalmTrack[i + 1], fpColors[clr], 2);
			}
		}
		*/
		// put a rectangle|ellepse on the image
		cv::ellipse(frame, (*it).handBox, fpColors[clr], 2);
        //cv::rectangle(frame, (*it).roiRectange, fpColors[clr], 2);
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