// MasterSlaveFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Slave.h"

int main()
{
	//Create slave
	Slave* pSlave = new Slave(1);

	if (pSlave)
	{
		//Establish Slave-Master connection
		pSlave->CreateDispatcher();

		SlaveDispatcher* psd = (pSlave)->GetDispatcher();
		if (psd)
		{
			psd->Initialize();
			psd->SocketSetup("127.0.0.1", 6969);
			psd->Start();
			//Create tasks out of packets

			if (MSFPacketQueue* ppq = psd->GetPacketQueue())
			{

			}


			//Execture tasks

			/*pSlave->PushTask(new Task(1001));
			Task* pTask = pSlave->PopTask();
			printf("Task Id: %lu", pTask->GetTaskId());*/

			//Report tasks results
			while (true)
			{

			}

			//DELETE_PTR(psd);
		}
	}


	return 0;
}