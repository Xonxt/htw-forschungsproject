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
	Variable<MovementDirection> varDirection;
	Variable<MovementSpeed> varSpeed;
	Variable<FingerCount> varFingers;
	Variable<FingersAngle> varAngle;

private:
	HandGesture gestureType;
};

