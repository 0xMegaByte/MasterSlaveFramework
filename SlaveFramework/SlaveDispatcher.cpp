#include "SlaveDispatcher.h"

DWORD __stdcall SlaveDispatcher::DispatcherThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		//Checks if WSA's status
		if (this->bWSA)
		{
			//Ref. to the socket for better understanding
			SOCKET& slave_socket = this->m_socket;

#pragma region connect

			//Connect to server (X times? + delay?)

			int nConnectRes = SOCKET_ERROR;

			
			if (nConnectRes = connect(slave_socket, this->m_pservice->ai_addr, (int)this->m_pservice->ai_addrlen) == SOCKET_ERROR)
			{
				//Failed - print error and exit/retry X times ->destory dispatcher outside the thread
				DEBUG_PRINT("connect failed! WSA:%d\n", WSA_ERR);
				closesocket(this->GetSocket());
				this->m_socket = INVALID_SOCKET;
			}

			//this->service can be freed

			if (this->m_socket == INVALID_SOCKET)
			{
				DEBUG_PRINT("Unable to connect to server!\n");
				this->SocketWSACleanup();

			}

			DEBUG_PRINT("Connected to server!\n");

#pragma endregion

			MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

			if (pPacketQueue)
			{
				int nResults = 0;
				char cRecvBuf[sizeof(MSFPacket)]{ 0 };

				while (this->bStart)
				{
					//TODO: add delay?
					if (pPacketQueue)
					{
						nResults = 0;
						ZeroMemory(cRecvBuf, sizeof(MSFPacket));

						do
						{
							nResults = recv(slave_socket, cRecvBuf, sizeof(MSFPacket), 0);
							if (nResults > 0)
							{
								printf("Bytes received: %d\n", nResults);
								//Handle recived bytes as MSFpacket
								MSFPacket* Packet = (MSFPacket*)cRecvBuf;

								printf("Bytes received: %d\n", nResults);

								//->ParsePacket() -> CreateTask

								//Create task using the MSFpacket data
								//Add task to Taskqueue
							}
							else if (nResults == 0)
							{
								//printf("Connection closed\n");
								DEBUG_PRINT("Connection closed\n");
							}
							else
							{
								//printf("recv failed: %d\n", WSAGetLastError());
								DEBUG_PRINT("Recv failed: %d\n", WSA_ERR);
							}

						} while (nResults > 0); //failed? terminate? retry?


						//Cleanup when this->deinitialize
					}
				}
			}
		}
		//close?
	}
	return 0;
}

void SlaveDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	if (this->bWSA)
	{
		addrinfo hints;

		ZeroMemory(&hints, sizeof(addrinfo));
		
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		if (pcIpAddress)
		{
			if (int res = getaddrinfo(pcIpAddress, "6969", &hints, &this->m_pservice) != 0)
			{
				DEBUG_PRINT("ERROR: gettaddrinfo code: %d | WSA: %d\n", res,WSA_ERR);
				this->SocketWSACleanup();
			}

			//Create a socket
			if (this->m_pservice)
			{
				addrinfo& service = *this->m_pservice;

				this->m_socket = socket(service.ai_family, service.ai_socktype, service.ai_protocol);

				if (this->m_socket == INVALID_SOCKET)
				{
					DEBUG_PRINT("Error at socket(): %ld\n", WSA_ERR);
					this->SocketWSACleanup();
					//TODO: Remember to free addrinfo using (freeaddrinfo(m_service))
				}

				if (this->m_socket != INVALID_SOCKET)
					DEBUG_PRINT("Socket initialized\n");

			}
		}
	}
	else
		DEBUG_PRINT("ERROR: WSA not working.\n");
}

//void PacketDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
//{

//}