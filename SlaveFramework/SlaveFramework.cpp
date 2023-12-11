// MasterSlaveFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Slave.h"

int main()
{
	//TODO: refactor code to be more prod like
	PrintWelcomeMessage();
	//Create slave
	Slave* pSlave = new Slave(1);

	if (pSlave)
	{
		DEBUG_PRINT("Slave started\n");
		//Establish Slave-Master connection
		pSlave->CreateDispatcher();
		pSlave->CreateTaskExecutor();
		SlaveDispatcher* psd = pSlave->GetDispatcher();
		TaskExecutor* pte = pSlave->GetTaskExecutor();
		if (psd && pte)
		{
			psd->Initialize();
			psd->SocketSetup("127.0.0.1", 6969);
			psd->Connect(pte);

			if (psd->IsDispatcherConnected())
			{
				psd->Start();
			}

			pte->MakeTaskCallbacks();

			while (true)
			{
				pte->ExecuteTasks();
				Sleep(1000);
				//LOOP UNTIL TERMINATION for tests
			}

			//DELETE_PTR(psd);
		}
	}

	return 0;
}