// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

#include "stdafx.h"
#include "GestureAnalyzer.h"

GestureAnalyzer::GestureAnalyzer() {

}

GestureAnalyzer::GestureAnalyzer(const cv::Size _frameSize) {
	FingerParameters.cosThreshold = 0.5;
	FingerParameters.equalThreshold = 1e-7;
	FingerParameters.r = 40; // 40;
	FingerParameters.step = 16; // 16;

	frameSize = _frameSize;

	geometricRecognizer.loadTemplates();
}


GestureAnalyzer::~GestureAnalyzer() {
}

void GestureAnalyzer::analyzeHand(Hand& hand) {

	HandGesture resultGesture = GESTURE_NONE;
	hand.handGesture.newGesture = false;

	// hand.handGesture.setGestureType(GESTURE_NONE);

	// CALCULATE VARIANCE TO DETERMINE IF HAND STATIONARY OR MOVING
	int size = (int)hand.Tracker.kalmTrack.size();
	int N = 4;
	if (size >= N) {

		// a vector of N last points of the track line
		std::vector<cv::Point2f> data;

		// normalize the point coordinates and put them into the vector
		for (int i = 1; i <= N; i++) {
			cv::Point a = hand.Tracker.kalmTrack[size - i];
			cv::Point2f af = cv::Point2f((float)a.x / (float)frameSize.width, (float)a.y / (float)frameSize.height);
			data.push_back(af);
		}

		double mx = 0, my = 0, dx = 0, dy = 0;

		// calculate mean:
		for (cv::Point2f pt : data) {
			mx += pt.x;
			my += pt.y;
		}
		mx /= N; my /= N;

		// calculate variance
		for (cv::Point2f pt : data) {
			dx += (pt.x - mx) * (pt.x - mx);
			dy += (pt.y - mx) * (pt.y - my);
		}
		dx = sqrt(dx / N); dy = sqrt(dy / N);

		// Check if the variance is small to negligible (default: 0-0.005)
		if (dx < 0.0075 && dy < 0.0075) {
			// Hand is stationary!

			cv::Point currPosition = cv::Point(hand.handBox.center.x, hand.handBox.center.y);

			// if track length is atl east N (default: 20)
			// then we check the "air drawing" shape
			DollarRecognizer::RecognitionResult result = geometricRecognizer.recognize(hand.Tracker.kalmTrack);
			float resultThreshold = 0.75;
			if (hand.Tracker.kalmTrack.size() >= 20) {
				if (result.score >= resultThreshold) {
					//hand.handGesture.gestureName = result.name;
					//hand.handGesture.setGestureType(result.gestureType);
					resultGesture = result.gestureType;
				}
				else {
					//hand.handGesture.gestureName = " ";
					//hand.handGesture.setGestureType(GESTURE_NONE);
				}
				std::cout << "shape found: " << result.name << ", with score: " << result.score << std::endl;
			}

			// if track length is less than N, or if the resulting shape score is less than X (default: 0.75)
			// or if no match found, then it's probably just a swipe
			if (result.score < resultThreshold || hand.Tracker.kalmTrack.size() < 20 || result.gestureType == GESTURE_NONE) {
				// also, the previous stationary position must be valid, i.e. not (-1; -1)
				if ((hand.prevPosition.x + hand.prevPosition.y) > 0) {

					// also, if the distance between points is bigger than the width of the hand region:
					if (getDistance(hand.prevPosition, currPosition) >= (hand.detectionBox.width)) {
						// calculate swipe direction:
						double angle = getAngle(hand.prevPosition, currPosition);

						// determine the angle:
						if (isInRange(angle, NE + 1, NW - 1)) {
							//hand.handGesture.setGestureType(GESTURE_SWIPE_UP);
							resultGesture = GESTURE_SWIPE_UP;
						}
						else if (isInRange(angle, NW + 1, SW - 1)) {
							//hand.handGesture.setGestureType(GESTURE_SWIPE_RIGHT);
							resultGesture = GESTURE_SWIPE_RIGHT;
						}
						else if (isInRange(angle, SW + 1, SE - 1)) {
							//hand.handGesture.setGestureType(GESTURE_SWIPE_DOWN);
							resultGesture = GESTURE_SWIPE_DOWN;
						}
						else if (isInRange(angle, 0, SE + 1) || isInRange(angle, 0, NE - 1)) {
							//hand.handGesture.setGestureType(GESTURE_SWIPE_LEFT);
							resultGesture = GESTURE_SWIPE_LEFT;
						}
						else {
							//hand.handGesture.setGestureType(GESTURE_NONE);
						}						
					}
					else { // if the distance is smaller that the hand itself, assume it didn't move
						//hand.handGesture.setGestureType(GESTURE_NONE);
					}
				}
			}
            
            // if the hand didn't move, then we process the fingers:
            //if (hand.handGesture.getGestureType() != GESTURE_NONE) {
            if (resultGesture == GESTURE_NONE) {
                // process the contour and extract fingers
                extractFingers(hand);
                
                // add amount of fingers
                switch ((int)hand.Parameters.fingers.size()) {
                    case 0:
                        hand.handGesture.varFingers.addValue(FINGERS_ZERO);
                        break;
                    case 1:
                        hand.handGesture.varFingers.addValue(FINGERS_ONE);
                        break;
                    case 2:
                        hand.handGesture.varFingers.addValue(FINGERS_TWO);
                        break;
                    case 3:
                        hand.handGesture.varFingers.addValue(FINGERS_THREE);
                        break;
                    case 4:
                        hand.handGesture.varFingers.addValue(FINGERS_FOUR);
                        break;
                    default:
                        hand.handGesture.varFingers.addValue(FINGERS_FIVE);
                }
                
                // set finger angles:
                if (hand.Parameters.fingers.size() == 2) {
                    if (hand.Parameters.fingers[0].Angles.angle2next <= 30) {
                        hand.handGesture.varAngle.addValue(ANGLE_CLOSE);
                    }
                    else {
                        hand.handGesture.varAngle.addValue(ANGLE_FAR);
                    }
                }
                else
                    hand.handGesture.varAngle.addValue(ANGLE_NONE);
                
                // get the finger-amount-posture
                //hand.handGesture.setGestureType(hand.handGesture.getPosture());
                resultGesture = hand.handGesture.getPosture();
            }

			hand.Tracker.kalmTrack.clear();

			hand.prevPosition = currPosition;
		}

	}
    
	if (hand.handGesture.getGestureType() != resultGesture && resultGesture != GESTURE_NONE) {
		hand.handGesture.setGestureType(resultGesture);
		hand.handGesture.newGesture = true;
		std::cout << "Gesture detected: " << GestureNames[resultGesture] << std::endl;;
	}
}

