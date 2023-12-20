#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
#include "HandleSlaveConnection.h"
//#define STORE_SLAVES_THREAD_HANDLE

typedef  unsigned long SLAVE_ID;
typedef  void* PSLAVE_THREAD_HANDLE;

class MasterDispatcher : public PacketDispatcher
{
private:
	unsigned long m_ulTotalSlavesCount;
public:

	DWORD WINAPI AcceptConnectionThread(LPVOID lpv);
	DWORD WINAPI MonitorConnections(LPVOID lpv);

	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);

	std::unordered_map<SLAVE_ID,SlaveConnection*> m_umConnections;
	std::mutex m_ConnectionsLock;

	void AddConnection(SLAVE_ID ulSlaveId, SlaveConnection* pSlaveConnection);

#ifdef STORE_SLAVES_THREAD_HANDLE
	//NOTE: You may store slave's thread handle outside Connection
	//If there is any need, implement it.
	std::unordered_map<SLAVE_ID, PSLAVE_THREAD_HANDLE> m_umSlaveThreadHandles;
	std::mutex m_SlaveThreadHandlesLock;
	void AddSlaveThreadHandle(unsigned long ulSlaveId, void* hSlaveThread);
#endif

	void IncrementTotalSlaveCount();
	void DecrementTotalSlaveCount();
	unsigned long GetTotalSlaveCount();
	std::mutex m_SlaveCountLock;


	MasterDispatcher() : PacketDispatcher(), m_ulTotalSlavesCount(0) {};
	~MasterDispatcher() {};
};

//TODO: How to cross examine SlaveThreadHandles and Connections vector
//Create a graceful / semi-graceful shutdown