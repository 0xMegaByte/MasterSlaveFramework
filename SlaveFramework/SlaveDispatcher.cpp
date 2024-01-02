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
#include "SlaveDispatcher.h"
#include "Task.h"

DWORD __stdcall SlaveDispatcher::ReceiveThread(LPVOID lpv)
{
	DEBUG_PRINT_CLS("Entered thread\n");

	if (TaskExecutor* pTaskExecutor = (TaskExecutor*)this->GetTaskExecutor())
	{
		if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
		{
			DEBUG_PRINT_CLS("Thread running..\n");

			if (this->m_bWSA)
			{
				//Ref. to the socket for better understanding
				SOCKET& _socket = this->m_socket;
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
								nResults = recv(_socket, cRecvBuf, sizeof(MSFPacket), 0);
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
											const char* buf = "<some_data>";

											MSFPacket* pACKPacket =
												new MSFPacket(EPACKET::PacketType::ResponsePacket,
													pPacket->getSlaveId(), (unsigned int)EPACKET::RESP::SLAVE_MASTER_OK_RESPONSE,
													(unsigned char*)buf);

											if (pACKPacket)
											{
												this->SecureQueuePushBack(pACKPacket);

												DEBUG_PRINT_CLS("Pushed ACK packet to queue\n");
											}

											break;
										}
										case EPACKET::PacketType::TaskPacket:
										{
											//Push new task to taskexecutor
											Task* pTask = new Task((EPACKET::Task)pPacket->getOpCode());
											if (pTask)
											{
												pTaskExecutor->SecurePushBack(pTask);
											}

											break;

										}
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
	}
	DEBUG_PRINT_CLS("Exisiting thread..\n");
	return 0;
}

DWORD __stdcall SlaveDispatcher::SendThread(LPVOID lpv)
{
	DEBUG_PRINT_CLS("Entered thread\n");

	if (WaitForSingleObject(this->m_hDispatcherEvent, INFINITE) == WAIT_OBJECT_0)
	{
		DEBUG_PRINT_CLS("Thread running..\n");

		if (this->m_bWSA)
		{
			SOCKET& slave_socket = this->m_socket;
			MSFPacketQueue* pPacketQueue = this->m_pPacketQueue;

			if (pPacketQueue)
			{
				while (this->m_bStart)
				{
					if (!pPacketQueue->empty())
					{
						DEBUG_PRINT("Packet queue size: %llu.\n", pPacketQueue->size());

						//TODO: Add lock
						MSFPacket* pPacket = pPacketQueue->front();

						if (pPacket)
						{
							DEBUG_PRINT_CLS("Sending packet:\n");

							pPacket->PrintPacket();

							char cPacketBuffer[MSFPACKET_SIZE]{ 0 };
							memcpy_s(cPacketBuffer, MSFPACKET_SIZE, pPacket, MSFPACKET_SIZE);

							int nbytesSent = send(slave_socket, cPacketBuffer, MSFPACKET_SIZE, 0);
							if (nbytesSent == SOCKET_ERROR)
							{
								DEBUG_PRINT_CLS("Failed to send packet! [WSA:%d]\n", WSA_ERR);
							}

							this->SecureQueuePopFront();
							DELETE_PTR(pPacket);
						}
					}
				}
			}
		}
	}
	DEBUG_PRINT_CLS("Exisiting thread..\n");
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
			std::string sPort = std::to_string(usPort);

			if (int res = getaddrinfo(pcIpAddress, sPort.c_str(), &hints, &this->m_pservice) != 0)
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
	{
		DEBUG_PRINT("ERROR: WSA not working.\n");
	}
}

void SlaveDispatcher::Connect()
{
	if (this->m_bWSA)
	{
		int nConnectRes = SOCKET_ERROR;
		int nRetriesLeft = 10;
		int nRetriesCount = 0;

		SOCKET& slave_socket = this->m_socket;

		do
		{
			nConnectRes =
				connect(slave_socket, this->m_pservice->ai_addr, (int)this->m_pservice->ai_addrlen);

			if (nConnectRes == SOCKET_ERROR)
			{
				DEBUG_PRINT_CLS("Failed to connect to server! [WSA:%d]\n", WSA_ERR);

				nRetriesCount++;

				if (nRetriesCount == nRetriesLeft)
				{
					DEBUG_PRINT_CLS("Retries exhausted! connect() failed: [WSA:%d]\n", WSA_ERR);
					closesocket(slave_socket);
					if (this->m_socket == INVALID_SOCKET)
					{
						this->m_socket = INVALID_SOCKET;
						SocketWSACleanup();
					}
					break;
				}
				else
				{
					DEBUG_PRINT_CLS("Retrying to connect again.. (%d/%d)\n\n",
						nRetriesCount, nRetriesLeft);
				}
			}
			else if (nConnectRes == 0 && slave_socket != INVALID_SOCKET)
			{
				DEBUG_PRINT("Connected to server!\n");
				this->m_bConnected = true;

				//this->CreateDispatcherThreads();

				break;
			}
		} while (!this->m_bConnected);
	}
}

void SlaveDispatcher::AssignTaskExecutor(void* pTaskExecutor)
{
	this->m_pTaskExecutor = pTaskExecutor;
}

void* SlaveDispatcher::GetTaskExecutor()
{
	return this->m_pTaskExecutor;
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

SlaveDispatcher::SlaveDispatcher() :
	PacketDispatcher(), m_bConnected(false), m_pTaskExecutor(nullptr), m_pPacketQueue(nullptr),
	m_hReceiveThread(INVALID_HANDLE_VALUE), m_hSendThread(INVALID_HANDLE_VALUE)
{
	if (!this->m_pPacketQueue)
	{
		this->m_pPacketQueue = new MSFPacketQueue();
	}
}

struct ThreadArgs
{
	SlaveDispatcher* m_pSlaveDisptacher = nullptr;
	void* m_pTaskExecutor = nullptr;
};

DWORD WINAPI ReceiveThreadWrapper(LPVOID lpv)
{
	if (lpv)
	{
		if (SlaveDispatcher* pSlaveDispatcher = static_cast<SlaveDispatcher*>(lpv))
		{
			return pSlaveDispatcher->ReceiveThread(nullptr);
		}
	}

	return -1;
}

DWORD WINAPI SendThreadWrapper(LPVOID lpv)
{
	if (lpv)
	{
		if (SlaveDispatcher* pSlaveDispatcher = static_cast<SlaveDispatcher*>(lpv))
		{
			return pSlaveDispatcher->SendThread(nullptr);
		}
	}

	return -1;
}

void SlaveDispatcher::CreateDispatcherThreads()
{
	if (this->m_hReceiveThread == INVALID_HANDLE_VALUE)
		this->m_hReceiveThread = CreateThread(0, 0, ReceiveThreadWrapper, this, 0, 0);

	if (this->m_hSendThread == INVALID_HANDLE_VALUE)
		this->m_hSendThread = CreateThread(0, 0, SendThreadWrapper, this, 0, 0);
}

void SlaveDispatcher::SecureQueuePopFront()
{
	this->m_PacketQueueLock.lock();
	{
		this->m_pPacketQueue->pop_front();
	}
	this->m_PacketQueueLock.unlock();
}
