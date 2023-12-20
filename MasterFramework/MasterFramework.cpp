// MasterFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Master.h"

int main()
{
	PrintWelcomeMessage();
	//Create master
	Master* pMaster = new Master();

	if (pMaster)
	{
		DEBUG_PRINT("Master started\n");

		//Establish Master-Slave
		pMaster->CreateDispatcher();

		MasterDispatcher* pMasterDisptacher = (pMaster)->GetDispatcher();
		if (pMasterDisptacher)
		{
			pMasterDisptacher->Initialize();
			pMasterDisptacher->SocketSetup(nullptr, 6969);
			pMasterDisptacher->Start(); //Start Dispatcher Thread

			DEBUG_PRINT("Master disptacher initialized\n");

			//TODO: USE DATABASE TO STORE WHICH SLAVE HAS WHAT IP


			while (true)
			{
				//LOOP UNTIL TERMINATION
			}

			DELETE_PTR(pMasterDisptacher);
		}

		DELETE_PTR(pMaster);
	}
}	