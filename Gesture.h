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
	const char* getGestureName();

	// variabales
	Variable varDirection;
	Variable varSpeed;
	Variable varFingers;
	Variable varAngle;

private:
	HandGesture gestureType;
};

