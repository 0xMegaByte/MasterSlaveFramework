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

class SlaveDispatcher : public PacketDispatcher
{
private:
	bool m_bConnected;
	void* m_pTaskExecutor;
	MSFPacketQueue* m_pPacketQueue;

	void* m_hSendThread;
	void* m_hReceiveThread;
public:

	DWORD WINAPI SendThread(LPVOID lpv);
	DWORD WINAPI ReceiveThread(LPVOID lpv);
	void SocketSetup(const char* pcIpAddress, const unsigned short usPort) override;

	void Connect();
	void AssignTaskExecutor(void* pTaskExecutor);
	void* GetTaskExecutor();
	bool IsDispatcherConnected();

	std::mutex m_PacketQueueLock;

	void SecureQueuePopFront();
	void SecureQueuePushBack(MSFPacket* pPacket);

	SlaveDispatcher();
	~SlaveDispatcher() {};

	void CreateDispatcherThreads();
};

