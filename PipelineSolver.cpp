#pragma once
#include <iostream>
#include <string>
#include <atlbase.h>
#include "pch.h"
#include "resource.h"
#include "AutoPipe.h"
#include "KompasUtility.h"
#include <comdef.h>
#include <comutil.h>  // ��� ������ � BSTR
#include <afxstr.h>    // ��� CString
#include <vector>
#include <utility>
#include <array>
#include "PipelineSolver.h"

using namespace std;

// ������� ����������
extern KompasObjectPtr pKompas_5;		// ��������� �� ��������� ������ � API5
extern IApplicationPtr pKompas_7;		// ��������� �� ��������� ������ � API7
extern CAutoPipeApp theApp;

PipelineSolver::PipelineSolver(bool useAutomaticStart, array<double, 3> userStartPoint)
{
    if (useAutomaticStart) {
        // ���� ����� ������������� ���������� ��������� �����
        start_point = defineFirstPoint();
    }
    else {
        // ���� ����� ���������� �������
        start_point = userStartPoint;
    }

    if (!isnan(start_point[0]))
    {
        pipiline_dots.push_back(start_point);  // ��������� ������ ����� � ������
        getLinesCoords();  // �������� ���������� �����
        findPipeline();  // ������� ����������
    }
}

void PipelineSolver::getPipelineSolution()
{
    printPipelineDots();  // �������� ���������
}

// ����� ��� ��������� ������� �����
vector <array<double, 3>> PipelineSolver::getPipelineTrajectory()
{
    return pipiline_dots;
}

// ������� ����������� ���������� ������ �����. ������������ ���������� ������ � ���������
array<double, 3> PipelineSolver::defineFirstPoint()
{
    IKompasDocumentPtr doc;
    pKompas_7->get_ActiveDocument(&doc);
    IKompasDocument3DPtr doc3D(doc);
    ISelectionManagerPtr iSelectMng;
    doc3D->get_SelectionManager(&iSelectMng);
    VARIANT selectedObjects;
    VariantInit(&selectedObjects);
    iSelectMng->get_SelectedObjects(&selectedObjects);
    iSelectMng->UnselectAll();

    if (selectedObjects.vt != VT_DISPATCH) {
        pKompas_5->ksMessage(_T("������ ��������� �������"));
        return {NAN,NAN,NAN};
    }

    IDispatchPtr iVertexDisp = selectedObjects.pdispVal;
    IVertexPtr iVertex;
    iVertexDisp->QueryInterface(__uuidof(IVertexPtr), (void**)&iVertex);
    if (iVertex)
    {
        double x, y, z;
        VARIANT_BOOL result;
        
        auto vertex_coords = iVertex->raw_GetPoint(&x, &y, &z, &result);
        /*if (result != VARIANT_TRUE)
        {
            pKompas_5->ksMessage(_T("������ ��������� ���������� �����"));
            return { NAN,NAN,NAN };
        }*/
        return { x, y, z, };
    }
    else
    {
        pKompas_5->ksMessage(_T("�������� ��� ����������� �������"));
        return { NAN, NAN, NAN };
    }
    return { NAN,NAN,NAN };
}

// ������� ��������� ��������� ���� ������ IEdge � ���������
void PipelineSolver::getLinesCoords()
{
    HRESULT hr;
    ksDocument3DPtr doc3D_5 = pKompas_5->ActiveDocument3D();
    ksPartPtr part5 = doc3D_5->GetPart(pTop_Part);
    ksEntityCollectionPtr entityCollection = part5->EntityCollection(o3d_edge);
    if (!entityCollection)
    {
        return;
    }
    long entityCount = entityCollection->GetCount();
    for (long i = 0; i < entityCount; ++i)
    {
        ksEntityPtr entity = entityCollection->GetByIndex(i);
        IDispatchPtr edgeDispatch = entity->GetDefinition();
        ksEdgeDefinitionPtr edgeDefinition;
        hr = edgeDispatch->QueryInterface(__uuidof(ksEdgeDefinition), (void**)&edgeDefinition);
        if (!edgeDefinition && FAILED(hr))
        {
            pKompas_5->ksMessage(_T("������ �������������� � ksEdgeDefinition"));
            return;
        }

        if (edgeDefinition->IsStraight())
        {
            IDispatchPtr curveDispatch = edgeDefinition->GetCurve3D();
            ksCurve3DPtr curve3D;
            hr = curveDispatch->QueryInterface(__uuidof(ksCurve3D), (void**)&curve3D);
            if (!curve3D && FAILED(hr))
            {
                pKompas_5->ksMessage(_T("������ �������������� � Curve3D"));
                return;
            }
            double x1, y1, z1, x2, y2, z2;
            curve3D->GetGabarit(&x1, &y1, &z1, &x2, &y2, &z2);
            lines_coords.push_back({ x1, y1, z1, x2, y2, z2 });
        }
    }
}

// ����������� ������� ������ ���������� ����������
void PipelineSolver::findPipeline()
{
    bool found = true;
    while (found)
    {
        found = false;
        for (auto it = lines_coords.begin(); it != lines_coords.end(); ++it)
        {
            auto first_point = array<double, 3>{ (*it)[0], (*it)[1], (*it)[2] };
            auto second_point = array<double, 3>{ (*it)[3], (*it)[4], (*it)[5] };
            if (second_point == start_point)
            {
                pipiline_dots.push_back(first_point);
                start_point = first_point;
                lines_coords.erase(it);
                found = true;
                break;
            }
            else if (first_point == start_point)
            {
                pipiline_dots.push_back(second_point);
                start_point = second_point;
                lines_coords.erase(it);
                found = true;
                break;
            }
        }
    }
}

// ����� ��� ������ �����
void PipelineSolver::printPipelineDots()
{
    for (const auto& dot : pipiline_dots)
    {
        if (dot[0] != NAN)
        {
            CString message;
            message.Format(_T("X: %f, Y: %f, Z: %f"), dot[0], dot[1], dot[2]);
            _bstr_t messageBSTR = _bstr_t(message);
            pKompas_5->ksMessage(messageBSTR);
        }
    }
}
