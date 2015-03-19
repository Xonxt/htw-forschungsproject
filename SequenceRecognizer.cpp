#include "stdafx.h"
#include "SequenceRecognizer.h"
#include "SequenceTypes.h"


SequenceRecognizer::SequenceRecognizer()
{
	/*// add a few gesture sequences
	// left-down-two-three
    std::vector<HandGesture> temp;
    temp.clear();
    temp.push_back(GESTURE_SWIPE_LEFT);
    temp.push_back(GESTURE_SWIPE_DOWN);
    temp.push_back(GESTURE_POSTURE_TWO);
    temp.push_back(GESTURE_POSTURE_THREE);
    
	addGestureSequence(GestureSequence("L-D-2-3", temp));
	// one-two-three-circle
    temp.clear();
    temp.push_back(GESTURE_POSTURE_ONE);
    temp.push_back(GESTURE_POSTURE_TWO);
    temp.push_back(GESTURE_POSTURE_THREE);
    temp.push_back(GESTURE_DRAWING_CIRCLE);
	addGestureSequence(GestureSequence("1-2-3-circle", temp));
	// fist-up-right-rectangle
    temp.clear();
    temp.push_back(GESTURE_POSTURE_FIST);
    temp.push_back(GESTURE_SWIPE_UP);
    temp.push_back(GESTURE_SWIPE_RIGHT);
    temp.push_back(GESTURE_DRAWING_RECTANGLE);
	addGestureSequence(GestureSequence("0-U-R-rect", temp));
	// left-right
    temp.clear();
    temp.push_back(GESTURE_SWIPE_LEFT);
    temp.push_back(GESTURE_SWIPE_RIGHT);
	addGestureSequence(GestureSequence("left-right",temp));
    */
    // C++14
    // left-down-two-three
	addGestureSequence(GestureSequence("L-D-2-3",
    { GESTURE_SWIPE_LEFT, GESTURE_SWIPE_DOWN, GESTURE_POSTURE_TWO, GESTURE_POSTURE_THREE }));
	// one-two-three-circle
	addGestureSequence(GestureSequence("1-2-3-circle",
    { GESTURE_POSTURE_ONE, GESTURE_POSTURE_TWO, GESTURE_POSTURE_THREE, GESTURE_DRAWING_CIRCLE }));
	// fist-up-right-rectangle
	addGestureSequence(GestureSequence("0-U-R-rect",
    { GESTURE_POSTURE_FIST, GESTURE_SWIPE_UP, GESTURE_SWIPE_RIGHT, GESTURE_DRAWING_RECTANGLE }));
	// left-right
	addGestureSequence(GestureSequence("left-right",
    { GESTURE_SWIPE_LEFT, GESTURE_SWIPE_RIGHT }));
    
}

void SequenceRecognizer::addGestureSequence(GestureSequence sequence) {
	sequenceSet.push_back(sequence);
}

// recognize a sequence of gestures from a predefined set
RecognizedSequence SequenceRecognizer::recognizeSequence(std::vector<HandGesture> sequence) {

	// check if there are saved sequences
	if (sequenceSet.empty()) {
		return RecognizedSequence("none", 0);
	}

	// check if the argument not an emtpy sequence 
	if (sequence.empty()) {
		return RecognizedSequence("none", 0);
	}

	// first, find try to find a saved sequence, that is contained in the input sequence
	int idx = 0;
	bool found = false;

	for (GestureSequence seq : sequenceSet) {
		std::vector<HandGesture> tempSeq = seq.getSequence();
		// a set, representing the input sequence
		std::set<HandGesture> set1(sequence.begin(), sequence.end());
		// a set, representing one of the template sequences
		std::set<HandGesture> set2(tempSeq.begin(), tempSeq.end());

		// check if set is included
		if (std::includes(set1.begin(), set1.end(), set2.begin(), set2.end())) {
			found = true;
			break;
		}
		else {
			idx++;
		}
	}

	// if such a set is not found, then c'est la vie :(
	if (!found)
		return RecognizedSequence("none", 0);
	else {
		// vector of "1" and "0"
		// it will look like "11101010", with every "1" representing a matching gesture
		// and every "0" - a mismatch
		std::vector<bool> checks;

		// prepare the stack
		std::stack<HandGesture> temp;
		for (HandGesture g : sequenceSet[idx].getSequence())
			temp.push(g);

		// go through the input sequence:
		for (int i = sequence.size() - 1; i >= 0; i--) {
			if (temp.top() == sequence[i]) {
				temp.pop();
				checks.push_back(true);
			}
			else {
				checks.push_back(false);
			}
			// when the stack is empty, exit the loop
			if (temp.empty())
				break;
		}
		// count "1"s in the "1010101" vector
		int i = std::count_if(checks.begin(), checks.end(), [](bool i) {return (i == true); });

		// the score is the amount of "1"s dicided by the total amount of checks
		return RecognizedSequence(sequenceSet[idx].getName(), (i*1.0) / checks.size());
	}
}
