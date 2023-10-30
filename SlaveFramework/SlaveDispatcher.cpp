#include "SlaveDispatcher.h"

DWORD __stdcall SlaveDispatcher::DispatcherThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		//Checks if WSA's status
		if (this->bWSA)
		{
#pragma region 
			//Ref. to the socket for better understanding
			SOCKET& slave_socket = this->m_socket;

			//Connect to server (X times? + delay?)
			if (connect(slave_socket, (sockaddr*)&this->m_service, sizeof(this->m_service)) == SOCKET_ERROR)
			{
				//Failed - print error and exit/retry X times ->destory dispatcher outside the thread
			}

			while (this->bStart)
			{
				int nResults = 0;
				char cRecvBuf[BUF_LEN]{ 0 };
				do
				{
					nResults = recv(slave_socket, cRecvBuf, BUF_LEN, 0);
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

		}
		//close?
	}
	return 0;
}

void SlaveDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	sockaddr_in& service = this->m_service;

	if (pcIpAddress)
	{

		service.sin_family = AF_INET;
		service.sin_port = usPort;
		inet_pton(AF_INET, pcIpAddress, &service.sin_addr);
	}
}

//void PacketDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
//{

//}