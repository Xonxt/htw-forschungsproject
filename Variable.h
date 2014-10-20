#pragma once
#include "stdafx.h"

class Variable {
public:
	Variable();
	Variable(const int _histSize, const int _valSize, const int _threshold, const char* _varName);

	// add new value
	void addValue(const int value);

	// return the index of the value. 
	int getValue();

	// return the variable's name as text
	const char* getVarName();

private:
	// the maximum amount of variables, that are saved
	int histSize;

	// the amount of values, assumed by the variable
	int valSize;

	// the threshold
	int varThreshold;

	// the text name of the variable
	std::string varName;

	// the vector of saved value history
	std::vector<int> historyVector;

	// the vector of values 
	std::vector<int> scores;

	// void clear scores
	void clearScores();

	// reassign the scores
	void assignScores();
};

