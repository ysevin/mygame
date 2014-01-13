#pragma once

#include <io.h>
#include <fcntl.h>

class open_console
{
public:
	open_console()
	{
		AllocConsole();//��win32����������һ���µĿ���̨

		HANDLE hin = ::GetStdHandle(STD_INPUT_HANDLE);//�õ�������
		HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);//�õ�������

		int hcin = _open_osfhandle((intptr_t)hin,_O_TEXT);
		FILE* fpin = _fdopen(hcin,"r");
		*stdin = *fpin; //�������� ����ֵ����׼�����

		int hcout = _open_osfhandle((intptr_t)hout,_O_TEXT);
		FILE* fpout = _fdopen(hcout,"wt");
		*stdout = *fpout; //�������� ����ֵ����׼�����

		std::ios_base::sync_with_stdio();
	}

	~open_console()
	{
		FreeConsole();
	}
};