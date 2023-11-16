#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"

class MasterDispatcher : public PacketDispatcher
{
public:

	DWORD WINAPI SendThread(LPVOID lpv) override;
	DWORD WINAPI ReceiveThread(LPVOID lpv) override;
	DWORD WINAPI AcceptConnectionThread(LPVOID lpv);

	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);


	struct Connection
	{
		unsigned long m_ulSlaveId;
		SOCKET m_socket;
		unsigned short m_usPort;
		unsigned long m_ulIpAddress;
	};
	//<SlaveThreadId, Connection{}>
	std::unordered_map <unsigned long, Connection> m_umConnectionMap;
	std::mutex m_ConnectionMapLock;



	MasterDispatcher() : PacketDispatcher() {};
	~MasterDispatcher() {};
};

