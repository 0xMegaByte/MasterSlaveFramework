#pragma once
#include <deque>
#include <unordered_map>
#include <mutex>
//#include <windows.h>

#define DELETE_PTR(ptr) if(ptr){decltype(ptr) tmptr = ptr; ptr = nullptr; delete tmptr;}
#define BUF_LEN 512 //(0x200)



//#ifndef __time__
//#define __time__ ctime_s(t,sizeof(t),&g_time)
//#endif

// Define __FILENAME__ if it's not already defined (assuming it's not provided by the compiler)
#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) //'\\' for WIN
#endif

//TODO: Add time
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(format, ...) printf("[%s:%s]: " format, __FILENAME__, __func__, ##__VA_ARGS__)
#endif

#ifndef DEBUG_PRINT_CLS
#define DEBUG_PRINT_CLS(format, ...) printf("[%s:<%s::%s>]: " format, __FILENAME__,typeid(*this).name(), __func__, ##__VA_ARGS__)
#endif // DEBUG_PRINT_CLS

void PrintWelcomeMessage();