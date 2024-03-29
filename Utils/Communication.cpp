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
#include "pch.h"
#pragma comment(lib,"Ws2_32.lib")

MSFPacket::MSFPacket(EPACKET::PacketType packetType, unsigned long ulSlaveId,
	unsigned int unOpCode, unsigned char* pucBuffer)
{
	this->m_epacketType = packetType;
	this->m_ulSlaveId = ulSlaveId;
	this->m_unOpCode = unOpCode;

	ZeroMemory(this->m_ucBuffer, BUF_LEN);

	memcpy_s(this->m_ucBuffer, BUF_LEN, pucBuffer, strnlen_s((const char*)pucBuffer,BUF_LEN));

	DEBUG_PRINT_CLS("Completed\n");
}

MSFPacket::~MSFPacket()
{
	DEBUG_PRINT_CLS("Completed\n");
}

EPACKET::PacketType MSFPacket::getPacketType() const
{
	return this->m_epacketType;
}

unsigned int MSFPacket::getOpCode() const
{
	return this->m_unOpCode;
}

unsigned char* MSFPacket::getBuffer()
{
	return this->m_ucBuffer;
}

unsigned long MSFPacket::getSlaveId() const 
{
	return this->m_ulSlaveId;
}

const char* MSFPacket::PacketTypeToString()
{
	const char* retVal = nullptr;

	switch (this->m_epacketType)
	{
	case EPACKET::PacketType::Acknowledge:
	{
		retVal = "Acknowledge";
		break;
	}
	case EPACKET::PacketType::TaskPacket:
	{
		retVal = "Task";
		break;
	}
	case EPACKET::PacketType::ResponsePacket:
	{
		retVal = "Response";
		break;
	}
	default:
	{
		retVal = "Error";
		break;
	}
	}
	return retVal ? retVal : nullptr;
}

void MSFPacket::PrintPacket()
{
	if (this->m_ucBuffer)
	{
		const char* pcPacketType = PacketTypeToString();

		if (pcPacketType)
		{
			DEBUG_PRINT_CLS("\n\tPacket Type: %s | OpCode: %d | SlaveId: %lu | Buffer: %s\n",
				pcPacketType, this->m_unOpCode, this->m_ulSlaveId, this->m_ucBuffer);
		}
	}
}

//-----------------Packet Dispatcher----------------------

SOCKET PacketDispatcher::GetSocket() const
{
	return this->m_socket;
}

addrinfo* PacketDispatcher::GetService() const 
{
	return this->m_pservice;
}

void PacketDispatcher::SocketWSACleanup()
{
	if (this->m_bWSA)
	{
		if (WSACleanup() == 0)
		{
			this->m_bWSA = false;
			DEBUG_PRINT_CLS("Completed\n");
		}
		else
		{
			DEBUG_PRINT_CLS("Failed\n");
		}
	}
}

void PacketDispatcher::Initialize()
{
	//Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &this->m_wsaData) != 0)
	{
		DEBUG_PRINT_CLS("WSA ERROR: %d", WSA_ERR);
	}
	else
	{
		this->m_bWSA = true;
	}

	DEBUG_PRINT_CLS("Completed\n");
}

void PacketDispatcher::Deinitialize()
{
	if (this->m_socket)
	{
		if (closesocket(this->m_socket) != 0)
		{
			DEBUG_PRINT_CLS("Failed. ESA:%d\n", WSA_ERR);
		}
		else
		{
			DEBUG_PRINT_CLS("Socket closed successfully\n");
		}
	}

	if (this->m_bWSA)
	{
		this->SocketWSACleanup();
	}

	if (this->m_hDispatcherEvent != INVALID_HANDLE_VALUE)
		CloseHandle(this->m_hDispatcherEvent);

	DEBUG_PRINT_CLS("Completed\n");
}

void PacketDispatcher::Start()
{
	this->m_bStart = true;

	//Change event to signaled state
	if (!IsEventStateSignaled(this->m_hDispatcherEvent))
		SetEvent(this->m_hDispatcherEvent);
	DEBUG_PRINT_CLS("Event signaled\n");
}

void PacketDispatcher::Terminate()
{
	//Reset event to non-signaled state
	this->m_bStart = false;

	if (IsEventStateSignaled(this->m_hDispatcherEvent))
		ResetEvent(this->m_hDispatcherEvent);

	DEBUG_PRINT_CLS("Completed\n");
}

bool PacketDispatcher::IsEventStateSignaled(void* hEvent)
{
	return WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0 ? true : false;
}

PacketDispatcher::PacketDispatcher() : m_socket(INVALID_SOCKET),
m_pservice(nullptr)
{
	ZeroMemory(&this->m_wsaData, sizeof(this->m_wsaData));

	this->m_bWSA = false;
	this->m_bStart = false;

	this->m_hDispatcherEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DEBUG_PRINT_CLS("Completed\n");
}

PacketDispatcher::~PacketDispatcher()
{
	if (this->m_socket != INVALID_SOCKET)
	{
		if (shutdown(this->m_socket, SD_BOTH) == 0)
		{
			closesocket(this->m_socket);
		}			
	}

	if (this->m_pservice)
	{
		freeaddrinfo(this->m_pservice);
	}

	if (this->m_bWSA)
	{
		WSACleanup();
		this->m_bWSA = false;
	}

	if (this->m_hDispatcherEvent != INVALID_HANDLE_VALUE)
		CloseHandle(this->m_hDispatcherEvent);

	DEBUG_PRINT_CLS("Completed\n");
}
