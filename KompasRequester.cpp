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
#include <cmath>


using namespace std;

// Внешние переменные
extern KompasObjectPtr pKompas_5;		// Указатель на интерфейс КОМПАС в API5
extern IApplicationPtr pKompas_7;		// Указатель на интерфейс КОМПАС в API7
extern CAutoPipeApp theApp;


array<double, 3> start_point_coordinates = {NAN, NAN, NAN};
IBody7Ptr selectedBody = nullptr;

void getBodyOwner(IFeature7* owner)
{
	VARIANT ResultBodies;
	VariantInit(&ResultBodies);

	owner->get_ResultBodies(&ResultBodies);
	if (ResultBodies.vt == VT_DISPATCH)
	{
		IBody7Ptr body7 = ResultBodies.pdispVal;
		if (body7)
		{
			selectedBody = body7;
		}
	}
}

double roundToThreeDecimalPlaces(double value) {
	return round(value * 1000.0) / 1000.0;
}

void getEdgeCenter(IEdgePtr iEdge)
{
	IMathCurve3DPtr iMathCurve;
	iEdge->get_MathCurve(&iMathCurve);
	double x, y, z;
	iMathCurve->GetCentre(&x, &y, &z);
	x = roundToThreeDecimalPlaces(x);
	y = roundToThreeDecimalPlaces(y);
	z = roundToThreeDecimalPlaces(z);
	start_point_coordinates = { x, y, z };
}

void getSelectedObjectParams(IFacePtr iFace = nullptr, IEdgePtr iEdge = nullptr)
{
	VARIANT_BOOL result;
	if (iFace!=nullptr)
	{
		try
		{
			IFeature7* owner = nullptr;
			iFace->get_Owner(&owner);
			if (owner)
			{
				getBodyOwner(owner);
				VARIANT connectedFaces;
				VariantInit(&connectedFaces);
				iFace->get_ConnectedFaces(&connectedFaces);
				if ((connectedFaces.vt & VT_ARRAY) && (connectedFaces.vt & VT_DISPATCH))
				{
					SAFEARRAY* faces = connectedFaces.parray;
					LONG lBound, uBound;
					SafeArrayGetLBound(faces, 1, &lBound);
					SafeArrayGetUBound(faces, 1, &uBound);
					for (LONG i = lBound; i <= uBound; ++i)
					{
						IDispatch* iFaceDisp = nullptr;
						SafeArrayGetElement(faces, &i, &iFaceDisp);
						if (iFaceDisp)
						{
							IFacePtr iFaceTorec = iFaceDisp;
							VARIANT_BOOL result;
							iFaceTorec->get_IsPlanar(&result);
							if (result == TRUE)
							{
								VARIANT limitingEdges;
								VariantInit(&limitingEdges);
								iFaceTorec->get_LimitingEdges(&limitingEdges);
								if ((limitingEdges.vt & VT_ARRAY) && (limitingEdges.vt & VT_DISPATCH))
								{

									SAFEARRAY* edges = limitingEdges.parray;
									LONG edgeLBound, edgeUBound;
									SafeArrayGetLBound(edges, 1, &edgeLBound);
									SafeArrayGetUBound(edges, 1, &edgeUBound);
									for (LONG i = edgeLBound; i <= edgeUBound; ++i)
									{
										IDispatch* iEdgeDisp = nullptr;
										SafeArrayGetElement(edges, &i, &iEdgeDisp);
										if (iEdgeDisp)
										{
											IEdgePtr iEdge = iEdgeDisp;
											getEdgeCenter(iEdge);
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			pKompas_5->ksMessage(_T("Ошибка FACE"));
		}
	}
	else
	{
		IFeature7* owner = nullptr;
		iEdge->get_Owner(&owner);
		if (owner)
		{
			getBodyOwner(owner);
			getEdgeCenter(iEdge);
			return;
		}
	}
	return;
}

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
			if (pEntity->type == o3d_face || pEntity->type == o3d_edge)
			{
				try
				{
					if (pEntity->type == o3d_face)
					{
						res = 1;
						ksFaceDefinitionPtr ksFace = pEntity->GetDefinition();
						LPUNKNOWN iFace = pKompas_5->TransferInterface(ksFace, ksAPITypeEnum::ksAPI7Dual, 0);
						if (iFace)
						{
							getSelectedObjectParams(iFace);
						}

					}
					else if (pEntity->type == o3d_edge)
					{
						res = 1;
						ksEdgeDefinitionPtr ksEdge = pEntity->GetDefinition();
						LPUNKNOWN iEdge = pKompas_5->TransferInterface(ksEdge, ksAPITypeEnum::ksAPI7Dual, 0);
						if (iEdge)
						{
							getSelectedObjectParams(nullptr, iEdge);
						}
					}
					else res = -1;
				}

				catch (...)
				{
					pKompas_5->ksMessage(_T("Ошибка"));
					res = -1;
				}
			}
		}
		if (res == 0)
			pCollection->SetByIndex(0, 0);
	}
	catch (...)
	{
		return -1;
	}
	return res;
}

void startCreatingDrawing(vector <array<double, 3>> pipelineTrajectory)
{
	IKompasDocumentPtr doc;
	pKompas_7->get_ActiveDocument(&doc);
	BSTR pathName;
	doc->get_PathName(&pathName);
	CreateBodyDrawing(selectedBody, pathName, pipelineTrajectory);
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

	if (selectedBody != nullptr)
	{
		if (!isnan(start_point_coordinates[0]))
		{
			PipelineSolver solver(false, start_point_coordinates);
			vector <array<double, 3>> pipilineTrajectory = solver.getPipelineTrajectory();
			startCreatingDrawing(pipilineTrajectory);
		}
		else
		{
			pKompas_5->ksMessage(_T("Ошибка получения траектории"));
		}

		start_point_coordinates = { NAN, NAN, NAN };
		selectedBody = nullptr;
	}
	else
	{
		pKompas_5->ksMessage(_T("Укажите объект"));
	}

}













