// MasterFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Master.h"

int main()
{
	//Create master
	Master* pMaster = new Master();

	if (pMaster)
	{
		//Establish Master-Slave
		pMaster->CreateDispatcher();

		MasterDispatcher* pmd = (pMaster)->GetDispatcher();
		if (pmd)
		{
			pmd->SocketSetup(nullptr, 6969);
			pmd->Initialize();
			pmd->Start(); //Start Dispatcher Thread

			//Create packets out of command or any interface
			//pMaster->CreatePacket()->QueuePush()
			MSFPacket* Packet = new MSFPacket(EPACKET::TYPE::MSF_TASK_PACKET, EPACKET::CMD::TASK::TASK_BEEP, 1, (unsigned char*)"");

			MSFPacketQueue* ppq = pmd->GetPacketQueue();

			if (ppq)
			{
				ppq->push_back(*Packet);
				//Check Dispatcher Thread to handle the new packet

			}

			//TODO: USE DATABASE TO REMEMBER WHICH SLAVE HAS WHAT IP



			//TODO: MANAGE THE RECIEVE SIDE MASTER AND SLAVE

			while (true)
			{
				//LOOP UNTIL TERMINATION
			}

			//DELETE_PTR(pmd);
		}
	}

	

}