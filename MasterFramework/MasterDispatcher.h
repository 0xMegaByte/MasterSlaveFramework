/*
Copyright (C) 2023 Matan Shitrit (0xMegaByte)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
	HANDLE m_hAcceptConnections;
	DWORD WINAPI AcceptConnections(LPVOID lpv);

	HANDLE m_hMonitorConnections;
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

	MasterDispatcher();
	~MasterDispatcher() {};
};

//Create a graceful / semi-graceful shutdown