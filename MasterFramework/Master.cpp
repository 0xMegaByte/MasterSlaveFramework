#include "Master.h"

Master::Master() :
	m_pDispatcher(nullptr), m_hDispatcherThread(INVALID_HANDLE_VALUE)
{
	this->m_ulCurrentConnections = 0;
	this->m_ulTotalSlavesCount = 0;

}

Master::~Master()
{
	DELETE_PTR(m_hDispatcherThread);
	DELETE_PTR(this->m_pDispatcher);
}

DWORD WINAPI DispatcherThreadWrapper(LPVOID lpv)
{
	MasterDispatcher* pmd = static_cast<MasterDispatcher*>(lpv);
	return pmd->DispatcherThread(nullptr);
}

void Master::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		this->m_pDispatcher = new MasterDispatcher();


	//Create Dispatcher Thread
	//BUG: Move to the dispatcher itself when init.
	//thread will run but will be stopped until event signaled
	if (this->m_hDispatcherThread == INVALID_HANDLE_VALUE)
	{
		this->m_hDispatcherThread = CreateThread(0, 0, DispatcherThreadWrapper, this->m_pDispatcher, 0, 0);
		if (this->m_hDispatcherThread)
			DEBUG_PRINT("Dispatcher thread started\n");
	}

	DEBUG_PRINT("Completed\n");
}
void Master::DestroyDispatcher()
{
	DELETE_PTR(this->m_pDispatcher);
}

MasterDispatcher* Master::GetDispatcher()
{
	return this->m_pDispatcher ? this->m_pDispatcher : nullptr;
}
