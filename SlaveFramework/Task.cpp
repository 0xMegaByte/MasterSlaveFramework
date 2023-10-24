#include "Task.h"

unsigned long Task::GetTaskId()
{
	return this->m_ulTaskId;
}

Task::Task(unsigned long m_ulTaskId)
{
	this->m_ulTaskId = m_ulTaskId;
	this->m_uiBufferSize = 1024;
	this->m_pucBuffer = new unsigned char[this->m_uiBufferSize]{0};
}
Task::~Task()
{
	DELETE_PTR(this->m_pucBuffer);
}
