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

class SlaveConnection
{
private:
	unsigned long m_ulSlaveConnectionId;
	SOCKET m_socket = INVALID_SOCKET;
	MSFPacketQueue* m_pSlavePacketQueue = nullptr;
	std::mutex* m_pPacketQueueLock;
	bool m_bStart;
	bool m_bIsThreadFinished;

public:
	
	unsigned long GetConnectionId() const;

	SOCKET GetSocket() const;

	MSFPacketQueue* GetMSFPacketQueue();
	std::mutex* GetMSFPacketQueueLock();

	bool GetStartFlag() const;
	void SetStartFlag(bool bStart);

	bool GetThreadFinishedFlag() const;
	void SetThreadFinishedFlag(bool bFinished);

	SlaveConnection(unsigned long ulSlaveConnectionId,
		SOCKET socket, bool bStart = false);
	~SlaveConnection();

};

