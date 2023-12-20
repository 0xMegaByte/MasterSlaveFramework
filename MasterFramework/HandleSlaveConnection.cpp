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
