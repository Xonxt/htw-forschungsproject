// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "Finger.h"


Finger::Finger() {
	outstretched = false;
	length = -0.1;
	Angles.angle2next = 0;
	Angles.orientationAngle = NAN;
}

