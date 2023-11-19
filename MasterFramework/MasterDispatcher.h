#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"

struct Connection
{
	unsigned long m_ulSlaveId = ~0ul;
	SOCKET m_socket = INVALID_SOCKET;
	MSFPacketQueue* m_pSlavePacketQueue = nullptr;
	bool m_bStart = false;
};

class MasterDispatcher : public PacketDispatcher
{
private:
	unsigned long m_ulTotalSlavesCount;
public:

	DWORD WINAPI AcceptConnectionThread(LPVOID lpv);

	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);

	std::vector<Connection> m_vConnections;
	std::mutex m_ConnectionsLock;

	void AddConnection(Connection connection);


	//<slaveId,thread handle>
	std::unordered_map<unsigned long, void*> m_umSlaveThreadHandles;
	std::mutex m_SlaveThreadHandlesLock;

	void AddSlaveThreadHandle(unsigned long ulSlaveId, void* hSlaveThread);

	void IncrementTotalSlaveCount();
	void DecrementTotalSlaveCount();
	unsigned long GetTotalSlaveCount();
	std::mutex m_SlaveCountLock;
	

	MasterDispatcher() : PacketDispatcher(), m_ulTotalSlavesCount(0) {};
	~MasterDispatcher() {};
};

