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
		DEBUG_PRINT("Master started\n");

		//Establish Master-Slave
		pMaster->CreateDispatcher();

		MasterDispatcher* pmd = (pMaster)->GetDispatcher();
		if (pmd)
		{
			pmd->Initialize();
			pmd->SocketSetup(nullptr, 6969);
			pmd->Start(); //Start Dispatcher Thread

			DEBUG_PRINT("Master disptacher initialized\n");

			//Create packets out of command or any interface
			//pMaster->CreatePacket()->QueuePush()
			MSFPacket* pPacket = new MSFPacket(EPACKET::TYPE::MSF_TASK_PACKET, 1, EPACKET::CMD::TASK::TASK_BEEP, (unsigned char*)"Test");
			MSFPacketQueue* ppq = pmd->GetPacketQueue();

			if (ppq)
			{
				ppq->push_back(pPacket);
				//Check Dispatcher Thread to handle the new packet

			}

			//TODO: USE DATABASE TO STORE WHICH SLAVE HAS WHAT IP


			while (true)
			{
				//LOOP UNTIL TERMINATION
			}

			//DELETE_PTR(pmd);
		}
	}

	

}	