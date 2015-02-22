#pragma once

#include "stdafx.h"
#include "Variable.h"

class Gesture {
public:
	Gesture();
	~Gesture();

	// get the gesture type as an enum
	HandGesture getGestureType();

	// get the gesture as a text
	std::string getGestureName();

	// set gesture type
	void setGestureType(HandGesture gesture);

	// get posture gesture
	HandGesture getPosture();

	// gesture name
	std::string gestureName;

	// variables
	Variable varFingers;
	Variable varAngle;

	// did the gesture change?
	bool newGesture;

private:
	HandGesture gestureType;
};

