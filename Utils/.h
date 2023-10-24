#pragma once
#include <deque>
#include <mutex>

#define DELETE(ptr) if(ptr){decltype(ptr) tmptr = ptr; ptr = nullptr; delete tmptr;}

class Task {
private:
	unsigned long m_ulTaskId;
	unsigned int m_uiBufferSize;
	unsigned char* m_pucBuffer;

public:
	unsigned long GetTaskId();

	Task(unsigned long m_ulTaskId);
	~Task();


};
typedef std::deque<Task*> TaskQueue;
