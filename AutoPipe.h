// AutoPipe.h: основной файл заголовка для библиотеки DLL AutoPipe
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CAutoPipeApp
// Реализацию этого класса см. в файле AutoPipe.cpp
//

class CAutoPipeApp : public CWinApp
{
public:
	CAutoPipeApp();

	DECLARE_MESSAGE_MAP()
};
