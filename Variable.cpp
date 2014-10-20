#include "stdafx.h"
#include "Variable.h"


// default constructor
Variable::Variable() {
	historyVector.clear();

	histSize = -1;
	valSize = -1;
	varName = "none";

	//clearScores();
}

// overloaded constructor
Variable::Variable(const int _histSize, const int _valSize, const int _threshold, const char* _varName) {
	historyVector.clear();

	histSize = _histSize;
	valSize = _valSize;
	varName = std::string(_varName);
	varThreshold = _threshold;

	clearScores();
}

// return the variable's name as text
const char* Variable::getVarName() {
	return varName.c_str();
}

// return the index of the value. 
int Variable::getValue() {
	std::vector<int>::iterator maxi = std::max(scores.begin(), scores.end());

	if ((*maxi) >= varThreshold) {
		return (int)(maxi - scores.begin());
	}
	else {
		return -1;
	}
}

// add new value
void Variable::addValue(const int value) {
	historyVector.push_back(value);

	if (historyVector.size() > histSize) {
		historyVector.erase(historyVector.begin());
	}

	assignScores();
}

// void clear scores
void Variable::clearScores() {
	scores.clear();
	scores = std::vector<int>(valSize);
	std::fill(scores.begin(), scores.end(), 0);
}

// reassign the scores
void Variable::assignScores() {
	clearScores();

	for (int val : historyVector) {
		scores[(int)val]++;
	}
}
