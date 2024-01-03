/*
Copyright (C) 2023 Matan Shitrit (0xMegaByte)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
			pMasterDisptacher->Start(); //Start Dispatcher Threads

			DEBUG_PRINT("Master disptacher initialized\n");   

			while (true)
			{
				//LOOP UNTIL TERMINATION
			}

			DELETE_PTR(pMasterDisptacher);
		}

		DELETE_PTR(pMaster);
	}
}	