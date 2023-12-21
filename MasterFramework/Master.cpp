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
#include "Master.h"

Master::Master() :
	m_pDispatcher(nullptr), m_hAcceptConnections(INVALID_HANDLE_VALUE)
{
	DEBUG_PRINT_CLS("Completed\n");
}

Master::~Master()
{
	//TODO: Valid?
	if (this->m_hAcceptConnections != INVALID_HANDLE_VALUE)
	{
		if (GetHandleInformation(this->m_hAcceptConnections, nullptr))
			CloseHandle(this->m_hAcceptConnections);
	}

	DELETE_PTR(this->m_pDispatcher);
	DEBUG_PRINT_CLS("Completed\n");
}

DWORD WINAPI AcceptConnectionsThreadWrapper(LPVOID lpv)
{
	if (MasterDispatcher* pMasterDispatcher = static_cast<MasterDispatcher*>(lpv))
	{
		pMasterDispatcher->AcceptConnectionThread(nullptr);
	}

	return 0;
}

void Master::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		if (this->m_pDispatcher = new MasterDispatcher())
			DEBUG_PRINT_CLS("Dispatcher created successfully\n");


	//Create Dispatcher Thread
	//BUG: Move to the dispatcher itself when init.
	//thread will run but will be stopped until event signaled
	if (this->m_hAcceptConnections == INVALID_HANDLE_VALUE)
	{
		this->m_hAcceptConnections = CreateThread(0, 0, AcceptConnectionsThreadWrapper, this->m_pDispatcher, 0, 0);
		if (this->m_hAcceptConnections)
			DEBUG_PRINT_CLS("Thread created (Handle @0x%p)\n", this->m_hAcceptConnections);
	}

	DEBUG_PRINT("Completed\n");
}

void Master::DestroyDispatcher()
{
	DELETE_PTR(this->m_pDispatcher);
}

MasterDispatcher* Master::GetDispatcher()
{
	return this->m_pDispatcher ? this->m_pDispatcher : nullptr;
}