// Determine if two floating point values are ~equal, with a threshold
bool GestureAnalyzer::isEqual(const double a, const double b) {
	return fabs(a - b) <= FingerParameters.equalThreshold;
}

// get the angle between two points in a contour
double GestureAnalyzer::getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = (int)contour.size();
	cv::Point p0 = (pt > 0) ? contour[pt % size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt > r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;
	return (ux * vx + uy * vy) / sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
}

// get the rotation between two points of the contour	
signed int GestureAnalyzer::getRotation(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = (int)contour.size();
	cv::Point p0 = (pt > 0) ? contour[pt % size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt > r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;

	signed int res = (ux * vy - vx * uy);

	return res;
}

// extract the fingertips by processing the contour
int GestureAnalyzer::extractFingers(Hand& hand) {
	int countFingers = 0;
	hand.Parameters.fingers.clear();

	if (hand.Parameters.handContour.size() <= 0)
		return -1;

	for (int j = 0; j < hand.Parameters.handContour.size(); j += FingerParameters.step) {
		double cos0 = getPointsAangle(hand.Parameters.handContour, j, FingerParameters.r);

		if ((cos0 > 0.5) && (j + FingerParameters.step < hand.Parameters.handContour.size())) {
			double cos1 = getPointsAangle(hand.Parameters.handContour, j - FingerParameters.step, FingerParameters.r);
			double cos2 = getPointsAangle(hand.Parameters.handContour, j + FingerParameters.step, FingerParameters.r);
			double maxCos = std::max(std::max(cos0, cos1), cos2);
			bool equal = isEqual(maxCos, cos0);
			signed int z = getRotation(hand.Parameters.handContour, j, FingerParameters.r);

			if (equal == 1 && z < 0) {
				// < init finger >
				Finger finger;
				finger.coordinates = hand.Parameters.handContour[j];
				finger.Angles.orientationAngle = getAngle(hand.handBox.center, finger.coordinates);
				finger.length = getDistance(finger.coordinates, hand.handBox.center);
				// </ init finger>

				if (hand.Parameters.fingers.size() > 1) {
					hand.Parameters.fingers[hand.Parameters.fingers.size() - 1].Angles.angle2next =
						fabs(hand.Parameters.fingers[hand.Parameters.fingers.size() - 1].Angles.orientationAngle - finger.Angles.orientationAngle);
				}

				hand.Parameters.fingers.push_back(finger);

				if (++countFingers >= 5)
					break;
			}
		}
	}
	return countFingers;
}

// get the angle of a vector, from pt1 to pt2
double GestureAnalyzer::getAngle(const cv::Point pt1, const cv::Point pt2) {
	double alpha; // result

	// calculate vector coordinates
	double	y = pt2.y - pt1.y,
		x = pt2.x - pt1.x;

	// hypotenuse
	double sqrtResult = 0;
	sqrtResult = sqrt(x * x + y * y);
	double angle;
	angle = asin(abs(x) / sqrtResult);

	alpha = angle * 180 / PI; // angle from North in degrees

	// correction
	if (x > 0) {
		// I or IV quadrant
		if (y < 0) {
			// IV quadrant
			alpha = 180 - alpha;
		}
	}
	else {
		// II or III quadrant
		if (y > 0) {
			// II quadrant
			alpha = -alpha;
		}
		else
			alpha = alpha - 180;
	}

	return correctAngle(alpha);
}

// correct the angle to a proper Descartes (0..360) orientation
double GestureAnalyzer::correctAngle(double alpha) {
	double angle = alpha;

	if (isInRange(alpha, 90, 180)) {		// 1st quadrant
		angle -= 90;
	}
	else if (isInRange(alpha, -90, -180)) {	// 2nd quadrant
		angle = (180 - abs(alpha)) + 90;
	}
	else if (isInRange(alpha, -0, -90)) {	// 3rd quadrant
		angle = 180 + (90 - abs(alpha));
	}
	else if (isInRange(alpha, 0, 90)) {		// 4th quadrant
		angle = 270 + abs(alpha);
		if (angle >= 360)
			angle = 0;
	}

	return (angle);
}

// check if the a value lies between two constrains
bool GestureAnalyzer::isInRange(const double value, const double A, const double B) {
	if (value >= fmin(A, B) && value <= fmax(A, B))
		return true;
	else
		return false;
}


// get the distance between two cv::Point points
double GestureAnalyzer::getDistance(const cv::Point pt1, const cv::Point pt2) {
	return (sqrt(pow(pt1.x - pt2.x, 2.0f) + pow(pt1.y - pt2.y, 2.0f)));
}