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
#include <cmath>
#include <sstream>
#include <algorithm>

#ifndef M_PI
    #define M_PI 3.1415926535  // Определение M_PI, если она не определена
#endif

using namespace std;

// Внешние переменные
extern KompasObjectPtr pKompas_5;		// Указатель на интерфейс КОМПАС в API5
extern IApplicationPtr pKompas_7;		// Указатель на интерфейс КОМПАС в API7



PipelineSolver::PipelineSolver(array<double, 3> userStartPoint)
{
    start_point = userStartPoint;
    for (size_t i = 0; i < 3; ++i)
    {
        start_point[i] = roundToThreeDecimalPlaces(start_point[i]);
    }
    zero_point = start_point;
    if (!isnan(start_point[0]))
    {
        pipeline_dots.push_back(start_point);  // Добавляем первую точку в массив
        getMacroObjectsNames();
        if (!macro_objects_names.empty())
        {
            getLinesCoords();  // Получаем координаты линий
            findPipeline();  // Находим траекторию
            getRelativePointCoordinates(); // Заполняем массив смещенных координат точек
            getZeroDotsCoords();
            calculateBendParams();
            formBendInformation();
        }
    }
}

void PipelineSolver::getMacroObjectsNames()
{
    IKompasDocumentPtr doc;
    pKompas_7->get_ActiveDocument(&doc);
    IKompasDocument3DPtr doc3D(doc);
    IPart7Ptr iPart;
    doc3D->get_TopPart(&iPart);
    IModelContainerPtr iModelContainer(iPart);
    IMacroObjects3DPtr iMacroObjects;
    iModelContainer->get_MacroObjects3D(&iMacroObjects);
    long macroCount;
    iMacroObjects->get_Count(&macroCount);
    if (macroCount != 0)
    {
        for (size_t i = 0; i < macroCount; i++)
        {
            VARIANT index;
            VariantInit(&index);
            index.vt = VT_I4;
            index.lVal = i;
            IMacroObject3DPtr iMacroObject;
            iMacroObjects->get_MacroObject3D(index, &iMacroObject);

            VARIANT macroObjects;
            VariantInit(&macroObjects);
            iMacroObject->get_Objects(&macroObjects);
            if (macroObjects.vt == VT_DISPATCH)
            {
                IDispatch* pObject = V_DISPATCH(&macroObjects);
                IUserObject3DPtr iUserObj;
                pObject->QueryInterface(__uuidof(IUserObject3D), (void**)&iUserObj);
                if (iUserObj != nullptr)
                {
                    BSTR objName;
                    iUserObj->get_Name(&objName);
                    wstring objName_w(objName, SysStringLen(objName));
                    auto it = find(macro_objects_names.begin(), macro_objects_names.end(),
                        objName_w);
                    if (it == macro_objects_names.end())
                    {
                        macro_objects_names.push_back(objName_w);
                    }
                }
            }
            if (macroObjects.vt & VT_DISPATCH && macroObjects.vt & VT_ARRAY)
            {
                SAFEARRAY* pArray = V_ARRAY(&macroObjects);
                LONG lBound, uBound;
                SafeArrayGetLBound(pArray, 1, &lBound);
                SafeArrayGetUBound(pArray, 1, &uBound);

                for (LONG i = lBound; i <= uBound; ++i)
                {
                    IDispatch* pObject = nullptr;
                    SafeArrayGetElement(pArray, &i, &pObject);
                    if (pObject != nullptr)
                    {
                        IUserObject3DPtr iUserObj;
                        pObject->QueryInterface(__uuidof(IUserObject3D), (void**)&iUserObj);
                        if (iUserObj != nullptr)
                        {
                            BSTR objName;
                            iUserObj->get_Name(&objName);
                            wstring objName_w(objName, SysStringLen(objName));
                            auto it = find(macro_objects_names.begin(), macro_objects_names.end(),
                                objName_w);
                            if (it == macro_objects_names.end())
                            {
                                macro_objects_names.push_back(objName_w);
                            }
                        }
                    }
                }
            }
            VariantClear(&macroObjects);
            VariantClear(&index);
        }
    }

}

