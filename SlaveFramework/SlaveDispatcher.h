#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
class SlaveDispatcher : public PacketDispatcher
{
private:
	bool m_bConnected;
	MSFPacketQueue* m_pPacketQueue;
public:

	DWORD WINAPI SendThread(LPVOID lpv);
	DWORD WINAPI ReceiveThread(LPVOID lpv);
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort) override;

	void Connect();
	bool IsDispatcherConnected();

	
	std::mutex m_PacketQueueLock;

	void SecureQueuePopFront();
	void SecureQueuePushBack(MSFPacket* pPacket);

	SlaveDispatcher() : PacketDispatcher(), m_bConnected(false)
	{
		this->m_pPacketQueue = new MSFPacketQueue();
	};
	~SlaveDispatcher() {};
};

