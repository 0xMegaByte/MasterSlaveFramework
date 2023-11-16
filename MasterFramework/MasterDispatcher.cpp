#include "MasterDispatcher.h"

//TODO: Delete. for test only
SOCKET slave_socket = INVALID_SOCKET;

DWORD WINAPI MasterDispatcher::SendThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT("Dispatcher event signaled\n");

		//Check if WSA's status
		if (this->m_bWSA)
		{

			SOCKET& master_socket = this->m_socket; //Ref. to the socket for better understanding

			if (this->m_socket)
			{
				//Handle Packet Queue
				MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

				while (this->m_bStart) //Flag to terminate this thread
				{
					

					//delay?

					if (pPacketQueue)
					{
						if (!pPacketQueue->empty())
						{
							DEBUG_PRINT("Packet queue size: %llu.\n", pPacketQueue->size());

							MSFPacket* pPacket = pPacketQueue->front();

							



							char buffer[sizeof(MSFPacket)]{ 0 };

							memcpy_s(buffer, sizeof(MSFPacket), pPacket, sizeof(MSFPacket));

							//TODO: make the packet sendong size dynamic. 
							// no need to send the whole empty bytes in the param.
							//send(slave_socket, (const char*)packet, sizeof(packet), 0);
							



							send(slave_socket, (const char*)buffer, sizeof(buffer), 0);

							DEBUG_PRINT("Packet sent successfully.\n");

							delete pPacketQueue->front();
							pPacketQueue->pop_front();// commented for test
						}
					}

					////Check if any recv response buffer


					/*int nResults, nSendResults;
					char cRecvBuf[BUF_LEN]{ 0 };

					nResults = nSendResults = 0;

					do
					{
						nResults = recv(slave_socket, cRecvBuf, BUF_LEN, 0);

						if (nResults > 0)
						{
							int i = 1;
							++i;
							//printf("Bytes received: %d\n", iResult);
							//Handle recived bytes as MSFpacket
							//Report to DB packets response using the MSFpacket data
						}
						else if (nResults == 0)
						{
							//printf("Connection closing...\n");
						}
						else
						{
							//fail
							//printf("%d", WSA_ERR);
						}

					} while (nResults > 0);
					*/
				}
			}
		}
		//close?

	}
	DEBUG_PRINT("Thread existed\n");
	return 0;
}

DWORD WINAPI MasterDispatcher::ReceiveThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (this->m_bWSA)
		{
			//Check if any recv response buffer


			int nResults, nSendResults;
			char cRecvBuf[BUF_LEN]{ 0 };

			nResults = nSendResults = 0;

			//TODO: iterate slaves' sockets
			while (slave_socket == INVALID_SOCKET)
			{
				//TODO: Test only, waits for slave_socket to be valid
			}
			do
			{
				nResults = recv(slave_socket, cRecvBuf, BUF_LEN, 0);

				if (nResults > 0)
				{
					int i = 1;
					++i;
					printf("Bytes received to master: %d\n", 1);
					//Handle recived bytes as MSFpacket
					//Report to DB packets response using the MSFpacket data
				}
				else if (nResults == 0)
				{
					printf("Connection closing...\n");
				}
				else
				{
					//fail
					printf("%d\n", WSA_ERR);
				}

			} while (nResults > 0); //BUG: Thread terminates due to the socket initialization is in the send thread and not outside these threads
		}
	}
	DEBUG_PRINT("Thread Exited\n");
	return 0;
}

struct ProcessSlaveStruct
{
	MasterDispatcher* pmd;
	SOCKET socket;
};

DWORD WINAPI ProcessSlaveWrapper(LPVOID lpv)
{
	ProcessSlaveStruct* pss = static_cast<ProcessSlaveStruct*>(lpv);
	if (pss)
	{
		MasterDispatcher* pmd = pss->pmd;
		if (pmd)
		{
			pmd->ProcessSlave((LPVOID)pss->socket);
		}
	}


	return 0;
}

DWORD WINAPI MasterDispatcher::AcceptConnectionThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (this->m_bWSA)
		{
			//Gets an empty vector of SOCKET type
			//Use tempSOCKET to wait on an incoming connection
			//When tempSOCKET is a valid socket
			//Copy ctor it to the vector
			//Reset the tempSocket (assign INVALID_SOCKET)
			//Repeat

			SOCKET& master_socket = this->m_socket;
			SOCKET tempSocket = INVALID_SOCKET;
			while (this->m_bStart)
			{
				do
				{
					tempSocket = accept(master_socket, 0, 0);

				} while (tempSocket == INVALID_SOCKET);

				//Connection accepted
				DEBUG_PRINT("Master socket accepted slave socket successfully.\n");
				//Create thead with

				DWORD dwThreadId = 0;

				ProcessSlaveStruct pss{ this,tempSocket };

				CreateThread(0, 0, ProcessSlaveWrapper, &pss, 0, &dwThreadId);

				tempSocket = INVALID_SOCKET;
				

				

				//

			}
		}
	}

	return 0;
}

DWORD __stdcall MasterDispatcher::ProcessSlave(LPVOID lpv)
{
	SOCKET SlaveSocket = (SOCKET)lpv;

	if (SlaveSocket)
	{
		int nbytesSent;
		int nbytesRecv = SOCKET_ERROR;
		char sendBuf[BUF_LEN];
		char recvBuf[BUF_LEN];

		//Send init packet

		while (this->m_bStart)
		{

		}
	}

	return 0;
}

void MasterDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	if (this->m_bWSA)
	{
		addrinfo hints;

		ZeroMemory(&hints, sizeof(addrinfo));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		if (int res = getaddrinfo(NULL, "6969", &hints, &this->m_pservice) != 0)
		{
			DEBUG_PRINT("ERROR: gettaddrinfo code: %d | WSA: %d\n", res, WSA_ERR);
			this->SocketWSACleanup();
		}

		//Create socket
		if (this->m_pservice)
		{
			addrinfo& service = *this->m_pservice;

			// Create a SOCKET for the server to listen for client connections
			this->m_socket = socket(service.ai_family, service.ai_socktype, service.ai_protocol);

			if (this->m_socket == INVALID_SOCKET)
			{
				DEBUG_PRINT("Error at socket(): %ld\n", WSA_ERR);
				this->SocketWSACleanup();
				//TODO: Remember to free addrinfo using (freeaddrinfo(m_service))
			}

			if (this->m_socket)
			{
				DEBUG_PRINT("Socket initialized\n");

				addrinfo*& service = this->m_pservice;

#pragma region bind

				int nRes = bind(this->m_socket, service->ai_addr, (int)service->ai_addrlen);

				if (nRes == SOCKET_ERROR)
				{
					//fail
					DEBUG_PRINT("bind() failed! WSA:%d\n", WSA_ERR);
					closesocket(this->m_socket);
					this->m_socket = INVALID_SOCKET;
					this->SocketWSACleanup();
				}

				DEBUG_PRINT("Master socket bind() successfully.\n");
#pragma endregion
#pragma region listen

				//Start listen on the address-port for incoming connections

				nRes = listen(this->m_socket, SOMAXCONN);

				if (nRes == SOCKET_ERROR)
				{
					//fail
					DEBUG_PRINT("listen() failed! WSA:%ld\n", WSA_ERR);
					closesocket(this->m_socket);
					this->m_socket = INVALID_SOCKET;
					this->SocketWSACleanup();

				}

				DEBUG_PRINT("Master socket listen() successfully.\n");

#pragma endregion

			}


		}
	}
	else
		DEBUG_PRINT("ERROR: WSA was not initiated!\n");

}