#include "stdafx.h"
#include "Variable.h"

// default constructor
template <class Type>
Variable<Type>::Variable() {
	historyVector.clear();

	histSize = -1;
	valSize = -1;
	varName = "none";

	clearScores();
}

// overloaded constructor
template <class Type>
Variable<Type>::Variable(const int _histSize, const int _valSize, const int _threshold, const char* _varName) {
	historyVector.clear();

	histSize = _histSize;
	valSize = _valSize;
	varName = std::string(_varName);
	varThreshold = _threshold;

	clearScores();
}

// return the variable's name as text
template <class Type>
char* Variable<Type>::getVarName() {
	return varName.c_str();
}

// return the index of the value. 
template <class Type>
int Variable<Type>::getValue() {
	std::vector<int>::iterator maxi = std::max(scores.begin(), scores.end());

	if ((*maxi) >= varThreshold) {
		return (int)(maxi - scores.begin());
	}
	else {
		return -1;
	}
}

// add new value
template <class Type>
void Variable<Type>::addValue(const Type value) {
	historyVector.push_back(value);

	if (historyVector.size() > histSize) {
		historyVector.erase(historyVector.begin());
	}

	assignScores();
}

// void clear scores
template <class Type>
void Variable<Type>::clearScores() {
	scores.clear();
	scores = std::vector<int>(valSize);
	std::fill(scores.begin(), scores.end(), 0);
}

// reassign the scores
template <class Type>
void Variable<Type>::assignScores() {
	clearScores();

	for (Type val : historyVector) {
		scores[(int)val]++;
	}
}