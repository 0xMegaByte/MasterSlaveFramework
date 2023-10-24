#pragma once

#define DELETE_PTR(ptr) if(ptr){decltype(ptr) tmptr = ptr; ptr = nullptr; delete tmptr;}

#define BUF_LEN 512