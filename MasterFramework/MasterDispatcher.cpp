#include "MasterDispatcher.h"

DWORD WINAPI MasterDispatcher::DispatcherThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		//Check if WSA's status
		if (this->bWSA)
		{

			SOCKET& master_socket = this->m_socket; //Ref. to the socket for better understanding

			//Bind to a network address
			if (bind(master_socket, (sockaddr*)&this->m_service, sizeof(this->m_service)) == SOCKET_ERROR)
			{
				//fail
			}

			//Start listen on the address-port for incoming connections
			if (listen(master_socket, SOMAXCONN) == SOCKET_ERROR)
			{
				//fail
			}

#pragma region run until terminated


			SOCKET incoming_slave_socket = INVALID_SOCKET;
			incoming_slave_socket = accept(master_socket, NULL, NULL);

			if (incoming_slave_socket == INVALID_SOCKET)
			{
				//fail
			}

			//Handle Packet Queue
			auto& pPacketQueue = this->m_pPacketQueue;

			while (this->bStart) //Flag to terminate this thread
			{
				//delay?

				if (pPacketQueue)
				{
					if (!pPacketQueue->empty())
					{
						MSFPacket packet = pPacketQueue->front();

						//TODO: send packet to slave

						pPacketQueue->pop_front();
					}
				}

				//Check if any recv response buffer


				int nResults, nSendResults;
				char cRecvBuf[BUF_LEN]{ 0 };

				nResults = nSendResults = 0;

				do
				{
					nResults = recv(incoming_slave_socket, cRecvBuf, BUF_LEN, 0);

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
#pragma endregion
		}
		//close?

	}
	return 0;
}

void MasterDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	unsigned long ulIpAddress = 0ul;
	sockaddr_in& service = this->m_service;

	service.sin_family = AF_INET;
	service.sin_port = usPort;

	if (pcIpAddress)
		inet_pton(AF_INET, pcIpAddress, &service.sin_addr);
	
	else if (!pcIpAddress)
		service.sin_addr.S_un.S_addr = INADDR_ANY;
}
