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
#pragma once
#include "Task.h"
#include "SlaveDispatcher.h"
#pragma comment(lib,"Utils.lib")

class Slave
{
private:
	unsigned long m_ulSlaveId;

	TaskExecutor* m_pTaskExectutor;
	SlaveDispatcher* m_pDispatcher;

public:
	Slave(unsigned long ulSlaveId);
	~Slave();

	//Handle Dispatcher
	void CreateDispatcher();
	void DestroyDispatcher();
	SlaveDispatcher* GetDispatcher();

	//Handle TaskExecutor
	void CreateTaskExecutor();
	void DestroyTaskExecutor();
	TaskExecutor* GetTaskExecutor();

	//Utils
	unsigned long GetSlaveId();
};