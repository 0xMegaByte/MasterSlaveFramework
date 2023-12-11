#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"

class SlaveDispatcher : public PacketDispatcher
{
private:
	bool m_bConnected;
	MSFPacketQueue* m_pPacketQueue;

	void* m_hSendThread;
	void* m_hReceiveThread;
public:

	DWORD WINAPI SendThread(LPVOID lpv);
	DWORD WINAPI ReceiveThread(LPVOID lpv);
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort) override;

	void Connect();
	bool IsDispatcherConnected();

	
	std::mutex m_PacketQueueLock;

	void SecureQueuePopFront();
	void SecureQueuePushBack(MSFPacket* pPacket);

	SlaveDispatcher() : PacketDispatcher(), m_bConnected(false),
		m_hReceiveThread(INVALID_HANDLE_VALUE), m_hSendThread(INVALID_HANDLE_VALUE)
	{
		this->m_pPacketQueue = new MSFPacketQueue();
	};
	~SlaveDispatcher() {};

	void CreateDispatcherThreads();
};

