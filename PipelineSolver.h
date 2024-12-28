#pragma once 
#include <comutil.h>  // Для работы с BSTR
#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(array<double, 3> userStartPoint = {NAN, NAN, NAN});  // Конструктор

    vector <array<double, 8>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // Переменная для первой точки
    vector<array<double, 3>> pipeline_dots;  // Массив точек
    vector<wstring> macro_objects_names; // Массив с именами объектов макро
    array<double, 3> zero_point;
    vector<array<double, 3>> zero_dots_coords;  // Массив с учетом обнуления
    vector<array<double, 6>> lines_coords;  // Массив координат линий
    vector <array <double, 3>> relativeCoordinates; //Массив координат относительного смещения точек
    double bendRadius = 75; // значение радиуса гиба трубы
    vector < array<double, 3>> bendParams;
    vector <array<double, 8>> bendInfo;

    void getMacroObjectsNames();
    bool arePointsEqual(const array<double, 3>& p1, const array<double, 3>& p2, double epsilon);
    void getLinesCoords();  // Метод для получения координат всех линий
    void findPipeline();  // Рекурсивная функция для нахождения решения
    void getZeroDotsCoords(); // Функция приведения точек к нулевой координате
    void getRelativePointCoordinates(); //функция определения относительного смещения точек
    void calculateBendParams();
    void formBendInformation();
    double calculateDistance(const array <double, 3> &p1, const array <double, 3> &p2);
    double calculateAngleBetweenVectors(const array<double, 3>& v1, const array<double, 3>& v2);
    double calculateAngleBetweenPlanes(const array<double, 3>& n1, const array<double, 3>& n2);
    array <double, 3> getCrossProduct(const array<double, 3>& v1, const array<double, 3>& v2);
    array <double, 3> normalizeVector(const array<double, 3>& v);
};

