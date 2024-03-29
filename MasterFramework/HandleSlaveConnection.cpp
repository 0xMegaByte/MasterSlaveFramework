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
#include "HandleSlaveConnection.h"

unsigned long SlaveConnection::GetConnectionId() const
{
	return this->m_ulSlaveConnectionId;
}

SOCKET SlaveConnection::GetSocket() const
{
	return this->m_socket;
}

MSFPacketQueue* SlaveConnection::GetMSFPacketQueue()
{
	return this->m_pSlavePacketQueue;
}

std::mutex* SlaveConnection::GetMSFPacketQueueLock()
{
	return this->m_pPacketQueueLock;
}

bool SlaveConnection::GetStartFlag() const 
{
	return this->m_bStart;
}

void SlaveConnection::SetStartFlag(bool bStart)
{
	this->m_bStart = bStart;
}

bool SlaveConnection::GetThreadFinishedFlag() const
{
	return this->m_bIsThreadFinished;
}

void SlaveConnection::SetThreadFinishedFlag(bool bFinished)
{
	this->m_bIsThreadFinished = bFinished;
}

SlaveConnection::SlaveConnection(unsigned long ulSlaveConnectionId,
	SOCKET socket, bool bStart) :
	m_ulSlaveConnectionId(ulSlaveConnectionId),
	m_socket(socket), m_bStart(bStart)
{
	this->m_pSlavePacketQueue = new MSFPacketQueue();
	this->m_pPacketQueueLock = new std::mutex();
	this->m_bIsThreadFinished = false;
}

SlaveConnection::~SlaveConnection()
{
	//Empty and gracefully delete all packets from the deque
	if (this->m_pSlavePacketQueue)
	{
		MSFPacketQueue& pSlavePacketQueue = *this->m_pSlavePacketQueue;

		if (this->m_pPacketQueueLock)
		{
			this->m_pPacketQueueLock->lock();

			while (!pSlavePacketQueue.empty())
			{
				MSFPacket* pFront = pSlavePacketQueue.front();
				DELETE_PTR(pFront);
				pSlavePacketQueue.pop_front();
			}

			this->m_pPacketQueueLock->unlock();

			DELETE_PTR(this->m_pPacketQueueLock);
		}

		DELETE_PTR(this->m_pSlavePacketQueue);
	}

	if (this->m_socket != INVALID_SOCKET)
		closesocket(this->m_socket);
}
