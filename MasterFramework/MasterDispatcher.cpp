#include "MasterDispatcher.h"

/*DWORD WINAPI MasterDispatcher::SendThread(LPVOID lpv)
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
				}
			}
		}
		//close?

	}
	DEBUG_PRINT("Thread existed\n");
	return 0;
}*/

/*DWORD WINAPI MasterDispatcher::ReceiveThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (this->m_bWSA)
		{
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
}*/

DWORD __stdcall ProcessSlave(LPVOID lpv)
{
	DEBUG_PRINT("Thread running...\n");
	Connection* pConnection = new Connection(*static_cast<Connection*>(lpv));

	bool bPacketSent = false;

	if (pConnection)
	{
		DEBUG_PRINT("Connection's details converted\n");
		unsigned long ulSlaveId = pConnection->m_ulSlaveId;

		bool bStart = pConnection->m_bStart;

		MSFPacketQueue* pPacketQueue = pConnection->m_pSlavePacketQueue;

		if (pPacketQueue)
		{
			SOCKET SlaveSocket = pConnection->m_socket;

			if (SlaveSocket)
			{
				int nbytesSent;
				int nbytesRecv = SOCKET_ERROR;
				char sendBuf[MSFPACKET_SIZE]{ 0 };
				char recvBuf[MSFPACKET_SIZE]{ 0 };

				/*timeval tvSocketBlocking
				{
					tvSocketBlocking.tv_sec = 0l,
					tvSocketBlocking.tv_usec = 500l
				};*/

				//setsockopt(SlaveSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tvSocketBlocking, sizeof(tvSocketBlocking));
				//setsockopt(SlaveSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tvSocketBlocking, sizeof(tvSocketBlocking));

				//Send init packet

				//Get front packet
				if (!pPacketQueue->empty())
				{
					MSFPacket* pAckPacket = pPacketQueue->front();

					if (pAckPacket)
					{
						//check if Ack type
						if (pAckPacket->getPacketType() == EPACKET::PacketType::Acknowledge)
						{
							DEBUG_PRINT("Sending ACK Packet to slave\n");
							//send it
							char cAckPacketBuffer[MSFPACKET_SIZE]{ 0 };
							memcpy_s(cAckPacketBuffer, MSFPACKET_SIZE, pAckPacket, MSFPACKET_SIZE);

							nbytesSent = send(SlaveSocket, cAckPacketBuffer, MSFPACKET_SIZE, 0);
							if (nbytesSent == SOCKET_ERROR)
							{
								DEBUG_PRINT("Failed to send packet! WSA:%d\n", WSA_ERR);
							}

							DEBUG_PRINT("Master sent ACK to Slave(%lu)\n", ulSlaveId);
						}

						//pop it from queue
						pPacketQueue->pop_front();
						//delete it from mem
						DELETE_PTR(pAckPacket);
					}
				}

				while (bStart)
				{
					//Handle Packet Queue send commands
					if (!pPacketQueue->empty())
					{
						MSFPacket* pPacket = pPacketQueue->front();
						if (pPacket)
						{
							char cPacketBuffer[MSFPACKET_SIZE]{ 0 };
							memcpy_s(cPacketBuffer, MSFPACKET_SIZE, pPacket, MSFPACKET_SIZE);

							nbytesSent = send(SlaveSocket, cPacketBuffer, MSFPACKET_SIZE, 0);
							if (nbytesSent == SOCKET_ERROR)
							{
								DEBUG_PRINT("Failed to send packet! WSA:%d\n", WSA_ERR);
							}
						}
						pPacketQueue->pop_front();
						DELETE_PTR(pPacket);

						DEBUG_PRINT("Waiting for slave reponse..\n");

						//Handle Recv after sending
						nbytesRecv = recv(SlaveSocket, recvBuf, MSFPACKET_SIZE, 0);
						if (nbytesRecv > 0)
						{
							DEBUG_PRINT("Slave(%lu) responded with <%d> Bytes\n", ulSlaveId, nbytesRecv);
							//Handle recived bytes as MSFpacket
							//Report to DB packets response using the MSFpacket data
						}
						else if (nbytesRecv == 0)
						{
							DEBUG_PRINT("Connection with slave(%lu) closing...\n", ulSlaveId);
						}
						else
						{
							//fail
							DEBUG_PRINT("Recv() failed. WSA:%d\n", WSA_ERR);
						}
					}

					
				}
			}
		}
	}

	DEBUG_PRINT("Existing Thread...\n");
	return 0;
}

