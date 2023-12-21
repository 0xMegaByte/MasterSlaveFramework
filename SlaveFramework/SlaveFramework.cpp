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
#include "Slave.h"

int main()
{
	PrintWelcomeMessage();

	//Create slave
	if (Slave* pSlave = new Slave(1);)
	{
		DEBUG_PRINT("Slave started\n");
		//Establish Slave-Master connection
		pSlave->CreateDispatcher();
		pSlave->CreateTaskExecutor();
		SlaveDispatcher* pSlaveDispatcher = pSlave->GetDispatcher();
		TaskExecutor* pTaskExecutor = pSlave->GetTaskExecutor();

		if (!pSlaveDispatcher)
		{
			DEBUG_PRINT("Failed to create dispatcher!\n Exiting..");
			return -1;
		}

		if (!pTaskExecutor)
		{
			DEBUG_PRINT("Failed to create Task Executor!\n Exiting..");
			return -1;
		}

		pSlaveDispatcher->Initialize();
		pSlaveDispatcher->SocketSetup("127.0.0.1", 6969);

		//TODO: Fix this awful abstraction
		pSlaveDispatcher->Connect(pTaskExecutor);

		if (pSlaveDispatcher->IsDispatcherConnected())
		{
			pSlaveDispatcher->Start();
		}

		pTaskExecutor->MakeTaskCallbacks();

		//note: could be fixed with its own thread
		while (true)
		{
			pTaskExecutor->ExecuteTasks();
			Sleep(1000);
		}

		DELETE_PTR(pTaskExecutor);
		DELETE_PTR(pSlaveDispatcher);
		DELETE_PTR(pSlave);
	}

	return 0;
}