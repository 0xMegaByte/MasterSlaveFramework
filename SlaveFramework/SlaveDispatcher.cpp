#include "SlaveDispatcher.h"

unsigned long __stdcall SlaveDispatcher::DispatcherThread(void* lpv)
{
	// ==> Client Thread
	//	--> Connect to server
	//	--> Send/Recv (n Times, until termination packet)
	//	--> Close

	if (this->bWSA)
	{
		SOCKET& slave_socket = this->m_socket;

		//Connect to server
		if (connect(slave_socket,(sockaddr*)&this->m_service,sizeof(this->m_service)) == SOCKET_ERROR)
		{
			//Failed - print error and exit/retry X times ->destory dispatcher outside the thread
		}

		int nResults = 0;
		char ucRecvBuf[BUF_LEN]{ 0 };
		do
		{
			nResults = recv(slave_socket, ucRecvBuf, BUF_LEN, 0);
			if (nResults > 0)
			{
				//printf("Bytes received: %d\n", nResults);
				//Handle recived bytes as MSFpacket
				//Create task using the MSFpacket data
				//Add task to Taskqueue
			}
			else if (nResults == 0)
			{
				//printf("Connection closed\n");
			}
			else
			{
				//printf("recv failed: %d\n", WSAGetLastError());
			}

		} while (nResults > 0); //failed? terminate? retry?


		//Cleanup when this->deinitialize


	}
	return 0;
}
