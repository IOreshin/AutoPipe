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
#include "AutoPipeConstants.h"
#include <sstream>
#include <string>
#include <atlbase.h>
#include <iomanip>

using namespace std;

// Внешние переменные
extern KompasObjectPtr pKompas_5;		// Указатель на интерфейс КОМПАС в API5
extern IApplicationPtr pKompas_7;		// Указатель на интерфейс КОМПАС в API7
extern CAutoPipeApp theApp;


//изменение формата листа
void ChangeLayoutSheet(IKompasDocumentPtr doc)
{
	ILayoutSheetsPtr iSheets;
	doc->get_LayoutSheets(&iSheets);
	ILayoutSheetPtr iSheet;
	iSheets->get_ItemByNumber(1, &iSheet);
	if (iSheet)
	{
		ISheetFormatPtr iSheetFormat;
		iSheet->get_Format(&iSheetFormat);
		iSheetFormat->put_Format(ksFormatA3);
		iSheetFormat->put_VerticalOrientation(VARIANT_FALSE);
		iSheet->Update();
	}
}

//создание вида в чертеже
IViewPtr CreateTubeView(IKompasDocumentPtr doc, IBody7Ptr Body7, BSTR pathName)
{
	IKompasDocument2DPtr doc2D(doc);
	IKompasDocument2D1Ptr doc2D1(doc2D);
	IViewsAndLayersManagerPtr iViewsMng;

	doc2D->get_ViewsAndLayersManager(&iViewsMng);
	IViewsPtr iViews;
	iViewsMng->get_Views(&iViews);

	IViewPtr iView;
	iViews->raw_Add(vt_Arbitrary, &iView);
	iView->Update();

	IAssociationViewPtr iAssoView(iView);
	iAssoView->put_SourceFileName(pathName);
	iView->Update();
	VARIANT variant;
	VariantInit(&variant);

	variant.vt = VT_DISPATCH;
	variant.pdispVal = Body7.GetInterfacePtr();

	_variant_t var = _variant_t(variant);

	iAssoView->SetProjectionObjects(var);
	VariantClear(&variant);

	CString projection_Name_c = _T("#Изометрия");
	BSTR projectionName = projection_Name_c.AllocSysString();
	iAssoView->put_ProjectionName(projectionName);

	iView->Update();
	doc2D1->RebuildDocument();
	return iView;
}

ViewGabarits GetViewGabarit(IViewPtr iView, gabaritType gabarit)
{
	ksDocument2DPtr doc2D_5 = pKompas_5->ActiveDocument2D();
	StructType2DEnum structType1 = ko_RectParam;

	IDispatchPtr pRectStruct = pKompas_5->GetParamStruct(ko_RectParam);
	ksRectParam* pRect = nullptr;
	pRectStruct->QueryInterface(__uuidof(ksRectParam), (void**)&pRect);

	long point = 0;
	iView->get_Reference(&point);

	doc2D_5->ksGetObjGabaritRect(point, pRect);

	//нижняя левая точка
	ksMathPointParamPtr pBot = pRect->GetpBot();
	double x_bot = pBot->Getx();
	double y_bot = pBot->Gety();

	ksMathPointParamPtr pTop = pRect->GetpTop();
	double x_top = pTop->Getx();
	double y_top = pTop->Gety();

	ViewGabarits result;

	if (gabarit == GABARIT_SIZE)
	{
		result.x_gab = x_top - x_bot;
		result.y_gab = y_top - y_bot;
	}
	else if (gabarit == GABARIT_POINTS)
	{
		result.x_bot = x_bot;
		result.y_bot = y_bot;
		result.x_top = x_top;
		result.y_top = y_top;
	}
	return result;
}


