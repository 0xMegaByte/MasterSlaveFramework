#pragma once
#include "MasterDispatcher.h"



class Master
{
private:
	/*unsigned long m_ulTotalSlavesCount;*/
	//unsigned long m_ulCurrentConnections;

	MasterDispatcher* m_pDispatcher; //WithPacketQueue

	void* m_hAcceptConnections;
	
public:

	//Handle Dispatcher
	void CreateDispatcher();
	void DestroyDispatcher();
	MasterDispatcher* GetDispatcher();

	Master();
	~Master();
};