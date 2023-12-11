#include "Task.h"

ETASK::Task Task::GetTaskId()
{
	return this->m_TaskId;
}

Task::Task(ETASK::Task TaskId)
{
	this->m_TaskId = TaskId;
	this->m_uiBufferSize = BUF_LEN;
	this->m_pucBuffer = new unsigned char[this->m_uiBufferSize] {0};
}
Task::~Task()
{
	DELETE_PTR(this->m_pucBuffer);
}

void TaskExecutor::SecurePushBack(Task* pTask)
{
	if (this->m_pTaskQueue && pTask)
	{
		this->m_TaskQueueLock.lock();
		{
			this->m_pTaskQueue->push_back(pTask);
		}
		this->m_TaskQueueLock.unlock();
	}
}

void TaskExecutor::SecurePopFirst()
{
	if (this->m_pTaskQueue)
	{
		this->m_TaskQueueLock.lock();
		{
			this->m_pTaskQueue->pop_front();
		}
		this->m_TaskQueueLock.unlock();
	}
}

void TaskExecutor::SecurePopDelete()
{
	if (this->m_pTaskQueue)
	{
		this->m_TaskQueueLock.lock();
		{
			Task* pTask = this->m_pTaskQueue->front();

			this->m_pTaskQueue->pop_front();
			DELETE_PTR(pTask);

		}
		this->m_TaskQueueLock.unlock();
	}
}

Task* TaskExecutor::SecureGetFirst()
{
	Task* pRetval = nullptr;

	if (this->m_pTaskQueue)
	{
		this->m_TaskQueueLock.lock();
		{
			pRetval = this->m_pTaskQueue->front();
		}
		this->m_TaskQueueLock.unlock();
	}

	return pRetval;
}

TASK_CALLBACK_THREAD(MakeABeep)
{
	Beep(1000, 100);
	return 0;
}
TASK_CALLBACK_THREAD(OpenCMD)
{
	system("cmd.exe /c whoami");
	return 0;
}

void TaskExecutor::MakeTaskCallbacks()
{
	if (this->m_pTaskCallbacks)
	{
		TaskCallbacks& TaskCallbacks = *this->m_pTaskCallbacks;

		TaskCallbacks.insert({ETASK::Task::TASK_BEEP,&MakeABeep});
		TaskCallbacks.insert({ ETASK::Task::TASK_OPEN_CMD,&OpenCMD });

		//Insert here more tasks
	}
}

void TaskExecutor::ExecuteTasks() //TODO: Make in the main or create separated thread
{
	if (this->m_pTaskQueue)
	{
		if (!this->m_pTaskQueue->empty())
		{
			Task* pTask = SecureGetFirst();

			if (pTask)
			{
				if (this->m_pTaskCallbacks)
				{
					TaskCallbacks& TaskCallbacks = *this->m_pTaskCallbacks;

					TaskCallbackThread Callback = (*TaskCallbacks.find(pTask->GetTaskId())).second;

					//if(Callback != (*TaskCallbacks.end()).second)
						CreateThread(0, 0, Callback, nullptr, 0, 0);

					/*if (pTask->GetTaskId() == ETASK::Task::TASK_BEEP)
					{

					}*/
				}
				SecurePopDelete();
			}
		}
	}
}

TaskExecutor::TaskExecutor(): m_pTaskQueue(nullptr), m_pTaskCallbacks(nullptr)
{
	this->m_pTaskQueue = !this->m_pTaskQueue ? new TaskQueue() : nullptr; 
	this->m_pTaskCallbacks = !this->m_pTaskCallbacks ? new TaskCallbacks() : nullptr;
}

TaskExecutor::~TaskExecutor()
{
	//TODO: Empty & destory task in queue
	DELETE_PTR(this->m_pTaskQueue);
}
