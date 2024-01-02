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
#pragma once
#include "../Utils/pch.h"

typedef DWORD(__stdcall* TaskCallbackThread)(void*);
#define TASK_CALLBACK_THREAD( Name ) DWORD __stdcall Name(void* lpv)


class Task
{
private:
	EPACKET::Task m_TaskId;
	unsigned int m_uiBufferSize;	//Params len
	unsigned char* m_pucBuffer;		//Params data

public:
	EPACKET::Task GetTaskId();

	Task(EPACKET::Task m_TaskId); //TODO: Handle params
	~Task();
};

typedef std::deque<Task*> TaskQueue;
typedef std::unordered_map<EPACKET::Task, TaskCallbackThread> TaskCallbacks;

class TaskExecutor
{
private:
	TaskQueue* m_pTaskQueue;
	std::mutex m_TaskQueueLock;

	TaskCallbacks* m_pTaskCallbacks;
	std::mutex m_TaskCallbacksLock;

public:

	//Handle Task Queue
	void SecurePushBack(Task* pTask);
	void SecurePopFirst();
	void SecurePopDelete();
	Task* SecureGetFirst();
	void EmptyTaskQueue();

	//Handle Task Callback map

	void MakeTaskCallbacks();
	void ExecuteTasks();

	TaskExecutor();
	~TaskExecutor();
};