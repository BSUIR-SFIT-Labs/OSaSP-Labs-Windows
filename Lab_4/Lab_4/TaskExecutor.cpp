#include <thread>
#include "TaskExecutor.h"

using namespace std;

TaskExecutor::TaskExecutor(TaskQueue queue)
{
	m_queue = queue;
}

void threadFunc(TaskQueue queue, int count)
{
	vector<thread> threads;

	while (count)
	{
		TTask task = queue.pop();

		if (task)
		{
			thread thr(task);
			threads.push_back(move(thr));

			count--;
		}
	}

	for (size_t i = 0; i < threads.size(); i++)
		threads[i].join();
}

void TaskExecutor::startExecution(int maxThreadCount)
{
	thread thr(threadFunc, m_queue, maxThreadCount);

	thr.join();
}