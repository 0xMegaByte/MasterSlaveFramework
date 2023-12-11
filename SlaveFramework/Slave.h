#pragma once

#include "Task.h"
#include "SlaveDispatcher.h"

#pragma comment(lib,"Utils.lib")
class Slave
{
private:

	unsigned long m_ulSlaveId;

	TaskExecutor* m_pTaskExectutor;
	SlaveDispatcher* m_pDispatcher;

public:
	Slave(unsigned long ulSlaveId);
	~Slave();

	//Handle Dispatcher
	void CreateDispatcher();
	void DestroyDispatcher();
	SlaveDispatcher* GetDispatcher();

	//Handle TaskExecutor
	void CreateTaskExecutor();
	void DestroyTaskExecutor();
	TaskExecutor* GetTaskExecutor();

	//Utils
	unsigned long GetSlaveId();
};