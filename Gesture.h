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

	// gesture name
	std::string gestureName;

	// variables
	Variable varDirection;
	Variable varSpeed;
	Variable varFingers;
	Variable varAngle;

private:
	HandGesture gestureType;
};

