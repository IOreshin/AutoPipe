#pragma once
#include "pch.h"
#include "AutoPipeDrawing.h"
#include "AutoPipe.h"
#include "KompasUtility.h"
#include <comdef.h>
#include <comutil.h>  // Для работы с BSTR
#include <afxstr.h>    // Для CString
#include <vector>
#include <utility>
#include <array>
#include "PipelineSolver.h"


using namespace std;

// Внешние переменные
extern KompasObjectPtr pKompas_5;		// Указатель на интерфейс КОМПАС в API5
extern IApplicationPtr pKompas_7;		// Указатель на интерфейс КОМПАС в API7
extern CAutoPipeApp theApp;


array<double, 3> start_point_coordinates = {NAN, NAN, NAN};

int WINAPI CALLBACKPOINT(LPDISPATCH _Entity, LPDISPATCH _RequestInfo)
{
	int res = 0;
	try
	{
		ksRequestInfo3DPtr info(_RequestInfo);
		ksEntityPtr pEntity(_Entity);
		ksEntityCollectionPtr pCollection;

		if (pEntity)
		{
			pCollection = info->GetEntityCollection();
			if (pEntity->type == o3d_vertex)
			{
				try
				{
					//ksVertexDefinitionPtr ksVertex = pCollection->SetByIndex(pEntity, 0);
					ksVertexDefinitionPtr ksVertex = pEntity->GetDefinition();
					double x, y, z;
					ksVertex->GetPoint(&x, &y, &z);
					start_point_coordinates = { x, y, z };
					res = 1;
				}

				catch (...)
				{
					pKompas_5->ksMessage(_T("Ошибка"));
				}
			}
		}
		if (res == 0)
			pCollection->SetByIndex(0, 0);
	}
	catch (...)
	{
		return 0;
	}
	return res;
}

void getRequestInfo()
{
	ksDocument3DPtr pDocument3D = pKompas_5->ActiveDocument3D();
	ksPartPtr pPart = pDocument3D->GetPart(pTop_Part);
	ksRequestInfo3DPtr pRequestInfo = pDocument3D->GetRequestInfo(pPart);

	pRequestInfo->title = _T("Запрос позиции размещения");
	pRequestInfo->prompt = _T("Укажите точку или грань на торце трубы");
	

	pRequestInfo->SetCallBackEx(_T("CALLBACKPOINT"), PtrToLong(theApp.m_hInstance), 0);

	pDocument3D->UserGetPlacementAndEntity(1);
	if (!isnan(start_point_coordinates[0]))
	{
		PipelineSolver solver(false, start_point_coordinates);
		solver.getPipelineSolution();
		start_point_coordinates = { NAN, NAN, NAN };
	}
	else
	{
		pKompas_5->ksMessage(_T("Выделите точку"));
	}
}













