/*
Copyright (C) 2023 Matan Shitrit (0xMegaByte)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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

		TaskCallbacks.insert({ ETASK::Task::TASK_BEEP,&MakeABeep });
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
			//TODO: Fix locking
			Task* pTask = SecureGetFirst();

			if (pTask)
			{
				if (this->m_pTaskCallbacks)
				{
					TaskCallbacks& TaskCallbacks = *this->m_pTaskCallbacks;

					TaskCallbackThread Callback = (*TaskCallbacks.find(pTask->GetTaskId())).second;

					CreateThread(0, 0, Callback, nullptr, 0, 0);

				}
				SecurePopDelete();
			}
		}
	}
}

TaskExecutor::TaskExecutor() : m_pTaskQueue(nullptr), m_pTaskCallbacks(nullptr)
{
	this->m_pTaskQueue = !this->m_pTaskQueue ? new TaskQueue() : nullptr;
	this->m_pTaskCallbacks = !this->m_pTaskCallbacks ? new TaskCallbacks() : nullptr;
}

TaskExecutor::~TaskExecutor()
{
	//TODO: Empty & destory task in queue
	DELETE_PTR(this->m_pTaskQueue);
}
