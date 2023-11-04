#pragma once
#include <deque>
#include <mutex>

#define DELETE_PTR(ptr) if(ptr){decltype(ptr) tmptr = ptr; ptr = nullptr; delete tmptr;}
#define BUF_LEN 512 //(0x200)

// Define __FILENAME__ if it's not already defined (assuming it's not provided by the compiler)
#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) //'\\' for WIN
#endif

//#ifndef __time__
//#define __time__ ctime_s(t,sizeof(t),&g_time)
//#endif


//TODO: Add time
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(format, ...) printf("[%s:%s]: " format, __FILENAME__, __func__, ##__VA_ARGS__)
#endif



//#define DEBUG_PRINT( str, ... )\
// printf("[%s:%s]: %s %s",__FILENAME__,__FUNCTION__, str, ##__VA_ARGS__)