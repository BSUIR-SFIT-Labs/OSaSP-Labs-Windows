#pragma once

#include <queue>
#include <functional>

using namespace std;

typedef function<void()> TTask;

class TaskQueue
{
public:
	void push(TTask task);
	TTask pop();
private:
	queue<TTask>* tasksQueue = new queue<TTask>();
};