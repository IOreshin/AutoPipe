#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Исключите редко используемые компоненты из заголовков Windows
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // некоторые конструкторы CString будут явными

#include <afxwin.h>         // основные и стандартные компоненты MFC
#include <afxext.h>         // Расширения MFC

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // классы MFC OLE
#include <afxodlgs.h>       // классы диалоговых окон MFC OLE
#include <afxdisp.h>        // классы автоматизации MFC
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // классы баз данных MFC ODBC
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // классы баз данных MFC DAO
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // поддержка MFC для типовых элементов управления Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // поддержка MFC для типовых элементов управления Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning( disable : 4192 )
#pragma warning( disable : 4278 )


#import "ksconstants.tlb"   no_namespace named_guids

#import "ksConstants3D.tlb" no_namespace named_guids


#import "kAPI5.tlb"        no_namespace named_guids rename( "ksFragmentLibrary", "ksIFragmentLibrary" )

#import "kAPI7.tlb"         no_namespace named_guids


#pragma warning( once : 4192 ) 
#pragma warning( once : 4278 ) 
