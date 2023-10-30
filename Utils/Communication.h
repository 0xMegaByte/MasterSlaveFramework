#pragma once
#include "Utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#define WSA_ERR WSAGetLastError()



//enum class MSF_PACKET_TYPE
//{
//	MSF_TASK_PACKET = 0,
//	MSF_RESP_PACKET
//};



namespace EPACKET
{
	enum class TYPE
	{
		MSF_TASK_PACKET = 0,
		MSF_RESP_PACKET
	};

	//_OPCODE

	class CMD
	{
	public:
		//Communication setup packet
		enum COMM
		{
			MASTER_SLAVE_INIT_CONNECTION = 1000,
			MASTER_SLAVE_DEINIT_CONNECTION,
			SLAVE_MASTER_CONNECTION_RESPONSE,
		};

		enum TASK
		{
			//TASKs enum
			TASK_BEEP = 2000,
			TASK_OPEN_CMD

			//Enter here more slave's tasks ...

		};
	};

	class RESP
	{
	public:
		enum CODE
		{
			SLAVE_MASTER_OK_RESPONSE = 3000
		};
	};

};

class MSFPacket						//Not socket but part of the socket
{
private:
	EPACKET::TYPE  m_epacketType;
	unsigned long m_ulOpCode;			//Task or Response
	unsigned long m_ulSlaveId;			//Slave Id recv/send

	unsigned char m_ucBuffer[512]{ 0 }; //Params

public:
	MSFPacket(EPACKET::TYPE packetType, unsigned long ulSlaveId,
		unsigned long ulOpCode, unsigned char* pucBuffer);
	~MSFPacket();
};


//class PacketBulilder
//{
//
//public:
//
//
//
//	PacketBulilder();
//	~PacketBulilder();
//};

typedef std::deque<MSFPacket> MSFPacketQueue;

class PacketDispatcher
{

private:
	WSADATA m_wsaData;

protected:
	bool bWSA;
	bool bStart;
	SOCKET m_socket;
	sockaddr_in m_service;

	void* m_hDispatcherEvent;

	//MSFPacket Queue
	MSFPacketQueue* m_pPacketQueue;


public:

	//Initiate service with ip-port
	virtual void SocketSetup(const char* pcIpAddress, const unsigned short usPort) = 0;

	virtual DWORD WINAPI DispatcherThread(LPVOID lpv) = 0; //Pure virtual func. to impl. in slave & master

	void Initialize();		//Setup WSA and initialize m_hDispatcherEvent handle
	void Deinitialize();	//Close socket, Cleanup WSA and Close m_hDispatcherEvent handle

	void Start();			//Set m_hDispatcherEvent to signaled state
	void Terminate();		//bTerminate set to true & set m_hDispatcherEvent to non-signaled state

	bool IsEventStateSignaled(void* hEvent);



	SOCKET GetSocket();			//Returns sockets
	sockaddr_in GetService();	//Returns service

	MSFPacketQueue* GetPacketQueue();

	PacketDispatcher();
	~PacketDispatcher();

	//Address (INADDR_ANY) any available IP for server
	//(Address + port)'s server (for client)


	/*
	// De/Init Dispatcher	- 1 time > [Server and client] (ctor)
	// --> WSAStartup, Create socket
	//
	// setup ip port
	//
	// Start Dispatcher		- 1 time >	Start Dispatcher Thread (Event signaling)
	// ==> Server Thread
	//	--> Bind
	//	--> Listen for incoming connections
	//	--> Accept clients' socket
	//	--> Close
	//
	// ==> Client Thread
	//	--> Connect to server
	//	--> Send/Recv (n Times, until termination packet)
	//	--> Close

	// Stop Distpatcher		- 1 time >	Stop Dispatcher Thread (Event signaling)

	// get; set; port		- 1 time
	// get; set; ip			- 1 time
	//
	*/
};

