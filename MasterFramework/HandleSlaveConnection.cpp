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

unsigned long SlaveConnection::GetConnectionId()
{
	return this->m_ulSlaveConnectionId;
}

SOCKET SlaveConnection::GetSocket()
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

bool SlaveConnection::GetStartFlag()
{
	return this->m_bStart;
}

void SlaveConnection::SetStartFlag(bool bStart)
{
	this->m_bStart = bStart;
}

bool SlaveConnection::GetThreadFinishedFlag()
{
	return this->m_bIsThreadFinished;
}

void SlaveConnection::SetThreadFinishedFlag(bool bFinished)
{
	this->m_bIsThreadFinished = bFinished;
}

SlaveConnection::SlaveConnection(unsigned long ulSlaveConnectionId,
	SOCKET socket, bool bStart):
	m_ulSlaveConnectionId(ulSlaveConnectionId),
	m_socket(socket), m_bStart(bStart)
{
	this->m_pSlavePacketQueue = new MSFPacketQueue();
	this->m_pPacketQueueLock = new std::mutex();
	this->m_bIsThreadFinished = false;
}

SlaveConnection::~SlaveConnection()
{
	//TODO: Clear packet queue gracefully
	//TODO: Close socket
	DELETE_PTR(this->m_pPacketQueueLock);
}
