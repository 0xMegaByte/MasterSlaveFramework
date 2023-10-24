#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

enum class MSF_PACKET_TYPE
{
	MSF_TASK_PACKET = 0,
	MSF_RESP_PACKET
};

class MSF_Packet //Not socket but part of the socket
{
private:
	MSF_PACKET_TYPE m_packetType;
	unsigned long m_ulOpCode;		//Task or Response
	unsigned long m_ulSlaveId;		//Slave Id recv/send

	unsigned char m_ucBuffer[512]{ 0 }; //Params
public:
	MSF_Packet(MSF_PACKET_TYPE packetType, unsigned long ulSlaveId,
		unsigned long ulOpCode, unsigned char* pucBuffer);
	~MSF_Packet();
};


class PACKETS_OPCODE
{
	//Communication setup packet
	enum PCK_COMM
	{
		MASTER_SLAVE_INIT_CONNECTION = 0,
		MASTER_SLAVE_DEINIT_CONNECTION,
		SLAVE_MASTER_CONNECTION_RESPONSE,
	};

	enum PCK_TASK
	{
		//TASKs enum
		TASK_BEEP = 0,
		TASK_OPEN_CMD

		//Enter here more slave's tasks ...

	};

	enum PCK_RESP
	{
		SLAVE_MASTER_OK_RESPONSE = 0
	};

};


class PacketDispatcher
{
private:
	WSADATA m_wsaData;

protected:
	bool bWSA;
	SOCKET m_socket;
	sockaddr_in m_service;

public:

	void SocketSetup(const char* pcIpAddress, const unsigned short usPort);


	//Pure virtual function: Impl. in server and client
	virtual unsigned long __stdcall DispatcherThread(void* lpv) = 0;

	void Initialize();
	void Deinitialize();


	SOCKET GetSocket();
	sockaddr_in GetService();

	PacketDispatcher();
	~PacketDispatcher();

	//Address (INADDR_ANY) any available IP for server
	//(Address + port)'s server (for client)



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

};
