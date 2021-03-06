// Forschungsproject (teil 3) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2015, Berlin

#pragma once

#include "stdafx.h"

class Finger {
public:
	Finger();

	// the coordinates of this fingertip
	cv::Point coordinates;

	// finger length - the distance from palm-center to fingertip
	double length;

	// is the finger deemed to be outstretched?
	bool outstretched;

	struct {
		// the angle from this finger to the next one
		double angle2next;

		// the overall orientation angle of the finger (from the palm center)
		double orientationAngle;
	} Angles;
};