// Метод для получения массива точек
vector <array<double, 8>> PipelineSolver::getPipelineTrajectory()
{
    return bendInfo;
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
        edgeDispatch->QueryInterface(__uuidof(ksEdgeDefinition), (void**)&edgeDefinition);

        if (edgeDefinition->IsStraight())
        {
            ksEntityPtr iEntityOwner = edgeDefinition->GetOwnerEntity();
            BSTR entity_name = iEntityOwner->name;
            wstring entity_name_w(entity_name, SysStringLen(entity_name));

            auto it = find(macro_objects_names.begin(),
                macro_objects_names.end(), entity_name_w);
            
            if (it != macro_objects_names.end())
            {
                IDispatchPtr curveDispatch = edgeDefinition->GetCurve3D();
                ksCurve3DPtr curve3D;
                curveDispatch->QueryInterface(__uuidof(ksCurve3D), (void**)&curve3D);
                double x1, y1, z1, x2, y2, z2;

                ksLineSeg3dParamPtr iLineSeg = curve3D->GetCurveParam();
                iLineSeg->GetPointFirst(&x1, &y1, &z1);
                iLineSeg->GetPointLast(&x2, &y2, &z2);

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
}

bool PipelineSolver::arePointsEqual(const array<double, 3>& p1, const array<double, 3>& p2, double epsilon = 1e-3)
{
    return fabs(p1[0] - p2[0]) < epsilon &&
        fabs(p1[1] - p2[1]) < epsilon &&
        fabs(p1[2] - p2[2]) < epsilon;
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
            if (arePointsEqual(second_point, start_point))
            {
                pipeline_dots.push_back(first_point);
                start_point = first_point;
                lines_coords.erase(it);
                found = true;
                break;
            }
            else if (arePointsEqual(first_point, start_point))
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

// Функция приведения точек к нулевой координате
void PipelineSolver::getZeroDotsCoords()
{
    for (size_t i = 0; i < pipeline_dots.size(); ++i)
    {
        if (i == 0)
        {
            zero_dots_coords.push_back({ 0,0,0 });
        }
        else
        {
            zero_dots_coords.push_back({
                pipeline_dots[i][0] - zero_point[0],
                pipeline_dots[i][1] - zero_point[1],
                pipeline_dots[i][2] - zero_point[2]
                });
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

double PipelineSolver::calculateDistance(const array <double, 3> &p1, const array <double, 3> &p2)
{
    return sqrt(
        pow(p2[0]-p1[0], 2)+
        pow(p2[1]-p1[1], 2)+
        pow(p2[2]-p1[2], 2)
    );
};

double PipelineSolver::calculateAngleBetweenVectors(const array<double, 3>& v1, const array<double, 3>& v2)
{
    double dot_product = 0;
    double mag_v1 = 0, mag_v2 = 0;

    for (size_t i = 0; i < 3; ++i) {
        dot_product += v1[i] * v2[i];
        mag_v1 += v1[i] * v1[i];
        mag_v2 += v2[i] * v2[i];
    }

    mag_v1 = sqrt(mag_v1);
    mag_v2 = sqrt(mag_v2);

    if (mag_v1 == 0 || mag_v2 == 0) return 0;

    double cos_theta = dot_product / (mag_v1 * mag_v2);
    cos_theta = max(min(cos_theta, 1.0), -1.0);

    return acos(cos_theta) * (180.0 / M_PI);
};

double PipelineSolver::calculateAngleBetweenPlanes(const array<double, 3>& n1, const array<double, 3>& n2)
{
    double dot_product = 0, mag_n1 = 0, mag_n2 = 0;

    for (size_t i = 0; i < 3; ++i) {
        dot_product += n1[i] * n2[i];
        mag_n1 += n1[i] * n1[i];
        mag_n2 += n2[i] * n2[i];
    }

    mag_n1 = sqrt(mag_n1);
    mag_n2 = sqrt(mag_n2);

    if (mag_n1 == 0 || mag_n2 == 0) return 0;

    double cos_theta = dot_product / (mag_n1 * mag_n2);
    cos_theta = max(min(cos_theta, 1.0), -1.0);

    return acos(cos_theta) * (180.0 / M_PI);
};

array <double, 3> PipelineSolver::getCrossProduct(const array<double, 3>& v1, const array<double, 3>& v2)
{
    return {
        v1[1] * v2[2] - v1[2] * v2[1],
        v1[2] * v2[0] - v1[0] * v2[2],
        v1[0] * v2[1] - v1[1] * v2[0]
    };
};

array <double, 3> PipelineSolver::normalizeVector(const array<double, 3>& v)
{
    double mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (mag != 0) {
        return { v[0] / mag, v[1] / mag, v[2] / mag };
    }
    return { 0, 0, 0 };
};

void PipelineSolver::calculateBendParams()
{
    array<double, 3> previous_normal = { 0.0, 0.0, 0.0 };

    for (size_t i = 1; i < zero_dots_coords.size() - 1; ++i) {
        double total_distance = calculateDistance(zero_dots_coords[i - 1], zero_dots_coords[i]);
        array<double, 3> v1, v2;

        for (size_t j = 0; j < 3; ++j) {
            v1[j] = zero_dots_coords[i][j] - zero_dots_coords[i - 1][j];
            v2[j] = zero_dots_coords[i + 1][j] - zero_dots_coords[i][j];
        }

        double C = calculateAngleBetweenVectors(v1, v2);
        array<double, 3> current_normal = normalizeVector(getCrossProduct(v1, v2));
        double B = 0;

        if (i > 1) {
            B = calculateAngleBetweenPlanes(previous_normal, current_normal);
            if (getCrossProduct(previous_normal, current_normal)[2] < 0) B = -B;
        }

        previous_normal = current_normal;
        double arc_length = bendRadius * tan((C * M_PI) / 360.0);
        double offset = 0;

        if (i == 1) {
            offset = total_distance - arc_length;
        }
        else {
            double arc_length_next = bendRadius * tan((bendParams[i - 2][2] * M_PI) / 360.0);
            offset = total_distance - arc_length - arc_length_next;
        }

        bendParams.push_back({ offset, B, C });
    }

    double Y_last = calculateDistance(
        zero_dots_coords[zero_dots_coords.size() - 2],
        zero_dots_coords[zero_dots_coords.size() - 1]
    ) - bendRadius * tan((bendParams.back()[2] * M_PI) / 360.0);

    bendParams.push_back({ Y_last, 0, 0 });
}

void PipelineSolver::formBendInformation()
{
    bendInfo.clear();

    for (size_t i = 0; i <= bendParams.size(); ++i)
    {
        array <double, 8> bendData;

        if (i == 0)
        {
            bendData = { 1,0,0,0,0,0,0,0 };
        }
        else
        {
            bendData[0] = i+1;
            bendData[1] = relativeCoordinates[i][0];
            bendData[2] = relativeCoordinates[i][1];
            bendData[3] = relativeCoordinates[i][2];
            bendData[4] = bendRadius;  // BEND RADIUS
            bendData[5] = round(bendParams[i - 1][2]); // BEND ANGLE
            bendData[6] = round(bendParams[i - 1][1]); // TWIST ANGLE
            bendData[7] = round(bendParams[i - 1][0]); // OFFSET
        }

        // Добавляем информацию о гибе в bendInfo
        bendInfo.push_back(bendData);
    }
}
