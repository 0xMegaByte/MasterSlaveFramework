#include "pch.h"
#pragma comment(lib,"Ws2_32.lib")

MSFPacket::MSFPacket(EPACKET::TYPE packetType, unsigned long ulSlaveId,
	unsigned long ulOpCode, unsigned char* pucBuffer)
{
	this->m_epacketType = packetType;
	this->m_ulSlaveId = ulSlaveId;
	this->m_ulOpCode = ulOpCode;

	memcpy_s(this->m_ucBuffer, BUF_LEN, pucBuffer, sizeof(pucBuffer));


}

MSFPacket::~MSFPacket()
{
}

//void PacketDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
//{
//	this->m_service.sin_family = AF_INET;
//	this->m_service.sin_port = usPort;
//	inet_pton(AF_INET, pcIpAddress, &this->m_service.sin_addr.S_un.S_addr);
//}

SOCKET PacketDispatcher::GetSocket()
{
	return this->m_socket;
}

addrinfo* PacketDispatcher::GetService()
{
	return this->m_pservice;
}

MSFPacketQueue* PacketDispatcher::GetPacketQueue()
{
	return this->m_pPacketQueue;
}

void PacketDispatcher::SocketWSACleanup()
{
	if (this->bWSA)
	{
		WSACleanup();
		this->bWSA = false;
		DEBUG_PRINT("Completed\n");
	}
}

void PacketDispatcher::Initialize()
{
	//Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &this->m_wsaData) != 0)
	{
		DEBUG_PRINT("WSA ERROR: %d", WSA_ERR);
	}

	if (!this->m_pPacketQueue)
	{
		this->m_pPacketQueue = new MSFPacketQueue(); //TODO: Destroyer
		if (this->m_pPacketQueue)
			DEBUG_PRINT("Packet queue created\n");

	}

	////Create Event
	//if (this->m_hDispatcherEvent = INVALID_HANDLE_VALUE)
	//	this->m_hDispatcherEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	bWSA = true;

	DEBUG_PRINT("Completed\n");
}

void PacketDispatcher::Deinitialize()
{
	if (this->m_socket)
		closesocket(this->m_socket);
	if (bWSA)
	{
		this->SocketWSACleanup();
	}

	//Create a Dispatcher non-signaled state even to trigger in the dispatcher thread
	if (this->m_hDispatcherEvent != INVALID_HANDLE_VALUE)
		CloseHandle(this->m_hDispatcherEvent);

	DELETE_PTR(this->m_pPacketQueue);
}

void PacketDispatcher::Start()
{
	bStart = true;
	//Change event to signaled state
	if (!IsEventStateSignaled(this->m_hDispatcherEvent))
		SetEvent(this->m_hDispatcherEvent);
}

void PacketDispatcher::Terminate()
{
	//Reset event to non-signaled state
	this->bStart = false;

	if (IsEventStateSignaled(this->m_hDispatcherEvent))
		ResetEvent(this->m_hDispatcherEvent);
}

bool PacketDispatcher::IsEventStateSignaled(void* hEvent)
{
	return WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0 ? true : false;
}

PacketDispatcher::PacketDispatcher() : m_socket(INVALID_SOCKET),
m_pPacketQueue(nullptr), m_pservice(nullptr)
{
	//ZeroMemory(&this->m_pservice, sizeof(this->m_pservice));
	ZeroMemory(&this->m_wsaData, sizeof(this->m_wsaData));

	this->bWSA = false;
	this->bStart = false;

	this->m_hDispatcherEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PacketDispatcher::~PacketDispatcher()
{
}
