#include "MasterDispatcher.h"

unsigned long __stdcall MasterDispatcher::DispatcherThread(void* lpv)
{
	// Start Dispatcher		- 1 time >	Start Dispatcher Thread (Event signaling)
	// ==> Server Thread
	//	--> Bind
	//	--> Listen for incoming connections
	//	--> Accept clients' socket
	//	--> Close

	if (this->bWSA)
	{
		SOCKET& master_socket = this->m_socket;

		if (bind(master_socket, (sockaddr*)&this->m_service, sizeof(this->m_service)) == SOCKET_ERROR)
		{
			//fail
		}

		if (listen(master_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			//fail
		}


	}

	return 0;
}
