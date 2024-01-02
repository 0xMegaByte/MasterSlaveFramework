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

unsigned long Slave::GetSlaveId()
{
	return this->m_ulSlaveId;
}

Slave::Slave(unsigned long ulSlaveId) :
	m_pDispatcher(nullptr), m_pTaskExectutor(nullptr)
{
	this->m_ulSlaveId = ulSlaveId != ~0ul ? ulSlaveId : ~0ul;
}
Slave::~Slave()
{
	DELETE_PTR(this->m_pTaskExectutor); //call TaskQueue dtor
	DELETE_PTR(this->m_pDispatcher);	//call SlaveDispatcher dtor
}

void Slave::CreateDispatcher()
{
	if (!this->m_pDispatcher)
		this->m_pDispatcher = new SlaveDispatcher();
}

void Slave::DestroyDispatcher()
{
	DELETE_PTR(this->m_pDispatcher);
}

SlaveDispatcher* Slave::GetDispatcher()
{
	return this->m_pDispatcher;
}

void Slave::CreateTaskExecutor()
{
	if (!this->m_pTaskExectutor)
		this->m_pTaskExectutor = new TaskExecutor();
}

void Slave::DestroyTaskExecutor()
{
	DELETE_PTR(this->m_pTaskExectutor);
}

TaskExecutor* Slave::GetTaskExecutor()
{
	return this->m_pTaskExectutor;
}


