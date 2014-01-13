#pragma once

#include <io.h>
#include <fcntl.h>

class open_console
{
public:
	open_console()
	{
		AllocConsole();//在win32程序里申请一个新的控制台

		HANDLE hin = ::GetStdHandle(STD_INPUT_HANDLE);//得到输入句柄
		HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);//得到输出句柄

		int hcin = _open_osfhandle((intptr_t)hin,_O_TEXT);
		FILE* fpin = _fdopen(hcin,"r");
		*stdin = *fpin; //打开输入句柄 并赋值给标准输入库

		int hcout = _open_osfhandle((intptr_t)hout,_O_TEXT);
		FILE* fpout = _fdopen(hcout,"wt");
		*stdout = *fpout; //打开输出句柄 并赋值给标准输出库

		std::ios_base::sync_with_stdio();
	}

	~open_console()
	{
		FreeConsole();
	}
};