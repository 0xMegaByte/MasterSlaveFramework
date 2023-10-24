#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"

class MasterDispatcher : PacketDispatcher
{
	unsigned long __stdcall DispatcherThread(void* lpv);
};

