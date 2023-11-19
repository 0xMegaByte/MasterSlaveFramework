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
		DEBUG_PRINT("Slave started\n");
		//Establish Slave-Master connection
		pSlave->CreateDispatcher();

		SlaveDispatcher* psd = (pSlave)->GetDispatcher();
		if (psd)
		{
			psd->Initialize();
			psd->SocketSetup("127.0.0.1", 6969);
			psd->Connect();

			if (psd->IsDispatcherConnected())
			{
				psd->Start();

				DEBUG_PRINT("Slave disptacher Started\n");
			}

			while (true)
			{
				//LOOP UNTIL TERMINATION for tests
			}

			//DELETE_PTR(psd);
		}
	}


	return 0;
}