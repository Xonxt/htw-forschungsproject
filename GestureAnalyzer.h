#pragma once
#include "Hand.h"

class GestureAnalyzer {
public:
	GestureAnalyzer();
	~GestureAnalyzer();

	// process a hand, fill in its parameters
	void processHand(Hand& hand);
};

