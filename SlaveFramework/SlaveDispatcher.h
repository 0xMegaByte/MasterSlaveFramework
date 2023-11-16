#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
class SlaveDispatcher : public PacketDispatcher
{
private:
	bool m_bConnected = false;
public:

	

	DWORD WINAPI SendThread(LPVOID lpv) override;
	DWORD WINAPI ReceiveThread(LPVOID lpv) override;
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);

	void Connect();
	bool IsDispatcherConnected();

	SlaveDispatcher() : PacketDispatcher() {};
	~SlaveDispatcher() {};
};

