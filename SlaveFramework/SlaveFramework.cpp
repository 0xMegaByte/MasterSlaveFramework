// MasterSlaveFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Slave.h"

int main()
{
	Slave* pSlave = new Slave(1);
	pSlave->PushTask(new Task(1001));
	Task* pTask = pSlave->PopTask();
	printf("Task Id: %lu", pTask->GetTaskId());

	//TODO: Add communication Master-Slave / Slave-Slave
}