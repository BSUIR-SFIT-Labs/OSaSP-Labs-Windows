#pragma once

#include "TaskQueue.h"

class TaskExecutor
{
public:
	TaskExecutor(TaskQueue queue);
	void startExecution(int maxThreadCount);
private:
	TaskQueue m_queue;
};