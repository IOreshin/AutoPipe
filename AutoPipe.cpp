// AutoPipe.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "AutoPipe.h"
#include "AutoPipeDrawing.h"
#include "PipelineSolver.h"
#include "KompasRequester.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CAutoPipeApp

BEGIN_MESSAGE_MAP(CAutoPipeApp, CWinApp)
END_MESSAGE_MAP()


// Создание CAutoPipeApp

CAutoPipeApp::CAutoPipeApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CAutoPipeApp

CAutoPipeApp theApp;

KompasObjectPtr pKompas_5(NULL);
IApplicationPtr pKompas_7(NULL);


void GetKompas()
{
    if ((!(bool)pKompas_5) && (!(bool)pKompas_7))
    {
        CString sFileName;
        CString sFileNameLib_5;
        CString sFileNameLib_7;

        if (::GetModuleFileName(NULL, sFileName.GetBuffer(255), 255))
        {
            sFileName.ReleaseBuffer(255);
            CString sLibName_5;
            CString sLibName_7;

            int iLib_5 = sLibName_5.LoadStringW(IDR_AUTO_LIB);  //kAPI5.dll
            int iLib_7 = sLibName_7.LoadStringW(IDR_API7);  //kAPI7.dll

            sFileNameLib_5 = sFileName;
            sFileNameLib_7 = sFileName;
            sFileNameLib_5.Replace(sFileNameLib_5.Right(sFileNameLib_5.GetLength() - (sFileNameLib_5.ReverseFind('\\') + 1)),
                sLibName_5);
            sFileNameLib_7.Replace(sFileNameLib_7.Right(sFileNameLib_7.GetLength() - (sFileNameLib_7.ReverseFind('\\') + 1)),
                sLibName_7);


            HINSTANCE hAppAuto_5 = LoadLibrary(sFileNameLib_5); // идентификатор kAPI5.dll
            HINSTANCE hAppAuto_7 = LoadLibrary(sFileNameLib_7); // идентификатор kAPI7.dll
            if (hAppAuto_5) {
                typedef LPDISPATCH(WINAPI* FCreateKompasObject)();
                FCreateKompasObject pCreateKompasObject =
                    (FCreateKompasObject)GetProcAddress(hAppAuto_5, "CreateKompasObject");
                if (pCreateKompasObject)
                    pKompas_5 = IDispatchPtr(pCreateKompasObject(), false/*AddRef*/);

                FreeLibrary(hAppAuto_5);
            }

            if (hAppAuto_7) {
                typedef LPDISPATCH(WINAPI* FCreateKompasApplication)();
                FCreateKompasApplication pCreateKompasApplication =
                    (FCreateKompasApplication)GetProcAddress(hAppAuto_7, "CreateKompasApplication");
                if (pCreateKompasApplication)
                    pKompas_7 = IDispatchPtr(pCreateKompasApplication(), false/*AddRef*/);

                FreeLibrary(hAppAuto_7);
            }

        }
    }
}


//-------------------------------------------------------------------------------
// Задать идентификатор ресурсов
// ---
unsigned int WINAPI LIBRARYID()
{
    return IDR_MENU;
}

void getPipelineSolution()
{
    PipelineSolver solver;
    solver.getPipelineSolution();
}

//-------------------------------------------------------------------------------
// Головная функция библиотеки
// ---
void WINAPI LIBRARYENTRY(unsigned int comm)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    GetKompas();
    if (pKompas_5)
    {
        try
        {
            switch (comm)
            {
            case IDM_OBJECT: SelectedBodyDrawing(); break;
            case IDM_DOTS: getPipelineSolution(); break;
            case IDM_REQUEST: getRequestInfo(); break;
            }
        }
        catch (...)
        {
        }
    }
}

