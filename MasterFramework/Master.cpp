#include "Master.h"

Master::Master() :
	m_pDispatcher(nullptr), m_hReceiveThread(INVALID_HANDLE_VALUE),
	m_hSendThread(INVALID_HANDLE_VALUE)
{
	this->m_ulCurrentConnections = 0;
	this->m_ulTotalSlavesCount = 0;

}

Master::~Master()
{
	//DELETE_PTR(m_hDispatcherThread);
	//TODO:
	DELETE_PTR(this->m_pDispatcher);
}

DWORD WINAPI ReceiveThreadWrapper(LPVOID lpv)
{
	MasterDispatcher* pmd = static_cast<MasterDispatcher*>(lpv);
	return pmd->ReceiveThread(nullptr);
}

DWORD WINAPI SendThreadWrapper(LPVOID lpv)
{
	MasterDispatcher* psd = static_cast<MasterDispatcher*>(lpv);
	return psd->SendThread(nullptr);
}

void Master::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		this->m_pDispatcher = new MasterDispatcher();


	//Create Dispatcher Thread
	//BUG: Move to the dispatcher itself when init.
	//thread will run but will be stopped until event signaled
	if (this->m_hReceiveThread == INVALID_HANDLE_VALUE)
		this->m_hReceiveThread = CreateThread(0, 0, ReceiveThreadWrapper, this->m_pDispatcher, 0, 0);

	if (this->m_hSendThread == INVALID_HANDLE_VALUE)
		this->m_hSendThread = CreateThread(0, 0, SendThreadWrapper, this->m_pDispatcher, 0, 0);

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
