#include "TaskQueue.h"
#include <mutex>

using namespace std;

mutex lock_obj;

void TaskQueue::push(TTask task)
{
	lock_obj.lock();
	tasksQueue->push(task);
	lock_obj.unlock();
}

TTask TaskQueue::pop()
{
	TTask res;

	lock_obj.lock();

	if (tasksQueue->empty())
	{
		res = NULL;
	}
	else
	{
		res = tasksQueue->front();
		tasksQueue->pop();
	}

	lock_obj.unlock();

	return res;
}