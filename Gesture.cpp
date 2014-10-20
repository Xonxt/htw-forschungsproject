#include "stdafx.h"
#include "Gesture.h"

Gesture::Gesture() {
	varDirection = Variable(10, 5, 6, "var:Direction");
	varSpeed = Variable(4, 3, 2, "var:Speed");
	varFingers = Variable(5, 6, 3, "var:Fingers");
	varAngle = Variable(3, 3, 3, "var:Angle");
}

Gesture::~Gesture() {
}

// get the gesture type as an enum
HandGesture Gesture::getGestureType() {
/*
	if (varSpeed.getValue() == SPEED_NONE) {
		gestureType = (HandGesture)varFingers.getValue();

		if (varFingers.getValue() == 2) {
			if (varAngle.getValue() == ANGLE_CLOSE) {
				gestureType = GESTURE_POSTURE_V;
			}
		}
	}
	else {
		int val = (varDirection.getValue() + 9);
		gestureType = (val <= 12) ? (HandGesture)val : GESTURE_NONE;
	}
*/

	if (varSpeed.getValue() == SPEED_NONE) { 
		switch (varFingers.getValue()) {
		case FINGERS_ZERO:
			gestureType = GESTURE_POSTURE_FIST;
			break;
		case FINGERS_ONE:
			gestureType = GESTURE_POSTURE_ONE;
			break;
		case FINGERS_TWO:
			if (varAngle.getValue() == ANGLE_CLOSE)
				gestureType = GESTURE_POSTURE_V;
			else
				gestureType = GESTURE_POSTURE_TWO;
			break;
		case FINGERS_THREE:
			gestureType = GESTURE_POSTURE_THREE;
			break;
		case FINGERS_FOUR:
			gestureType = GESTURE_POSTURE_FOUR;
			break;
		case FINGERS_FIVE:
			gestureType = GESTURE_POSTURE_FIVE;
			break;
		default:
			gestureType = GESTURE_NONE;
		}
	} // end-speed_none
	else {
		switch (varDirection.getValue()) {
		case MOVEMENT_UP:
			gestureType = GESTURE_SWIPE_UP;
			break;
		case MOVEMENT_DOWN:
			gestureType = GESTURE_SWIPE_DOWN;
			break;
		case MOVEMENT_LEFT:
			gestureType = GESTURE_SWIPE_LEFT;
			break;
		case MOVEMENT_RIGHT:
			gestureType = GESTURE_SWIPE_RIGHT;
			break;
		default:
			gestureType = GESTURE_NONE;
		}
	}

	return gestureType;
}

// get the gesture as a text
const char* Gesture::getGestureName() {
	if (gestureType != GESTURE_NONE) {
		//return GestureNames[gestureType];
		return GestureNames[gestureType].c_str();
	}
	else
		return "No gesture";		
}
