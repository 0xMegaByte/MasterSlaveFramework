#pragma once
#include <deque>
#include <mutex>


#define DELETE_PTR(ptr) if(ptr){decltype(ptr) tmptr = ptr; ptr = nullptr; delete tmptr;}
#define BUF_LEN 512