#include "Slave.h"

void Slave::EmptyTaskQueue()
{
	this->m_TaskQueueMutex.lock();

	{
		if (this->m_pTaskQueue)
		{
			if (!this->m_pTaskQueue->empty())
			{
				for (TaskQueue::iterator it = this->m_pTaskQueue->begin();
					it != this->m_pTaskQueue->end();)
				{
					delete (*it);
					it = this->m_pTaskQueue->erase(it);
				}
			}
		}
	}

	this->m_TaskQueueMutex.unlock();
}

void Slave::PushTask(Task* pTask) //push_back
{
	if (this->m_pTaskQueue)
	{
		this->m_TaskQueueMutex.lock();

		{
			this->m_pTaskQueue->push_back(pTask);
		}

		this->m_TaskQueueMutex.unlock();
	}
}

Task* Slave::PopTask()
{
	if (this->m_pTaskQueue)
	{
		Task* pTask = nullptr;
		this->m_TaskQueueMutex.lock();

		{
			pTask = this->m_pTaskQueue->front();
			this->m_pTaskQueue->pop_front();
		}

		this->m_TaskQueueMutex.unlock();

		return pTask;
	}

	return nullptr;
}



Slave::Slave(unsigned long ulSlaveId)
{
	this->m_ulSlaveId = ulSlaveId;
	//this->m_pTaskQueue = new TaskQueue();
	//this->m_pSlaveDispatcher = nullptr;

}
Slave::~Slave()
{
	//this->EmptyTaskQueue();
	//DELETE_PTR(this->m_pTaskQueue);
	//DELETE_PTR(this->m_pSlaveDispatcher);

}

/// <summary>
/// Handle Dispatcher
/// </summary>

void Slave::CreateDispatcher()
{
	if (!this->m_pSlaveDispatcher)
		this->m_pSlaveDispatcher = new SlaveDispatcher();
}

void Slave::DestroyDispatcher()
{
	DELETE_PTR(this->m_pSlaveDispatcher);
}

/// <summary>
/// Handle Task Queue
/// </summary>

void Slave::CreateTaskQueue()
{
	if (!this->m_pTaskQueue)
		this->m_pTaskQueue = new TaskQueue();
}

void Slave::DestoyTaskQueue()
{
	this->EmptyTaskQueue();
	DELETE_PTR(this->m_pTaskQueue);
}