void ViewSettings(IViewPtr iView)
{
	CString view_name_cstr = _T("Главный изометрический вид");  // Строка в формате CString
	BSTR view_name = view_name_cstr.AllocSysString();
	iView->put_Name(view_name);
	IViewDesignationPtr iViewDesignation(iView);
	iViewDesignation->put_ShowScale(VARIANT_FALSE);

	iView->Update();

	auto viewGabarit = GetViewGabarit(iView, GABARIT_SIZE);
	double max_gab = 180.0;
	double viewScale = 1;
	if (viewGabarit.x_gab > viewGabarit.y_gab)
	{
		viewScale = 1/(viewGabarit.x_gab/max_gab);
	}
	if (viewGabarit.x_gab < viewGabarit.y_gab)
	{
		viewScale = 1/(viewGabarit.y_gab/max_gab);
	}

	iView->put_Scale(viewScale);
	iView->Update();

	auto viewGabarit1 = GetViewGabarit(iView, GABARIT_POINTS);
	double x_centr = viewGabarit1.x_bot+(viewGabarit1.x_top - viewGabarit1.x_bot)/2;
	double y_centr = viewGabarit1.y_bot+(viewGabarit1.y_top - viewGabarit1.y_bot)/2;

	double x_offset = x_centr - 140;
	double y_offset = y_centr - 170;

	double x, y;
	iView->get_X(&x);
	iView->get_Y(&y);
	double x_fin = x - x_offset;
	double y_fin = y - y_offset;
	iView->put_X(x_fin);
	iView->put_Y(y_fin);
	iView->Update();
}

double roundToTwoDecimalPlaces(double value) {
	return round(value * 10.0) / 10.0;
}

void CreateBendTable(vector <array<double, 8>> pipelineTrajectory, IKompasDocumentPtr doc)
{
	IKompasDocument2DPtr doc2D(doc);
	IKompasDocument2D1Ptr doc2D1(doc2D);
	IViewsAndLayersManagerPtr iViewsMng;

	doc2D->get_ViewsAndLayersManager(&iViewsMng);
	IViewsPtr iViews;
	iViewsMng->get_Views(&iViews);

	IViewPtr iView;
	iViews->raw_Add(vt_Normal, &iView);

	CString view_name_cstr = _T("Таблица гибов");  // Строка в формате CString
	BSTR view_name = view_name_cstr.AllocSysString();
	iView->put_Name(view_name);

	iView->put_X(415);
	iView->put_Y(292);

	iView->Update();

	ISymbols2DContainerPtr iSymContainer(iView);
	IDrawingTablesPtr iDrwTables;
	iSymContainer->get_DrawingTables(&iDrwTables);
	IDrawingTablePtr iDrwTable;

	long rowsCount = pipelineTrajectory.size()+1;
	long colsCount = 8;
	long rowHeight = 8;
	long colWidth = 25;

	iDrwTables->raw_Add(rowsCount, colsCount, rowHeight, colWidth, ksTableTileLayoutEnum::ksTTLNotCreate, &iDrwTable);


	iDrwTable->put_X(double( - colsCount * colWidth));
	iDrwTable->Update();

	ITablePtr iTable(iDrwTable);
	BSTR titles[] = {L"N", L"X", L"Y", L"Z" , L"BEND RADIUS", L"BEND ANGLE", L"BEND TWIST", L"OFFSET"};
	//СТРОКИ
	for (long i = 0; i < rowsCount; ++i)
	{
		//СТОЛБЦЫ
		for (long n = 0; n < colsCount; ++n)
		{
			ITableCellPtr iCell;
			iTable->get_Cell(i, n, &iCell);
			IKompasAPIObjectPtr iKomObj;
			iCell->get_Text(&iKomObj);
			ITextPtr iText = iKomObj;
			if (i == 0)
			{
				iText->put_Str(titles[n]);
			}
			else
			{
				array<double, 8> coords = pipelineTrajectory[i - 1];
				double coord = coords[n];

				// Округление до 1 знака после запятой
				std::wostringstream stream;
				stream << std::fixed << std::setprecision(1) << coord;
				// Преобразование в BSTR
				CComBSTR bstrCoord(stream.str().c_str());
				iText->put_Str(bstrCoord);
			}

		}
	}
	iDrwTable->Update();

}


void CreateBodyDrawing(IBody7Ptr Body7, BSTR pathName, vector <array<double, 8>> pipelineTrajectory)
{
	HRESULT hr;
	IDocumentsPtr iDocuments;
	pKompas_7->get_Documents(&iDocuments);
	IKompasDocumentPtr doc;
	DocumentTypeEnum doc_type = ksDocumentDrawing;
	VARIANT_BOOL visible = VARIANT_TRUE;
	
	hr = iDocuments->raw_Add(doc_type, visible, &doc);
	if (FAILED(hr))
	{
		pKompas_5->ksMessage(_T("Ошибка создания чертежа"));
		return;
	}

	ChangeLayoutSheet(doc);
	IViewPtr iView = CreateTubeView(doc, Body7, pathName);
	ViewSettings(iView);
	CreateBendTable(pipelineTrajectory, doc);
}

