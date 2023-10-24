#include "pch.h"

MSF_Packet::MSF_Packet(MSF_PACKET_TYPE packetType, unsigned long ulSlaveId,
	unsigned long ulOpCode, unsigned char* pucBuffer)
{
	this->m_packetType = packetType;
	this->m_ulSlaveId = ulSlaveId;
	this->m_ulOpCode = ulOpCode;

	memcpy_s(&this->m_ucBuffer, BUF_LEN, pucBuffer, BUF_LEN);


}

MSF_Packet::~MSF_Packet()
{
}

void PacketDispatcher::SocketSetup(const char* pcIpAddress, const unsigned short usPort)
{
	this->m_service.sin_family = AF_INET;
	this->m_service.sin_port = usPort;
	inet_pton(AF_INET, pcIpAddress, &this->m_service.sin_addr.S_un.S_addr); 
}

SOCKET PacketDispatcher::GetSocket()
{
	return this->m_socket;
}

sockaddr_in PacketDispatcher::GetService()
{
	return this->m_service;
}

void PacketDispatcher::Initialize()
{
	//Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &this->m_wsaData) == 0)
	{
		bWSA = true;
		//Create a socket
		this->m_socket = socket(AF_INET, SOCK_STREAM, SOCK_STREAM /*TCP*/);
	}
}

void PacketDispatcher::Deinitialize()
{
	if (this->m_socket)
		closesocket(this->m_socket);
	if(bWSA)
		WSACleanup(); //TODO: Add an indicator that WSA started up
}

PacketDispatcher::PacketDispatcher()
{
	this->m_socket = INVALID_SOCKET;
	ZeroMemory(&this->m_service, sizeof(sockaddr_in));
	ZeroMemory(&this->m_wsaData, sizeof(m_wsaData));
	this->bWSA = false;
}

PacketDispatcher::~PacketDispatcher()
{
}
