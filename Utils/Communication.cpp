#include "pch.h"
#pragma comment(lib,"Ws2_32.lib")

MSFPacket::MSFPacket(EPACKET::TYPE packetType, unsigned long ulSlaveId,
	unsigned long ulOpCode, unsigned char* pucBuffer)
{
	this->m_epacketType = packetType;
	this->m_ulSlaveId = ulSlaveId;
	this->m_ulOpCode = ulOpCode;

	memcpy_s(this->m_ucBuffer, BUF_LEN, pucBuffer, BUF_LEN);


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

sockaddr_in PacketDispatcher::GetService()
{
	return this->m_service;
}

MSFPacketQueue* PacketDispatcher::GetPacketQueue()
{
	return this->m_pPacketQueue;
}

void PacketDispatcher::Initialize()
{
	//Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &this->m_wsaData) == 0)
	{
		//Create a socket
		this->m_socket = socket(AF_INET, SOCK_STREAM, SOCK_STREAM /*TCP*/);
	}

	if (!this->m_pPacketQueue)
		this->m_pPacketQueue = new MSFPacketQueue(); //TODO: Destroyer

	////Create Event
	//if (this->m_hDispatcherEvent = INVALID_HANDLE_VALUE)
	//	this->m_hDispatcherEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	bWSA = true;
}

void PacketDispatcher::Deinitialize()
{
	if (this->m_socket)
		closesocket(this->m_socket);
	if (bWSA)
	{
		WSACleanup();
		bWSA = false;
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
m_pPacketQueue(nullptr)
{
	ZeroMemory(&this->m_service, sizeof(this->m_service));
	ZeroMemory(&this->m_wsaData, sizeof(this->m_wsaData));

	this->bWSA = false;
	this->bStart = false;

	this->m_hDispatcherEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PacketDispatcher::~PacketDispatcher()
{
}
