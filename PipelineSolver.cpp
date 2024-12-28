#pragma once
#include <iostream>
#include <string>
#include <atlbase.h>
#include "pch.h"
#include "resource.h"
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



PipelineSolver::PipelineSolver(array<double, 3> userStartPoint)
{
    start_point = userStartPoint;
    if (!isnan(start_point[0]))
    {
        pipeline_dots.push_back(start_point);  // Добавляем первую точку в массив
        getLinesCoords();  // Получаем координаты линий
        findPipeline();  // Находим траекторию
        getRelativePointCoordinates(); // Заполняем массив смещенных координат точек
    }
}

// Метод для получения массива точек
vector <array<double, 3>> PipelineSolver::getPipelineTrajectory()
{
    return relativeCoordinates;
}

// Функция получения координат всех прямых IEdge в документе
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
            pKompas_5->ksMessage(_T("Ошибка преобразования к ksEdgeDefinition"));
            return;
        }

        if (edgeDefinition->IsStraight())
        {
            IDispatchPtr curveDispatch = edgeDefinition->GetCurve3D();
            ksCurve3DPtr curve3D;
            hr = curveDispatch->QueryInterface(__uuidof(ksCurve3D), (void**)&curve3D);
            if (!curve3D && FAILED(hr))
            {
                pKompas_5->ksMessage(_T("Ошибка преобразования к Curve3D"));
                return;
            }
            double x1, y1, z1, x2, y2, z2;
            curve3D->GetGabarit(&x1, &y1, &z1, &x2, &y2, &z2);
            x1 = roundToThreeDecimalPlaces(x1);
            y1 = roundToThreeDecimalPlaces(y1);
            z1 = roundToThreeDecimalPlaces(z1);
            x2 = roundToThreeDecimalPlaces(x2);
            y2 = roundToThreeDecimalPlaces(y2);
            z2 = roundToThreeDecimalPlaces(z2);
            lines_coords.push_back({ x1, y1, z1, x2, y2, z2 });
        }
    }
}

// Рекурсивная функция поиска подходящей траектории
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
                pipeline_dots.push_back(first_point);
                start_point = first_point;
                lines_coords.erase(it);
                found = true;
                break;
            }
            else if (first_point == start_point)
            {
                pipeline_dots.push_back(second_point);
                start_point = second_point;
                lines_coords.erase(it);
                found = true;
                break;
            }
        }
    }
}

//функция определения относителеного смещения координатных точек
void PipelineSolver::getRelativePointCoordinates()
{
    for (size_t i = 0; i < pipeline_dots.size(); ++i)
    {
        if (i == 0)
        {
            relativeCoordinates.push_back({ 0,0,0 });
        }
        else
        {
            array <double, 3> point_movement;
            for (int n = 0; n < 3; ++n)
            {
                point_movement[n] = pipeline_dots[i][n] - pipeline_dots[i - 1][n];
            }
            relativeCoordinates.push_back(point_movement);
        }
    }
}