#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
class SlaveDispatcher : public PacketDispatcher
{
public:
	DWORD WINAPI DispatcherThread(LPVOID lpv) override;
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);

	SlaveDispatcher() : PacketDispatcher() {};
	~SlaveDispatcher() {};
};

