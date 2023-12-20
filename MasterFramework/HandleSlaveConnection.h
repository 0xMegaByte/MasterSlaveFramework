#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"

class SlaveConnection
{
private:
	unsigned long m_ulSlaveConnectionId;
	SOCKET m_socket = INVALID_SOCKET;
	MSFPacketQueue* m_pSlavePacketQueue = nullptr;
	std::mutex* m_pPacketQueueLock;
	bool m_bStart;
	bool m_bIsThreadFinished;

public:
	
	unsigned long GetConnectionId();
	//void SetConnectionId(unsigned long ulSlaveConnectionId);

	SOCKET GetSocket();
	//void SetSocket(SOCKET socket);

	MSFPacketQueue* GetMSFPacketQueue();
	//void SetMSFPacketQueue(MSFPacketQueue* pPacketQueue);

	std::mutex* GetMSFPacketQueueLock();
	//void SetMSFPacketQueueLock(std::mutex Lock);


	bool GetStartFlag();
	void SetStartFlag(bool bStart);

	bool GetThreadFinishedFlag();
	void SetThreadFinishedFlag(bool bFinished);



	SlaveConnection(unsigned long ulSlaveConnectionId,
		SOCKET socket, bool bStart = false);
	~SlaveConnection();

};

