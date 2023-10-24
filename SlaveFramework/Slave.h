#pragma once

#include <mutex>
#include "Task.h"
#include "SlaveDispatcher.h"

#pragma comment(lib,"Utils.lib")
class Slave
{
private:
	unsigned long m_ulSlaveId;
	TaskQueue* m_pTaskQueue;
	std::mutex m_TaskQueueMutex;

	SlaveDispatcher* m_pSlaveDispatcher;
public:
	Slave(unsigned long ulSlaveId);
	~Slave();

	//Handle Dispatcher
	void CreateDispatcher();
	void DestroyDispatcher();

	//Handle TaskQueue
	void CreateTaskQueue();
	void DestoyTaskQueue();

	void EmptyTaskQueue();
	void PushTask(Task* pTask); //Back
	Task* PopTask(); //Front
};

