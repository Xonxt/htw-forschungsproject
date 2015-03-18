#pragma once
#include <vector>

class GestureSequence {

public:
	GestureSequence() { }

	GestureSequence(std::string name, std::vector<HandGesture> sequence) {
		setSequence(name, sequence);
	}

	void setSequence(std::string name, std::vector<HandGesture> sequence) {
		gestureSequence.clear();

		sequenceName = name;
		gestureSequence = sequence;
	}

	std::vector<HandGesture> getSequence() const {
		return gestureSequence;
	}

	std::string getName() const {
		return sequenceName;
	}

private:
	// the saved sequence of gestures
	std::vector<HandGesture> gestureSequence;

	std::string sequenceName;
};

class RecognizedSequence {
public:
	RecognizedSequence() :
		recognitionScore(0.0f),	sequenceName(" ") { };

	RecognizedSequence(std::string name, double score) :
		recognitionScore(score), sequenceName(name) { }

	double getScore() {
		return recognitionScore;
	}

	std::string getName() {
		return sequenceName;
	}

private:
	double recognitionScore;
	std::string sequenceName;
};