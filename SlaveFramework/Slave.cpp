#include "Slave.h"

unsigned long Slave::GetSlaveId()
{
	return this->m_ulSlaveId;
}

Slave::Slave(unsigned long ulSlaveId) :
	m_pDispatcher(nullptr)
{
	this->m_ulSlaveId = ulSlaveId != ~0ul ? ulSlaveId : ~0ul;
	this->m_pTaskExectutor = new TaskExecutor();
}
Slave::~Slave()
{
	//TODO: Slave dtor
	//this->EmptyTaskQueue();
	DELETE_PTR(this->m_pTaskExectutor); //call TaskQueue dtor
	DELETE_PTR(this->m_pDispatcher);

}

/// <summary>
/// Handle Dispatcher
/// </summary>



void Slave::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		this->m_pDispatcher = new SlaveDispatcher();
}

void Slave::DestroyDispatcher()
{
	DELETE_PTR(this->m_pDispatcher);
}

SlaveDispatcher* Slave::GetDispatcher()
{
	return this->m_pDispatcher;
}

void Slave::CreateTaskExecutor()
{
	if (!this->m_pTaskExectutor)
		this->m_pTaskExectutor = new TaskExecutor();
}

void Slave::DestroyTaskExecutor()
{
	DELETE_PTR(this->m_pTaskExectutor);
}

TaskExecutor* Slave::GetTaskExecutor()
{
	return this->m_pTaskExectutor;
}


