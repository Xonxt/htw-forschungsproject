#include "stdafx.h"
#include "Gesture.h"

Gesture::Gesture() {
	varFingers = Variable(3, 6, 2, "var:Fingers");
	varAngle = Variable(2, 3, 2, "var:Angle");

	newGesture = false;
}

Gesture::~Gesture() {
}

HandGesture Gesture::getPosture() {
	HandGesture returnType = GESTURE_NONE;

	switch (varFingers.getValue()) {
	case FINGERS_ZERO:
		returnType = GESTURE_POSTURE_FIST;
		break;
	case FINGERS_ONE:
		returnType = GESTURE_POSTURE_ONE;
		break;
	case FINGERS_TWO:
		//if (varAngle.getValue() == ANGLE_CLOSE)
		//	returnType = GESTURE_POSTURE_V;
		//else
			returnType = GESTURE_POSTURE_TWO;
		break;
	case FINGERS_THREE:
		returnType = GESTURE_POSTURE_THREE;
		break;
	case FINGERS_FOUR:
		returnType = GESTURE_POSTURE_FOUR;
		break;
	case FINGERS_FIVE:
		returnType = GESTURE_POSTURE_FIVE;
		break;
	default:
		returnType = GESTURE_NONE;
	}

	return returnType;
}

// get the gesture type as an enum
HandGesture Gesture::getGestureType() {
	/*
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
	}*/

	return gestureType;
}


// set gesture type
void Gesture::setGestureType(HandGesture gesture) {
	gestureType = gesture;
}

// get the gesture as a text
std::string Gesture::getGestureName() {
	if (gestureType != GESTURE_NONE) {
		//return GestureNames[gestureType];
		return GestureNames[gestureType];
	}
	else
		return "No gesture";
}
