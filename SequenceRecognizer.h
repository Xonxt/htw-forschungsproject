#pragma once

#include <set>
#include <stack>
#include <algorithm>
#include "SequenceTypes.h"

class SequenceRecognizer
{
public:
	SequenceRecognizer();

	// recognize a sequence of gestures from a predefined set
	RecognizedSequence recognizeSequence(std::vector<HandGesture> sequence);

private:
	// a set of saved gesture sequences
	std::vector<GestureSequence> sequenceSet;

	// add sequence
	void addGestureSequence(GestureSequence sequence);

};