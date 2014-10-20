#pragma once
template <class Type>
class Variable {
public:
	Variable();
	Variable(const int _histSize, const int _valSize, const int _threshold, const char* _varName);

	// add new value
	void addValue(const Type value);	

	// return the index of the value. 
	int getValue();

	// return the variable's name as text
	char* getVarName();

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
	std::vector<Type> historyVector;

	// the vector of values 
	std::vector<int> scores;

	// void clear scores
	void clearScores();

	// reassign the scores
	void assignScores();
};

