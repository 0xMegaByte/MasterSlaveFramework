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

Master::Master() : m_pDispatcher(nullptr)
{
	DEBUG_PRINT_CLS("Completed\n");
}

Master::~Master()
{
	DELETE_PTR(this->m_pDispatcher);
	DEBUG_PRINT_CLS("Completed\n");
}

DWORD WINAPI AcceptConnectionsThreadWrapper(LPVOID lpv)
{
	if (MasterDispatcher* pMasterDispatcher = static_cast<MasterDispatcher*>(lpv))
	{
		pMasterDispatcher->AcceptConnections(nullptr);
	}

	return 0;
}

void Master::CreateDispatcher()
{
	if (!this->m_pDispatcher)
	{
		if (this->m_pDispatcher = new MasterDispatcher())
		{
			DEBUG_PRINT_CLS("Dispatcher created successfully\n");
		}
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
