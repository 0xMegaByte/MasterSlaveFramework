#include "SlaveDispatcher.h"

DWORD __stdcall SlaveDispatcher::ReceiveThread(LPVOID lpv)
{
	DEBUG_PRINT_CLS("Entered thread\n");
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT_CLS("Thread running..\n");
		//Checks if WSA's status
		if (this->m_bWSA)
		{
			//Ref. to the socket for better understanding
			SOCKET& slave_socket = this->m_socket;

			MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

			if (pPacketQueue)
			{
				int nResults = 0;
				char cRecvBuf[sizeof(MSFPacket)]{ 0 };

				while (this->m_bStart)
				{
					if (pPacketQueue)
					{
						nResults = 0;
						ZeroMemory(cRecvBuf, sizeof(MSFPacket));

						do
						{
							nResults = recv(slave_socket, cRecvBuf, sizeof(MSFPacket), 0);
							if (nResults > 0)
							{
								DEBUG_PRINT_CLS("Bytes received: %d\n", nResults);

								//Handle recived bytes as MSFpacket
								MSFPacket* pPacket = (MSFPacket*)cRecvBuf;
								if (pPacket)
								{

									pPacket->PrintPacket();

									switch (pPacket->getPacketType())
									{
									case EPACKET::PacketType::Acknowledge:
									{
										MSFPacket* pACKPacket =
											new MSFPacket(EPACKET::PacketType::ResponsePacket,
												pPacket->getSlaveId(), EPACKET::RESP::SLAVE_MASTER_OK_RESPONSE,
												(unsigned char*)"<some_data>");

										this->SecureQueuePushBack(pACKPacket);

										DEBUG_PRINT_CLS("Pushed ACK packet to queue\n");

										break;
									}
									case EPACKET::PacketType::TaskPacket:
										break;


									default:
										break;
									}
								}
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

							Sleep(500);
						} while (nResults > 0); //failed? terminate? retry?


						//Cleanup when this->deinitialize
					}
				}
			}
		}
		//close?
	}

	DEBUG_PRINT_CLS("Exisiting thread..\n");
	return 0;
}

DWORD __stdcall SlaveDispatcher::SendThread(LPVOID lpv)
{
	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (this->m_bWSA)
		{
			SOCKET& slave_socket = this->m_socket;
			MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

			while (this->m_bStart)
			{

				if (pPacketQueue)
				{
					if (!pPacketQueue->empty())
					{
						DEBUG_PRINT("Packet queue size: %llu.\n", pPacketQueue->size());

						MSFPacket* pPacket = pPacketQueue->front();

						if (pPacket)
						{
							DEBUG_PRINT_CLS("Sending packet:\n");
							pPacket->PrintPacket();

							char buffer[sizeof(pPacket)]{ 0 };
							memcpy_s(buffer, sizeof(pPacket), pPacket, sizeof(pPacket));

							int nbytesSent = send(slave_socket, (const char*)buffer, sizeof(buffer), 0);
							if (nbytesSent == SOCKET_ERROR)
							{
								DEBUG_PRINT_CLS("Failed to send packet! WSA:%d\n", WSA_ERR);
							}

							this->SecureQueuePopFront();
							DELETE_PTR(pPacket);
						}
					}
				}
			}
		}
	}
	return 0;
}

void SlaveDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	if (this->m_bWSA)
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
				DEBUG_PRINT("ERROR: gettaddrinfo code: %d | WSA: %d\n", res, WSA_ERR);
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
				{
					DEBUG_PRINT("Socket initialized\n");
				}
			}
		}
	}
	else
		DEBUG_PRINT("ERROR: WSA not working.\n");
}

void SlaveDispatcher::Connect()
{
	if (this->m_bWSA)
	{
		int nConnectRes = SOCKET_ERROR;

		SOCKET& slave_socket = this->m_socket;

		if (nConnectRes = connect(slave_socket, this->m_pservice->ai_addr, (int)this->m_pservice->ai_addrlen) == SOCKET_ERROR)
		{
			//Failed - print error and exit/retry X times ->destory dispatcher outside the thread
			DEBUG_PRINT("connect() failed! WSA:%d\n", WSA_ERR);
			closesocket(slave_socket);
			this->m_socket = INVALID_SOCKET;
			//TODO: BUG: WHEN FAILED, THE SOFTWARE CONTINUES TO RUN
		}

		//this->service can be freed

		if (slave_socket == INVALID_SOCKET)
		{
			DEBUG_PRINT("Unable to connect to server!\n");
			this->SocketWSACleanup();

		}
		else
		{
			DEBUG_PRINT("Connected to server!\n");
			this->m_bConnected = true;
		}
	}
}

bool SlaveDispatcher::IsDispatcherConnected()
{
	return this->m_bConnected;
}

void SlaveDispatcher::SecureQueuePushBack(MSFPacket* pPacket)
{
	this->m_PacketQueueLock.lock();
	{
		if (pPacket)
			this->m_pPacketQueue->push_back(pPacket);
	}
	this->m_PacketQueueLock.unlock();
}

void SlaveDispatcher::SecureQueuePopFront()
{
	this->m_PacketQueueLock.lock();
	{
		this->m_pPacketQueue->pop_front();
	}
	this->m_PacketQueueLock.unlock();
}
