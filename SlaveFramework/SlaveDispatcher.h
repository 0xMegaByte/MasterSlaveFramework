#pragma once
#include "..\Utils\Communication.h"
#include "..\Utils\Utils.h"
class SlaveDispatcher : private PacketDispatcher
{
	unsigned long __stdcall DispatcherThread(void* lpv) override;
};

