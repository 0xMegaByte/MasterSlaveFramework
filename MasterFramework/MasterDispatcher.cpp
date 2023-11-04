#include "MasterDispatcher.h"

DWORD WINAPI MasterDispatcher::DispatcherThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT("Dispatcher event signaled\n");

		//Check if WSA's status
		if (this->bWSA)
		{

			SOCKET& master_socket = this->m_socket; //Ref. to the socket for better understanding

			if (this->m_socket)
			{
#pragma region bind
				//Bind to a network address
				auto& service = this->m_pservice;
				int nRes = bind(this->m_socket, service->ai_addr, (int)service->ai_addrlen);

				if (nRes == SOCKET_ERROR)
				{
					//fail
					DEBUG_PRINT("bind() failed! WSA:%d\n", WSA_ERR);
					closesocket(master_socket);
					this->m_socket = INVALID_SOCKET;
					this->SocketWSACleanup();
				}

				DEBUG_PRINT("Master socket bind() successfully.\n");
#pragma endregion
#pragma region listen

				//Start listen on the address-port for incoming connections

				nRes = listen(master_socket, SOMAXCONN);

				if (nRes == SOCKET_ERROR)
				{
					//fail
					DEBUG_PRINT("listen() failed! WSA:%ld\n", WSA_ERR);
					closesocket(master_socket);
					this->m_socket = INVALID_SOCKET;
					this->SocketWSACleanup();

				}

				DEBUG_PRINT("Master socket listen() successfully.\n");

#pragma endregion
#pragma region accept
				//Accept client socket

				SOCKET slave_socket = INVALID_SOCKET;
				slave_socket = accept(master_socket, NULL, NULL);

				if (slave_socket == INVALID_SOCKET)
				{
					//fail
					DEBUG_PRINT("accept() failed! WSA:%ld\n", WSA_ERR);
					closesocket(master_socket);
					this->m_socket = INVALID_SOCKET;
					this->SocketWSACleanup();
				}

				DEBUG_PRINT("Master socket accepted slave socket successfully.\n");

#pragma endregion

				while (this->bStart) //Flag to terminate this thread
				{
					//Handle Packet Queue
					MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

					//delay?

					if (pPacketQueue)
					{
						if (!pPacketQueue->empty())
						{
							DEBUG_PRINT("Packet queue size: %llu.\n", pPacketQueue->size());

							MSFPacket* packet = pPacketQueue->front();

							char buffer[sizeof(MSFPacket)]{ 0 };

							memcpy_s(buffer, sizeof(MSFPacket), packet, sizeof(MSFPacket));
							
							//TODO: sending too much information
							//send(slave_socket, (const char*)packet, sizeof(packet), 0);
							send(slave_socket, (const char*)buffer, sizeof(buffer), 0);

							DEBUG_PRINT("Packet sent successfully.\n");

							//pPacketQueue->pop_front(); commented for test
						}
					}

					//Check if any recv response buffer


					int nResults, nSendResults;
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
				}
			}
		}
		//close?

	}
	return 0;
}

void MasterDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	if (this->bWSA)
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

			if (this->m_socket != INVALID_SOCKET)
				DEBUG_PRINT("Socket initialized\n");
		}
	}
	else
		DEBUG_PRINT("ERROR: WSA was not initiated!\n");

}
