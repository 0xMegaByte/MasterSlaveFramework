#pragma once

#include "..\Utils\Communication.h"

class Master
{
private:
	unsigned long m_ulTotalSlavesCount;
	unsigned long m_ulCurrentConnections;
public:
	Master();
	~Master();
};