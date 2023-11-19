#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
class SlaveDispatcher : public PacketDispatcher
{
private:
	bool m_bConnected;
public:

	DWORD WINAPI SendThread(LPVOID lpv);
	DWORD WINAPI ReceiveThread(LPVOID lpv);
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort) override;

	void Connect();
	bool IsDispatcherConnected();

	MSFPacketQueue* m_pPacketQueue;

	SlaveDispatcher() : PacketDispatcher(), m_bConnected(false)
	{
		this->m_pPacketQueue = new MSFPacketQueue();
	};
	~SlaveDispatcher() {};
};

