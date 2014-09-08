#include "stdafx.h"
#include "Finger.h"


Finger::Finger() {
	outstretched = false;
	length = -0.1;
	Angles.angle2next = -1;
	Angles.angle2prev = -1;
	Angles.orientationAngle = NAN;
}

