#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>

#include "TaskQueue.h"
#include "TaskExecutor.h"

using namespace std;

constexpr auto PATH_TO_FILE = "D:\\1.txt";
constexpr auto THREAD_COUNT = 10;

vector<string>* vectorOfStrings = new vector<string>();
vector<vector<string>*>* vectorOfParts = new vector<vector<string>*>();

bool getFileContent(string, vector<string>*);
void splitVectors(TaskQueue, int);
void addVector(vector<string>*, TaskQueue);
vector<string> mergeTwoVectors(vector<string>, vector<string>);
vector<string> mergeVectors(void);
void printVectors(vector<string>);

int main()
{
	bool isFileContentReceived = getFileContent(PATH_TO_FILE, vectorOfStrings);

	if (!isFileContentReceived)
	{
		cout << "File isn't exist" << endl;
		return -1;
	}

	TaskQueue taskQueue;
	TaskExecutor taskExecutor(taskQueue);

	int threadsCount = THREAD_COUNT > vectorOfStrings->size() 
		? vectorOfStrings->size() : THREAD_COUNT;

	splitVectors(taskQueue, threadsCount);
	taskExecutor.startExecution(threadsCount);

	vector<string> sortedVectors = mergeVectors();
	printVectors(sortedVectors);

	return 0;
}

bool getFileContent(string pathToFile, vector<string>* vectorOfStrings)
{
	ifstream file(pathToFile);
	string line;

	if (!file.good())
		return false;

	while (getline(file, line))
	{
		string new_line;
		new_line = line + "\n";

		if (new_line.size() != 0)
			vectorOfStrings->push_back(new_line);
	}

	return true;
}

void splitVectors(TaskQueue taskQueue, int threadCount)
{
	size_t onePartCount = (size_t)floor(((vectorOfStrings->size()) / threadCount));

	for (size_t i = 0; i < vectorOfStrings->size(); i += onePartCount)
	{
		vector<string>* newVector = new vector<string>();
		vectorOfParts->push_back(newVector);

		for (size_t j = i; j < i + onePartCount; j++)
		{
			if (j < vectorOfStrings->size())
			{
				string str = (*vectorOfStrings)[j];
				newVector->push_back(str);
			}
		}

		addVector(newVector, taskQueue);
	}
}

void addVector(vector<string>* vect, TaskQueue queue) 
{
	queue.push([vect]() {
		std::vector<std::string>* copy = vect;
		std::sort(vect->begin(), vect->end());
		});
}

vector<string> mergeTwoVectors(vector<string> firstVector, vector<string> secondVector)
{
	size_t firstVectorSize = firstVector.size();
	size_t secondVectorSize = secondVector.size();

	vector<string> resultVector;
	resultVector.reserve(firstVectorSize + secondVectorSize);

	size_t i = 0;
	size_t j = 0;

	while (i < firstVectorSize && j < secondVectorSize)
	{
		if (firstVector[i] <= secondVector[j])
			resultVector.push_back(firstVector[i++]);
		else
			resultVector.push_back(secondVector[j++]);
	}

	while (i < firstVectorSize)
		resultVector.push_back(firstVector[i++]);

	while (j < secondVectorSize)
		resultVector.push_back(secondVector[j++]);

	return resultVector;
}

vector<string> mergeVectors()
{
	vector<string> tmpVector;

	if (vectorOfParts->size() > 0)
		tmpVector = *(*vectorOfParts)[0];

	for (size_t i = 1; i < vectorOfParts->size(); i++)
		tmpVector = mergeTwoVectors(tmpVector, *(*vectorOfParts)[i]);

	return tmpVector;
}

void printVectors(vector<string> vector)
{
	for (size_t i = 0; i < vector.size(); i++)
		cout << vector[i] << endl;
}