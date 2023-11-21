#include "Slave.h"

unsigned long Slave::GetSlaveId()
{
	return this->m_ulSlaveId;
}

Slave::Slave(unsigned long ulSlaveId) : 
	m_pDispatcher(nullptr), m_hReceiveThread(INVALID_HANDLE_VALUE),
	m_hSendThread(INVALID_HANDLE_VALUE)
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

DWORD WINAPI ReceiveThreadWrapper(LPVOID lpv)
{
	SlaveDispatcher* psd = static_cast<SlaveDispatcher*>(lpv);
	return psd->ReceiveThread(nullptr);
}

DWORD WINAPI SendThreadWrapper(LPVOID lpv)
{
	SlaveDispatcher* psd = static_cast<SlaveDispatcher*>(lpv);
	return psd->SendThread(nullptr);
}

void Slave::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		this->m_pDispatcher = new SlaveDispatcher();

	if (this->m_hReceiveThread == INVALID_HANDLE_VALUE)
		this->m_hReceiveThread = CreateThread(0, 0, ReceiveThreadWrapper, this->m_pDispatcher,0,0);

	if (this->m_hSendThread == INVALID_HANDLE_VALUE)
		this->m_hSendThread = CreateThread(0, 0, SendThreadWrapper, this->m_pDispatcher, 0, 0);
}

void Slave::DestroyDispatcher()
{
	DELETE_PTR(this->m_pDispatcher);
}

SlaveDispatcher* Slave::GetDispatcher()
{
	return this->m_pDispatcher ? this->m_pDispatcher : nullptr;
}

TaskExecutor* Slave::GetTaskExecutor()
{
	return this->m_pTaskExectutor;
}