DWORD WINAPI MasterDispatcher::AcceptConnectionThread(LPVOID lpv)
{
	DEBUG_PRINT_CLS("Entered Thread...\n");

	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT_CLS("Thread Running!\n");
		if (this->m_bWSA)
		{
			SOCKET& master_socket = this->m_socket;
			SOCKET tempSocket = INVALID_SOCKET;

			while (this->m_bStart)
			{
				//Run until connection accepted
				DEBUG_PRINT_CLS("Waiting for a new slave socket..\n");
				do
				{
					tempSocket = accept(master_socket, 0, 0);

				} while (tempSocket == INVALID_SOCKET);

				//Connection accepted
				DEBUG_PRINT_CLS("Master socket accepted slave socket successfully.\n");

				//Create new slave's packet queue
				MSFPacketQueue* pNewConnectionPQ = new MSFPacketQueue();

				if (pNewConnectionPQ)
				{
					this->IncrementTotalSlaveCount(); //Started with 0

					unsigned long ulSlaveId = this->GetTotalSlaveCount();

					//Issue new connection info
					Connection NewConnection
					{
						ulSlaveId, //Assign SlaveId
						tempSocket,
						pNewConnectionPQ,
						this->m_bStart
					};

					HANDLE hNewConnection = INVALID_HANDLE_VALUE;
					hNewConnection = CreateThread(0, 0, ProcessSlave, &NewConnection, CREATE_SUSPENDED,0);

					if (hNewConnection)
					{
						DEBUG_PRINT_CLS("New 'ProcessSlave' thread created\n");

						this->AddSlaveThreadHandle(ulSlaveId, hNewConnection);
						DEBUG_PRINT_CLS("Registered new thread handle\n");

						this->AddConnection(NewConnection);
						DEBUG_PRINT_CLS("Registered new connection object\n");

						MSFPacket* pAckPacket = new MSFPacket(
							EPACKET::PacketType::Acknowledge,
							ulSlaveId,
							EPACKET::CMD::MASTER_SLAVE_ACK_CONNECTION,
							(unsigned char*)"");

						pNewConnectionPQ->push_back(pAckPacket);
						DEBUG_PRINT_CLS("Added ACK packet to slave's packet queue\n");
						ResumeThread(hNewConnection);
					}
					
					pNewConnectionPQ = nullptr;
				}

				tempSocket = INVALID_SOCKET;
			}
		}
	}

	DEBUG_PRINT_CLS("Existing Thread...\n");
	return 0;
}

void MasterDispatcher::AddConnection(Connection connection)
{
	this->m_ConnectionsLock.lock();
	this->m_vConnections.push_back(connection);
	this->m_ConnectionsLock.unlock();
}

void MasterDispatcher::AddSlaveThreadHandle(unsigned long ulSlaveId, void* hSlaveThread)
{
	this->m_SlaveThreadHandlesLock.lock();
	this->m_umSlaveThreadHandles.insert({ ulSlaveId,hSlaveThread });
	this->m_SlaveThreadHandlesLock.unlock();
}

void MasterDispatcher::IncrementTotalSlaveCount()
{
	this->m_SlaveCountLock.lock();
	++this->m_ulTotalSlavesCount;
	this->m_SlaveCountLock.unlock();

}

void MasterDispatcher::DecrementTotalSlaveCount()
{
	this->m_SlaveCountLock.lock();
	--this->m_ulTotalSlavesCount;
	this->m_SlaveCountLock.unlock();
}

unsigned long MasterDispatcher::GetTotalSlaveCount()
{
	return this->m_ulTotalSlavesCount;
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