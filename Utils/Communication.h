#pragma once
#include "Utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#define WSA_ERR WSAGetLastError()

namespace EPACKET {
	// Enum class for packet types
	enum class PacketType {
		TaskPacket = 0,
		ResponsePacket,
		Acknowledge
	};

	namespace CMD {
		// Communication setup packet
		constexpr int MASTER_SLAVE_ACK_CONNECTION = 1000;
		//constexpr int MASTER_SLAVE_DEINIT_CONNECTION = 1001;
		constexpr int SLAVE_MASTER_ACK_CONNECTION_RESPONSE = 1002; //response token?

		// Tasks enum
		constexpr int TASK_BEEP = 2000;
		constexpr int TASK_OPEN_CMD = 2001;

		// Add more slave's tasks here ...
	}

	namespace RESP {
		// Response codes
		constexpr int SLAVE_MASTER_OK_RESPONSE = 3000;
	};
}



#define MSFPACKET_SIZE sizeof(MSFPacket) //524ui64

class MSFPacket						//Not socket but part of the socket
{
private:
	EPACKET::PacketType  m_epacketType;
	unsigned long m_ulOpCode;			//Task or Response
	unsigned long m_ulSlaveId;			//Slave Id recv/send

	unsigned char m_ucBuffer[BUF_LEN]; //Params

public:
	MSFPacket(EPACKET::PacketType packetType, unsigned long ulSlaveId,
		unsigned long ulOpCode, unsigned char* pucBuffer);
	~MSFPacket();

	EPACKET::PacketType getPacketType();
	unsigned long getOpCode();
	unsigned char* getBuffer();
	unsigned long getSlaveId();

	const char* PacketTypeToString();

	void PrintPacket();
};

typedef std::deque<MSFPacket*> MSFPacketQueue; //TOOD: Maybe create a class where each PQ has an internal/external lock?

class PacketDispatcher
{

private:
	WSADATA m_wsaData;

protected:
	bool m_bWSA;
	bool m_bStart;

	SOCKET m_socket;
	addrinfo* m_pservice;

	void* m_hDispatcherEvent;
public:

	virtual void SocketSetup(const char* pcIpAddress, const unsigned short usPort) = 0;

	void Initialize();		//Setup WSA and initialize m_hDispatcherEvent handle
	void Deinitialize();	//Close socket, Cleanup WSA and Close m_hDispatcherEvent handle

	void Start();			//Set m_hDispatcherEvent to signaled state
	void Terminate();		//bTerminate set to true & set m_hDispatcherEvent to non-signaled state

	bool IsEventStateSignaled(void* hEvent);


	SOCKET GetSocket();			//Returns sockets
	addrinfo* GetService();		//Returns service

	//MSFPacketQueue* GetPacketQueue();

	void SocketWSACleanup();

	PacketDispatcher();
	~PacketDispatcher();
};

