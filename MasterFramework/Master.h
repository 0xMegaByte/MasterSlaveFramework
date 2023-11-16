#pragma once
#include "MasterDispatcher.h"



class Master
{
private:
	unsigned long m_ulTotalSlavesCount;
	unsigned long m_ulCurrentConnections;

	MasterDispatcher* m_pDispatcher; //WithPacketQueue
	void* m_hSendThread;
	void* m_hReceiveThread;
	
public:

	//Handle Dispatcher
	void CreateDispatcher();
	void DestroyDispatcher();
	MasterDispatcher* GetDispatcher();

	DWORD WINAPI ProcessSlave(LPVOID lpv); //will handle send/recv per socket(slave)
	//instead of Send/RecveiveThread

	Master();
	~Master();
};