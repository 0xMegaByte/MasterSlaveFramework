#pragma once
#include "..\Utils\Utils.h"

typedef DWORD(__stdcall* TaskCallbackThread)(void*);
#define TASK_CALLBACK_THREAD( Name ) DWORD __stdcall Name(void* lpv)

namespace ETASK
{
	enum class Task
	{
		TASK_BEEP = 2000,
		TASK_OPEN_CMD
	};
}

class Task {
private:
	ETASK::Task m_TaskId;		
	unsigned int m_uiBufferSize;	//Params len
	unsigned char* m_pucBuffer;		//Params data

public:
	ETASK::Task GetTaskId();

	Task(ETASK::Task m_TaskId);
	~Task();
};

typedef std::deque<Task*> TaskQueue;
typedef std::unordered_map<ETASK::Task, TaskCallbackThread> TaskCallbacks;

class TaskExecutor
{
private:
	TaskQueue* m_pTaskQueue;
	std::mutex m_TaskQueueLock;

	//Tasks-Callbacks list
	TaskCallbacks* m_pTaskCallbacks;
	std::mutex m_TaskCallbacksLock;

	//Handle running tasks

public:

	//Handle Task Queue
	void SecurePushBack(Task* pTask);
	void SecurePopFirst();
	void SecurePopDelete();
	Task* SecureGetFirst();

	//Handle Task Callback map
	
	void MakeTaskCallbacks();
	void ExecuteTasks();

	TaskExecutor();
	~TaskExecutor();
};